/****************************************************************************
**
** LEAF EPF Render engine
** http://leaf.dreamlogics.com/
**
** Copyright (C) 2013 DreamLogics
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published
** by the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "cvideoobject.h"
//#include "cavdecoder.h"
#include "../../src/css/css_style.h"
#include "../../src/cdocument.h"
#include <QPainter>
#include <QDebug>
#include <QtAV/QtAV.h>
#include <QTimer>

static QTimer* g_postimer = 0;

using namespace QtAV;

CBaseObject* CVideoObjectFactory::create(QString id, CLayer *layer)
{
    return new CVideoObject(id,layer);
}

CVideoObject::CVideoObject(QString id, CLayer *layer) : CBaseObject(id,layer)
{
    m_dPos = 0;
    m_pAVPlayer = 0;
    if (!g_postimer)
    {
        g_postimer = new QTimer();
        g_postimer->setInterval(40);
        g_postimer->start();
    }
    connect(g_postimer,SIGNAL(timeout()),this,SLOT(updateTime()));
}

CVideoObject::~CVideoObject()
{
    if (m_pAVPlayer)
        delete m_pAVPlayer;
}

void CVideoObject::preload()
{
    m_pAVPlayer = new AVPlayer();
    m_pAVPlayer->setRenderer(this);
}

void CVideoObject::unload()
{
    CBaseObject::unload();
    if (m_pAVPlayer)
    {
        m_pAVPlayer->stop();
        m_pAVPlayer->removeVideoRenderer(this);
    }
}

void CVideoObject::paint(QPainter *painter)
{

}

void CVideoObject::paintBuffered(QPainter *p)
{
    DPTR_D(CVideoObject);
    if (m_pAVPlayer->isLoaded())
    {
        QMutexLocker locker(&d.img_mutex);
        //m_pAV->drawFrame(p,boundingRect().height(),boundingRect().width());
        if (d.image.isNull()) {
            //TODO: when setInSize()?
            d.image = QImage(rendererSize(), QImage::Format_RGB32);
            d.image.fill(Qt::black); //maemo 4.7.0: QImage.fill(uint)
        }
        if (d.image.size() != rendererSize())
            d.image = d.image.scaled(rendererSize());
        p->drawImage(0,0,d.image);
        drawAVControls(p);
    }
    else
    {
        QRectF r(0,0,boundingRect().width(),boundingRect().height());
        p->fillRect(r,QColor("black"));
    }
}

void CVideoObject::layout(QRectF relativeTo, QList<CBaseObject*> updatelist)
{
    CBaseObject::layout(relativeTo,updatelist);

    if ((updatelist.size() > 0 && updatelist.contains(this)) || updatelist.size() == 0)
    {
        CSS::Stylesheet* css = document()->stylesheet();
        CSS::Property vidprop = css->property(this,"video");
        QString vid = vidprop.toString();
        QRegExp srcreg("src *\\([ \"]*([^\"\\) ]+)[ \"]*\\)");

        if (srcreg.indexIn(vid) != -1)
            vid = srcreg.cap(1);

        //setup video if needed
        if (m_sVideo != vid)
        {
            m_sVideo = vid;
            ResourceIO* io = document()->resourceIO(m_sVideo);

            if (!io)
            {
                m_pAVPlayer->setIODevice(0);
                return;
            }

            /*if (m_pAV)
                delete m_pAV;
            m_pAV = new AV::CAVDecoder();
            connect(m_pAV,SIGNAL(update()),document(),SLOT(updateRenderView()));
            connect(m_pAV,SIGNAL(updateTime(int)),m_pPrivate,SLOT(updateTime(int)));
            m_pAV->init(io);*/
            //connect(m_pAVPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(updateTime(qint64)));

            m_pAVPlayer->setIODevice(io);
        }

        resizeRenderer(boundingRect().size().toSize());
    }
}

void CVideoObject::updateTime()
{
    if (!m_pAVPlayer)
        return;
    if (!m_pAVPlayer->isLoaded())
        return;
    m_dPos = (double)m_pAVPlayer->position() / (double)m_pAVPlayer->duration();
    document()->updateRenderView();
}

void CVideoObject::mouseReleaseEvent(QPoint pos)
{
    if (m_pAVPlayer->isPlaying())
    {
        m_pAVPlayer->pause(!m_pAVPlayer->isPaused());
        document()->updateRenderView();
    }
    else
    {
        m_pAVPlayer->play();
    }
    /*if (m_pAV)
    {
        if (m_pAV->isPlaying())
        {
            m_pAV->pause();
            document()->updateRenderView();
        }
        else
        {
            m_pAV->play();

        }
    }*/
}

void CVideoObject::drawAVControls(QPainter *p)
{
    QRectF r(0,boundingRect().height()-20,boundingRect().width(),20);
    p->fillRect(r,QColor(0,0,0,60));

    QPen pen;
    pen.setWidth(0);
    pen.setColor(QColor(0,0,0,60));

    p->setBrush(QColor(255,255,255,200));
    p->setPen(pen);

    if (!m_pAVPlayer->isPaused())
    {
        QPolygonF pause1;
        pause1.append(QPointF(10,boundingRect().height()-18));
        pause1.append(QPointF(14,boundingRect().height()-18));
        pause1.append(QPointF(14,boundingRect().height()-2));
        pause1.append(QPointF(10,boundingRect().height()-2));

        QPolygonF pause2;
        pause2.append(QPointF(16,boundingRect().height()-18));
        pause2.append(QPointF(20,boundingRect().height()-18));
        pause2.append(QPointF(20,boundingRect().height()-2));
        pause2.append(QPointF(16,boundingRect().height()-2));

        p->drawPolygon(pause1);
        p->drawPolygon(pause2);
    }
    else
    {
        QPolygonF play;
        play.append(QPointF(10,boundingRect().height()-18));
        play.append(QPointF(20,boundingRect().height()-10));
        play.append(QPointF(10,boundingRect().height()-2));
        p->drawPolygon(play);
    }

    QBrush b(QColor(0,0,0,60));
    QBrush br(QColor(240,20,20,200));
    p->setBrush(b);
    p->setPen(pen);

    p->drawRoundedRect(30,boundingRect().height()-16,boundingRect().width()-40,12,6,6);
    p->setBrush(br);
    p->drawRoundedRect(30,boundingRect().height()-16,(double)(boundingRect().width()-40)*m_dPos,12,6,6);
    //qDebug() << m_iCurTime << m_pAV->duration() << pos;
}

bool CVideoObject::receiveFrame(const VideoFrame &frame)
{
    prepareFrame(frame);
    //call update
    document()->updateRenderView();
    return true;
}

bool CVideoObject::needUpdateBackground() const
{
    DPTR_D(const CVideoObject);
    return d.out_rect != boundingRect().toRect();
}

void CVideoObject::drawBackground()
{
    //DPTR_D(CVideoObject);
    //d.painter->fillRect(rect(), QColor(0, 0, 0));
}

void CVideoObject::drawFrame()
{
    /*DPTR_D(CVideoObject);
    if (d.image.isNull()) {
        //TODO: when setInSize()?
        d.image = QImage(rendererSize(), QImage::Format_RGB32);
        d.image.fill(Qt::black); //maemo 4.7.0: QImage.fill(uint)
    }
    QRect roi = realROI();
    //assume that the image data is already scaled to out_size(NOT renderer size!)
    if (!d.scale_in_renderer || roi.size() == d.out_rect.size()) {
        //d.preview = d.image;
        d.painter->drawImage(d.out_rect.topLeft(), d.image, roi);
    } else {
        //qDebug("size not fit. may slow. %dx%d ==> %dx%d"
        //       , d.image.size().width(), image.size().height(), d.renderer_width, d.renderer_height);
        d.painter->drawImage(d.out_rect, d.image, roi);
        //what's the difference?
        //d.painter->drawImage(QPoint(), image.scaled(d.renderer_width, d.renderer_height));
    }*/
}
