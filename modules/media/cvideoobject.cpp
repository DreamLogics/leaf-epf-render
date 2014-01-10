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
#include "cavdecoder.h"
#include "../../src/css/css_style.h"
#include "../../src/cdocument.h"
#include <QPainter>
#include <QDebug>

CBaseObject* CVideoObjectFactory::create(QString id, CLayer *layer)
{
    return new CVideoObject(id,layer);
}

CVideoObject::CVideoObject(QString id, CLayer *layer) : CBaseObject(id,layer)
{
    m_pAV = 0;
    m_pPrivate = new CVideoPrivate(this);
    m_iCurTime = 0;
}

CVideoObject::~CVideoObject()
{
    delete m_pPrivate;
    if (m_pAV)
        delete m_pAV;
}

void CVideoObject::preload()
{

}

void CVideoObject::paint(QPainter *painter)
{

}

void CVideoObject::paintBuffered(QPainter *p)
{
    if (m_pAV)
    {
        m_pAV->drawFrame(p,boundingRect().height(),boundingRect().width());
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
                if (m_pAV)
                    delete m_pAV;
                m_pAV = 0;
                return;
            }

            if (m_pAV)
                delete m_pAV;
            m_pAV = new AV::CAVDecoder();
            connect(m_pAV,SIGNAL(update()),document(),SLOT(updateRenderView()));
            connect(m_pAV,SIGNAL(updateTime(int)),m_pPrivate,SLOT(updateTime(int)));
            m_pAV->init(io);
        }

    }
}

void CVideoObject::mouseReleaseEvent(QPoint pos)
{
    if (m_pAV)
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
    }
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

    if (m_pAV->isPlaying())
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
    double pos = (double)m_iCurTime / (double)m_pAV->duration();
    p->setBrush(br);
    p->drawRoundedRect(30,boundingRect().height()-16,(double)(boundingRect().width()-40)*pos,12,6,6);
    //qDebug() << m_iCurTime << m_pAV->duration() << pos;
}

CVideoPrivate::CVideoPrivate(CVideoObject *p) : m_pObject(p)
{

}

void CVideoPrivate::updateTime(int time)
{
    m_pObject->m_iCurTime = time;
}
