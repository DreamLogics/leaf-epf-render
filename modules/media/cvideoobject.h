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

#include "cbaseobject.h"
#include "iepfobjectfactory.h"

class CVideoObjectFactory : public IEPFObjectFactory
{
    virtual CBaseObject* create(QString id, CLayer *layer);
};

namespace AV {
    class CAVDecoder;
}
class CAVDecoder;

class CVideoObject : public CBaseObject
{
    Q_OBJECT
public:
    CVideoObject(QString id, CLayer* layer);

    virtual void preload();

    virtual void layout(QRectF relativeTo);

    virtual void paint(QPainter *painter);
    virtual void paintBuffered(QPainter *p);

    virtual void mouseReleaseEvent(QPoint pos);

private:
    AV::CAVDecoder* m_pAV;
    QString m_sVideo;
};

#endif // CVideoOBJECT_H
