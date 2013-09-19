#ifndef CAVDECODER_H
#define CAVDECODER_H

#include <QObject>
#include <QIODevice>
#include <QTimer>
#include <QImage>
#include <QMutex>
#include <QThread>
#include <QTime>

#define FRAME_BUFFER_SIZE 30
#define DATA_BUFFER_SIZE 32768

class QPainter;

struct AVFormatContext;
struct AVCodecContext;
struct AVCodec;
struct SwsContext;
struct AVFrame;
//struct AVIOContext;
struct AVInputFormat;

namespace AV
{

class CAVDecoder;

class CAVWorker : public QObject
{
    Q_OBJECT
public:
    CAVWorker(CAVDecoder*);

public slots:
    void decodeFrames();

private:
    CAVDecoder* m_pDecoder;
};

class CAVIOContext
{
public:
    CAVIOContext(QIODevice* io);
    static int read(void *opaque, unsigned char *buf, int buf_size);
    static int64_t seek(void *opaque, int64_t offset, int whence);

    QIODevice* m_pIO;
};

class CAVDecoder : public QObject
{
    Q_OBJECT
public:
    explicit CAVDecoder(QObject *parent = 0);
    ~CAVDecoder();

    void init(QIODevice* io);
    void drawFrame(QPainter* p);
    void release();

    void play();
    void stop();

    bool isPlaying();

private:

    void addFrame(AVFrame* frame);
    int frameBufferCount();

signals:

    void needMoreFrames();
    void update();
    
public slots:

    void nextFrame();
    
private:
    bool m_bIsInited;
    bool m_bShouldPlay;
    CAVIOContext* m_pIOContext;
    QImage* m_Frames[FRAME_BUFFER_SIZE];
    QMutex m_mFrameMutex;
    CAVWorker* m_pWorker;
    QThread* m_pWorkerThread;
    int m_iFrameCount;
    QTimer* m_pTimer;
    bool m_bNextFrame;

    bool m_bHasVideo;
    bool m_bHasAudio;

    QTime m_tFrameSync;
    QTime m_tFPSTime;

    int m_iFrameRateSync;
    int m_iNextFrameTime;

    bool m_bEndOfStream;

    AVFormatContext *m_pFormatCtx;
    int              m_iVideoStream;
    int              m_iAudioStream;
    AVCodecContext  *m_pCodecCtx;
    AVCodec         *m_pCodec;
    //AVIOContext     *m_pAVIOContext;
    AVInputFormat   *m_pInputFormat;

    SwsContext* m_pSWSContext;

    unsigned char* m_ucDataBuffer;

    friend class CAVWorker;
};

}

#endif // CAVDECODER_H
