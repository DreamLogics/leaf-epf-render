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

#include "cimageobject.h"
#include "../cdocument.h"
#include <QPainter>
#include <QApplication>

CBaseObject* CImageObjectFactory::create(QString id, CLayer *layer)
{
    return new CImageObject(id,layer);
}

CImageObject::CImageObject(QString id, CLayer *layer) : CBaseObject(id,layer)
{
}

void CImageObject::preload()
{
    CBaseObject::preload();
}

void CImageObject::layout(QRectF relativeTo, QList<CBaseObject *> updatelist)
{
    if (!updatelist.contains(this))
        return;

    QString src = styleProperty("image-source").toString();

    if (m_sImageSrc != src)
    {
        m_Image = QImage::fromData(document()->resource(src));

        qreal dpr = qApp->devicePixelRatio();
        m_Image.setDevicePixelRatio(dpr);

        CSS::Property sizeprop = styleProperty("image-size");
        CSS::Property height = styleProperty("height");
        CSS::Property width = styleProperty("width");

        if ((height.isNull() || width.isNull()) && !sizeprop.isNull())
        {
            QString size = sizeprop.toString();
            QRegExp percreg("([0-9]+)%");
            QRegExp sizereg("([0-9]+)px +([0-9]+)px");
            QRegExp sizeautoreg("([0-9]+)px");
            //TODO: scale the sizes?

            if (sizereg.indexIn(size) != -1)
                m_Image = m_Image.scaled(sizereg.cap(1).toInt(),sizereg.cap(2).toInt(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            else if (sizeautoreg.indexIn(size) != -1)
                m_Image = m_Image.scaledToWidth(sizeautoreg.cap(1).toInt(),Qt::SmoothTransformation);
            else if (percreg.indexIn(size) != -1)
                m_Image = m_Image.scaled(((float)(percreg.cap(1).toInt())/100)*m_Image.width(),((float)(percreg.cap(2).toInt())/100)*m_Image.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

        }
        else if (!sizeprop.isNull())
        {
            QString size = sizeprop.toString();
            if (size == "cover")
                m_Image = m_Image.scaled(boundingRect().width(),boundingRect().height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            else if (size == "contain")
                m_Image = m_Image.scaled(boundingRect().width(),boundingRect().height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        }

        if (height.isNull())
            height.setValue(m_Image.height());

        if (width.isNull())
            width.setValue(m_Image.width());

    }

    CBaseObject::layout(relativeTo,updatelist);

}

void CImageObject::paint(QPainter *painter)
{
    if (m_Image.isNull())
        return;

    painter->drawImage(0,0,m_Image);
}

void CImageObject::setImage(QString src)
{

}

QObject* CImageObject::makeJsProxy()
{
    return new JSImageObjectProxy(this);
}

JSImageObjectProxy::JSImageObjectProxy(CImageObject *obj) : JSBaseObjectProxy(obj), m_pImageObject(obj)
{

}

void JSImageObjectProxy::setImage(QString src)
{
    m_pImageObject->setImage(src);
}
