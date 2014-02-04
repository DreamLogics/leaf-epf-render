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

#ifndef CVideoOBJECT_H
#define CVideoOBJECT_H

#include "../../src/cbaseobject.h"
#include "../../src/iepfobjectfactory.h"
#include <QtAV/QPainterRenderer.h>
#include <QtAV/private/QPainterRenderer_p.h>

class CVideoObjectFactory : public IEPFObjectFactory
{
    virtual CBaseObject* create(QString id, CLayer *layer);
};

namespace AV {
    class CAVDecoder;
}
class CAVDecoder;
class CVideoObject;
class QResizeEvent;

namespace QtAV
{
    class AVPlayer;
}

class CVideoObjectPrivate : public QtAV::QPainterRendererPrivate
{
public:
    virtual ~CVideoObjectPrivate(){}
};

class CVideoObject : public CBaseObject, public QtAV::QPainterRenderer
{
    Q_OBJECT
public:
    CVideoObject(QString id, CLayer* layer);
    virtual ~CVideoObject();

    virtual void preload();

    virtual void layout(QRectF relativeTo, QList<CBaseObject*> updatelist);

    virtual void paint(QPainter *painter);
    virtual void paintBuffered(QPainter *p);

    virtual void mouseReleaseEvent(QPoint pos);

protected:
    virtual bool receiveFrame(const QtAV::VideoFrame& frame);
    virtual bool needUpdateBackground() const;
    //called in paintEvent before drawFrame() when required
    virtual void drawBackground();
    //draw the current frame using the current paint engine. called by paintEvent()
    virtual void drawFrame();

private:

    void drawAVControls(QPainter* p);

private slots:

    void updateTime();

private:
    //AV::CAVDecoder* m_pAV;
    QtAV::AVPlayer* m_pAVPlayer;
    QString m_sVideo;
    double m_dPos;
    friend class CVideoPrivate;
};

#endif // CVideoOBJECT_H
