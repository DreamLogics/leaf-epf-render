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

#ifndef CTEXTOBJECT_H
#define CTEXTOBJECT_H

#include "cbaseobject.h"
#include "iepfobjectfactory.h"

class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

class CTextObjectFactory : public IEPFObjectFactory
{
    virtual CBaseObject* create(QString id, CLayer *layer);
};

class CTextObject : public CBaseObject
{
    Q_OBJECT
public:
    CTextObject(QString id, CLayer* layer);

    virtual void preload();
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void layout(QRectF relativeTo);
    QString overflow();

private:

    QString css();
    
signals:
    
public slots:

private:

    QTextDocument* m_pTextDoc;
    QString m_sOverflow;
    int m_iRenderOffset;
    
};

#endif // CTEXTOBJECT_H
