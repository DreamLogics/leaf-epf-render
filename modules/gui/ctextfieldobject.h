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

#ifndef CTEXTFIELDOBJECT_H
#define CTEXTFIELDOBJECT_H

#include "../../src/cbaseobject.h"
#include "../../src/iepfobjectfactory.h"

class CTextFieldObjectFactory : public IEPFObjectFactory
{
    virtual CBaseObject* create(QString id, CLayer *layer);
};

class QLineEdit;

class CTextFieldObject : public CBaseObject
{
    Q_OBJECT
public:
    CTextFieldObject(QString id, CLayer* layer);
    virtual ~CTextFieldObject();

    virtual void preload();

    virtual void layout(QRectF relativeTo);

    virtual void paint(QPainter *painter);
    virtual void paintBuffered(QPainter *p);

signals:

    void createTextField();
    void updateWidgetGeometry(QRect r);
    void showWidget();
    void hideWidget();
    void destroyWidget();

private:

};

#endif // CTextFieldOBJECT_H
