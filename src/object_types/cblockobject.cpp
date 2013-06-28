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
#include "css/css_painters.h"
#include <QDebug>

CBaseObject* CBlockObjectFactory::create(QString id, CLayer *layer)
{
    return new CBlockObject(id,layer);
}

CBlockObject::CBlockObject(QString id, CLayer *layer) : CBaseObject(id,layer)
{

}

void CBlockObject::preload()
{

}

void CBlockObject::paint(QPainter *painter)
{
    CSS::Stylesheet* css = document()->stylesheet();
    QRectF r = boundingRect();
    r.moveTop(0);
    r.moveLeft(0);

    if (!css->property(this,"background-color")->isNull())
    {
        CSS::paintBackgroundColor(painter,r,css->property(this,"background-color")->toString());
    }

    if (!css->property(this,"background-image")->isNull())
    {
        QString size;

        if (!css->property(this,"background-size")->isNull())
            size = css->property(this,"background-size")->toString();
        else if (!css->property(this,"background-image-size")->isNull())
            size = css->property(this,"background-image-size")->toString();

        CSS::paintBackgroundImage(painter,r,size,css->property(this,"background-image")->toString(),document());
    }

    if (!css->property(this,"color-overlay")->isNull())
    {
        QRegExp cov("(#[0-9a-fA-F]{3,6}) +([a-zA-Z]+) +([0-9\\.]+)");
        if (cov.indexIn(css->property(this,"color-overlay")->toString()) != -1)
        {
            CSS::paintColorOverlay(painter,r,cov.cap(1),CSS::renderModeFromString(cov.cap(2)),cov.cap(3).toDouble());
        }
    }

    /*painter->setFont(QFont("sans-serif",10));
    painter->setPen(QColor("red"));
    painter->drawText(r.left()+16,r.bottom()-16,id());

    painter->drawRect(r);*/
}
