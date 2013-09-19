#include "cavdecoder.h"
#include <QPainter>
extern "C" {

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

}

#include <stdio.h>
#include <QDebug>

using namespace AV;

CAVDecoder::CAVDecoder(QObject *parent) :
    QObject(parent)
{
    m_bIsInited = false;
    m_bShouldPlay = false;

    m_pWorker = new CAVWorker(this);
    m_pWorkerThread = new QThread();
    m_pWorker->moveToThread(m_pWorkerThread);
    m_pWorkerThread->start();

    m_pTimer = new QTimer();
    m_pTimer->setTimerType(Qt::PreciseTimer);
    m_pTimer->setSingleShot(true);
    m_pTimer->setInterval(1);

    m_bEndOfStream = false;

    m_ucDataBuffer = new unsigned char[DATA_BUFFER_SIZE];

    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(nextFrame()));
    connect(this,SIGNAL(needMoreFrames()),m_pWorker,SLOT(decodeFrames()));

    m_iFrameCount = 0;

}

CAVDecoder::~CAVDecoder()
{
    for (int i=0;i<FRAME_BUFFER_SIZE;i++)
        delete m_Frames[i];
    delete m_pIOContext;
    delete m_ucDataBuffer;
}

void CAVDecoder::release()
{
    av_free(m_pSWSContext);
    // Close the codec
    avcodec_close(m_pCodecCtx);

    // Close the video file
    avformat_close_input(&m_pFormatCtx);
}

void CAVDecoder::init(QIODevice* io)
{
    if (m_bIsInited)
        return;

    m_pIOContext = new CAVIOContext(io);

    av_register_all();

    //set iocontext
    m_pFormatCtx = avformat_alloc_context();
    m_pFormatCtx->pb = avio_alloc_context(m_ucDataBuffer,DATA_BUFFER_SIZE,0,m_pIOContext,&m_pIOContext->read,0,&m_pIOContext->seek);

    //if (av_open_input_stream(m_pFormatCtx,,"media",m_pInputFormat,0)<0)
    //    return;

    if (avformat_open_input(&m_pFormatCtx,"media",0,0) < 0)
        return;

    // Retrieve stream information
    if(avformat_find_stream_info(m_pFormatCtx,0)<0)
        return; // Couldn't find stream information

    // Dump information about file onto standard error
    //dump_format(pFormatCtx, 0, file.toStdString().c_str(), false);

    // Find the first video and audio stream
    m_iVideoStream=m_iAudioStream=-1;
    for(unsigned int i=0; i<m_pFormatCtx->nb_streams; i++)
    {
        if(m_pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO && m_iVideoStream == -1)
            m_iVideoStream=i;
        else if (m_pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO && m_iAudioStream == -1)
            m_iAudioStream=i;
        else if (m_iVideoStream != -1 && m_iAudioStream != -1)
            break;
    }
    if(m_iVideoStream==-1 && m_iAudioStream==-1)
        return; // Didn't find a video or audio stream

    m_bHasVideo=m_bHasAudio=true;
    if (m_iVideoStream==-1)
        m_bHasVideo = false;
    if (m_iAudioStream==-1)
        m_bHasAudio = false;

    // Get a pointer to the codec context for the video stream
    m_pCodecCtx=m_pFormatCtx->streams[m_iVideoStream]->codec;

    // Find the decoder for the video stream
    m_pCodec=avcodec_find_decoder(m_pCodecCtx->codec_id);
    if(m_pCodec==NULL)
        return; // Codec not found

    // Open codec
    if(avcodec_open2(m_pCodecCtx, m_pCodec,0)<0)
        return; // Could not open codec

    m_pSWSContext = sws_getCachedContext(NULL,m_pCodecCtx->width, m_pCodecCtx->height,
                                                       m_pCodecCtx->pix_fmt,
                                                       m_pCodecCtx->width, m_pCodecCtx->height,
                                                       PIX_FMT_RGB24, SWS_BILINEAR,
                                                       NULL, NULL, NULL);

    for (int i=0;i<FRAME_BUFFER_SIZE;i++)
        m_Frames[i] = new QImage(m_pCodecCtx->width,m_pCodecCtx->height,QImage::Format_RGB888);

    emit needMoreFrames();

    m_bIsInited = true;
}

void CAVDecoder::drawFrame(QPainter* p)
{
    if (!m_bIsInited)
        return;
    if (!m_bHasVideo)
        return;

    m_mFrameMutex.lock();
    if (m_Frames[0]->isNull() || m_iFrameCount == 0)
    {
        m_mFrameMutex.unlock();
        return;
    }
    p->drawImage(0,0,*m_Frames[0]);
    //p->drawImage(0,0,m_Frames[0]->scaled(p->device()->width(),p->device()->height(),Qt::KeepAspectRatio,Qt::FastTransformation));
    for (int i=0;i<FRAME_BUFFER_SIZE;i++)
    {
        if (i == FRAME_BUFFER_SIZE-1)
            m_Frames[i] = m_Frames[0];
        else
            m_Frames[i] = m_Frames[i+1];
    }
    m_iFrameCount--;
    m_bNextFrame = false;

    if (!m_bEndOfStream)
        emit needMoreFrames();

    m_mFrameMutex.unlock();
}

void CAVDecoder::addFrame(AVFrame *frame)
{
    m_mFrameMutex.lock();
    int x,y;
    unsigned int p;
    unsigned char r,g,b;


    for (y=0;y<m_pCodecCtx->height;y++)
    {
        for (x=0;x<frame->linesize[0];x+=3)
        {
            memcpy(&r,frame->data[0]+y*frame->linesize[0]+x,1);
            memcpy(&g,frame->data[0]+y*frame->linesize[0]+x+1,1);
            memcpy(&b,frame->data[0]+y*frame->linesize[0]+x+2,1);
            p = qRgb(r,g,b);
            m_Frames[m_iFrameCount]->setPixel(x/3,y,p);
        }
    }

    m_iFrameCount++;

    //qDebug() << "adding frame" << m_iFrameCount;

    m_mFrameMutex.unlock();
}

void CAVDecoder::nextFrame()
{
    int syncsec = m_tFrameSync.msecsTo(QTime::currentTime());
    static int framecount=0;
    if (syncsec >= 1000)
    {
        //one second has passed, sync the rate
        syncsec = syncsec - 1000;
        m_bNextFrame = true;
        emit update();
        m_tFPSTime = QTime::currentTime();
        m_tFPSTime.addMSecs(m_iNextFrameTime-syncsec);
        m_tFrameSync = QTime::currentTime();
        //qDebug() << "framesync";
        framecount=0;
    }
    else if (m_tFPSTime.msecsTo(QTime::currentTime()) >= m_iNextFrameTime)
    {
        m_bNextFrame = true;
        emit update();
        m_tFPSTime = m_tFPSTime.addMSecs(m_iNextFrameTime);
        framecount++;
        //qDebug() << "next frame" << framecount << syncsec;
    }

    if (m_bEndOfStream && m_iFrameCount == 0)
        m_bShouldPlay = false;

    if (m_bShouldPlay)
        m_pTimer->start();
    else
        m_pTimer->stop();
}

void CAVDecoder::play()
{
    if (!m_bIsInited)
        return;

    //double fps = m_pCodecCtx->time_base.num / m_pCodecCtx->time_base.den;
    int i = m_pFormatCtx->streams[m_iVideoStream]->avg_frame_rate.num / m_pFormatCtx->streams[m_iVideoStream]->avg_frame_rate.den;
    //i /= m_pCodecCtx->time_base.num / m_pCodecCtx->time_base.den;
    i=m_pCodecCtx->time_base.den/(i*m_pCodecCtx->time_base.num);

    qDebug() << "fps" << i << m_pFormatCtx->streams[m_iVideoStream]->avg_frame_rate.num << m_pFormatCtx->streams[m_iVideoStream]->avg_frame_rate.den;
    m_iNextFrameTime = i;
    m_tFPSTime = QTime::currentTime();
    m_tFPSTime.addMSecs(i);
    m_tFrameSync = QTime::currentTime();
    m_bShouldPlay = true;
    m_pTimer->start();
}

void CAVDecoder::stop()
{
    if (!m_bIsInited)
        return;
    m_bShouldPlay=false;
}

int CAVDecoder::frameBufferCount()
{
    int i;
    m_mFrameMutex.lock();
    i=m_iFrameCount;
    m_mFrameMutex.unlock();
    return i;
}


CAVWorker::CAVWorker(CAVDecoder *p) : m_pDecoder(p)
{
}

void CAVWorker::decodeFrames()
{
    AVFrame         *pFrame;
    AVFrame         *pFrameRGB;
    AVPacket        packet;
    int             frameFinished;
    int             numBytes;
    quint8         *buffer;


    pFrame=avcodec_alloc_frame();
    pFrameRGB=avcodec_alloc_frame();

    // Determine required buffer size and allocate buffer
    numBytes=avpicture_get_size(PIX_FMT_RGB24, m_pDecoder->m_pCodecCtx->width, m_pDecoder->m_pCodecCtx->height);
    buffer=new uint8_t[numBytes];

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,m_pDecoder->m_pCodecCtx->width, m_pDecoder->m_pCodecCtx->height);

    int framecount=FRAME_BUFFER_SIZE-m_pDecoder->frameBufferCount();
    bool framestodecode=false;
    while(framecount > 0 && av_read_frame(m_pDecoder->m_pFormatCtx, &packet)>=0)
    {
        if(packet.stream_index==m_pDecoder->m_iVideoStream)
        {

            avcodec_decode_video2(m_pDecoder->m_pCodecCtx,pFrame,&frameFinished,&packet);

            // Did we get a video frame?
            if(frameFinished)
            {
                // Convert the image from its native format to RGB
                sws_scale(m_pDecoder->m_pSWSContext, pFrame->data, pFrame->linesize, 0, m_pDecoder->m_pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
                m_pDecoder->addFrame(pFrameRGB);
                //m_pDecoder->addFrame(pFrame);
                framecount--;
            }
        }

        av_free_packet(&packet);
        framestodecode = true;
    }

    if (!framestodecode && framecount > 0)
    {
        qDebug() << "end of stream";
        m_pDecoder->m_bEndOfStream = true;
    }

    delete [] buffer;
    av_free(pFrameRGB);
    av_free(pFrame);
}

CAVIOContext::CAVIOContext(QIODevice *io) : m_pIO(io)
{

}

int CAVIOContext::read(void *opaque, unsigned char *buf, int buf_size)
{
    CAVIOContext* avio = static_cast<CAVIOContext*>(opaque);
    return avio->m_pIO->read((char*)buf,buf_size);
}

int64_t CAVIOContext::seek(void *opaque, int64_t offset, int whence)
{
    CAVIOContext* avio = static_cast<CAVIOContext*>(opaque);
    int i = 0;

    if (whence == SEEK_CUR)
        offset = avio->m_pIO->pos() + offset;
    else if (whence == SEEK_END)
        offset = avio->m_pIO->bytesAvailable() - offset;

    if (!avio->m_pIO->seek(offset))
        i=-1;
    return i;
}
