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
#include <css/css_style.h>
#include <QPainter>
#include <QDebug>

CBaseObject* CVideoObjectFactory::create(QString id, CLayer *layer)
{
    return new CVideoObject(id,layer);
}

CVideoObject::CVideoObject(QString id, CLayer *layer) : CBaseObject(id,layer)
{
    m_pAV = 0;
}

void CVideoObject::preload()
{

}

void CVideoObject::paint(QPainter *painter)
{

}

void CVideoObject::paintBuffered(QPainter *p)
{
    if (m_pAV && m_pAV->isPlaying())
        m_pAV->drawFrame(p);
    else
    {
        QRectF r(0,0,boundingRect().width(),boundingRect().height());
        p->fillRect(r,QColor("black"));
    }
}

void CVideoObject::layout(QRectF relativeTo)
{
    CBaseObject::layout(relativeTo);

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
        m_pAV->init(io);
    }
}

void CVideoObject::mouseReleaseEvent(QPoint pos)
{
    if (m_pAV)
    {
        if (m_pAV->isPlaying())
            m_pAV->stop();
        else
        {
            m_pAV->play();
            qDebug() << "play vid";
        }
    }
}
