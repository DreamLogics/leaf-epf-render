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

#ifndef CBLOCKOBJECT_H
#define CBLOCKOBJECT_H

#include "cbaseobject.h"
#include "iepfobjectfactory.h"

class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

class CBlockObjectFactory : public IEPFObjectFactory
{
    virtual CBaseObject* create(QString id, CLayer *layer);
};

class CBlockObject : public CBaseObject
{
    Q_OBJECT
public:
    CBlockObject(QString id, CLayer* layer);
    
    virtual void preload();
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
    
public slots:
    
};

#endif // CBLOCKOBJECT_H
