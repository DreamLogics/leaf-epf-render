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
#include "../css/css_painters.h"
#include <QDebug>
#include "cdocument.h"

CBaseObject* CLabelObjectFactory::create(QString id, CLayer *layer)
{
    return new CLabelObject(id,layer);
}

CLabelObject::CLabelObject(QString id, CLayer *layer) : CBaseObject(id,layer)
{

}

void CLabelObject::preload()
{

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

    QString text;
    QFont font;
    CSS::Property prop;

    if (m_sText.isNull())
    {
        text = css->property(this,"label").toString();
        if (text.startsWith("\""))
            text = text.mid(1);
        if (text.endsWith("\""))
            text = text.left(text.size()-1);
    }
    else
        text = m_sText;

    prop = css->property(this,"font-family");
    if (!prop.isNull())
        font.setFamily(prop.toString());

    prop = css->property(this,"font-size");
    if (!prop.isNull())
    {
        if (prop.toString().endsWith("px"))
            font.setPixelSize(prop.toInt());
        else
            font.setPointSize(prop.toInt());
    }

    prop = css->property(this,"font-weight");
    if (!prop.isNull())
    {
        if (prop.toString() == "bold")
            font.setBold(true);
    }

    QColor c(255,255,255);

    prop = css->property(this,"color");
    if (!prop.isNull())
        c.setNamedColor(prop.toString());

    int flags = Qt::TextWordWrap;

    prop=css->property(this,"text-align");
    if (prop.toString() == "right")
        flags |= Qt::AlignRight;
    else if (prop.toString() == "center")
        flags |= Qt::AlignHCenter;
    else
        flags |= Qt::AlignLeft;

    prop=css->property(this,"v-align");
    if (prop.toString() == "bottom")
        flags |= Qt::AlignBottom;
    else if (prop.toString() == "center")
        flags |= Qt::AlignVCenter;
    else
        flags |= Qt::AlignTop;


    painter->setFont(font);
    painter->setPen(c);

    painter->drawText(0,0,r.width(),r.height(),flags,text);


    /*painter->setFont(QFont("sans-serif",10));
    painter->setPen(QColor("red"));
    painter->drawText(r.left()+16,r.bottom()-16,id());

    painter->drawRect(r);*/
}
