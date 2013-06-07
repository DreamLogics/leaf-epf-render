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

void CBlockObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    CSS::Stylesheet* css = document()->stylesheet();

    if (!css->property(this,"background-color")->isNull())
    {
        CSS::paintBackgroundColor(painter,boundingRect(),css->property(this,"background-color")->toString());
    }

    /*if (!css->property(this,"background-image")->isNull())
    {
        QRectF qrSize;
        bool bAspRat=false;

        if (!css->property(this,"background-image-size")->isNull())
        {
            QRegExp percreg("([0-9]+)%");
            QRegExp sizereg("([0-9]+)px +([0-9]+)px");
            QRegExp sizeautoreg("([0-9]+)px");
            QString propstr = css->property(this,"background-image-size")->toString();
            if (propstr == "cover")
                qrSize = boundingRect();
            else if (propstr == "contain")
            {
                bAspRat = true;
                qrSize = boundingRect();
            }
            else if (sizereg.indexIn(propstr) != -1)
            {
                qrSize.setWidth(sizereg.cap(1).toInt());
                qrSize.setHeight(sizereg.cap(2).toInt());
            }
            else if (sizeautoreg.indexIn(propstr) != -1)
            {
                qrSize.setWidth(sizeautoreg.cap(1).toInt());
                bAspRat = true;
            }
            else if (sizeautoreg.indexIn(propstr) != -1)
            {
                qrSize.setWidth(sizeautoreg.cap(1).toInt());
                bAspRat = true;
            }
        }

        CSS::paintBackgroundImage(painter,boundingRect(),qrSize,css->property(this,"background-color")->toString(),document());
    }*/
}
