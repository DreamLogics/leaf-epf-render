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

#include "clabelobject.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include "css/css_painters.h"
#include <QDebug>

CBaseObject* CLabelObjectFactory::create(QString id, CLayer *layer)
{
    return new CLabelObject(id,layer);
}

CLabelObject::CLabelObject(QString id, CLayer *layer) : CBaseObject(id,layer)
{

}

void CLabelObject::preload()
{
    m_sText = property("text");
}

void CLabelObject::paint(QPainter *painter)
{
    CSS::Stylesheet* css = document()->stylesheet();
    QRectF r = boundingRect();
    r.moveTop(0);
    r.moveLeft(0);

    CSS::paintBackgroundColor(painter,this);
    CSS::paintBackgroundImage(painter,this);
    CSS::paintColorOverlay(painter,this);

    /*painter->setFont(QFont("sans-serif",10));
    painter->setPen(QColor("red"));
    painter->drawText(r.left()+16,r.bottom()-16,id());

    painter->drawRect(r);*/
}
