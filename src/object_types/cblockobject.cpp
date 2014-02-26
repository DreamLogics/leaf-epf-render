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

#include "cblockobject.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include "../css/css_painters.h"

CBaseObject* CBlockObjectFactory::create(QString id, CLayer *layer)
{
    return new CBlockObject(id,layer);
}

CBlockObject::CBlockObject(QString id, CLayer *layer) : CBaseObject(id,layer)
{

}

void CBlockObject::preload()
{
    CBaseObject::preload();
}

void CBlockObject::paint(QPainter *painter)
{
    CSS::paintBackgroundColor(painter,this);
    CSS::paintBackgroundGradient(painter,this);
    CSS::paintBackgroundImage(painter,this);
    CSS::paintBorder(painter,this);
    CSS::paintColorOverlay(painter,this);

    /*painter->setFont(QFont("sans-serif",10));
    painter->setPen(QColor("red"));
    painter->drawText(r.left()+16,r.bottom()-16,id());

    painter->drawRect(r);*/
}
