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

#include "csection.h"
#include "clayer.h"
#include <QPainter>
#include "cdocument.h"
#include "cepfview.h"
#include "cbaseobject.h"
#include "css/css_style.h"
#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsItem>


CViewportItem::CViewportItem()
{
}

void CViewportItem::paint(QPainter *painter/*, const QStyleOptionGraphicsItem *option, QWidget *widget*/)
{
    //painter->fillRect(boundingRect(),QColor("red"));
}

QRectF CViewportItem::boundingRect() const
{
    return m_qrRect;
}

void CViewportItem::setSize(int height, int width)
{
    //prepareGeometryChange();
    m_qrRect.setHeight(height);
    m_qrRect.setWidth(width);
}


CSection::CSection(QString id, CDocument* doc,bool hidden) : QObject(doc), m_sID(id), m_pDoc(doc), m_bHidden(hidden)
{
    setParent(doc);

    connect(this,SIGNAL(changed(QList<QRectF>)),this,SLOT(updateRendered(QList<QRectF>)));

    m_pViewportItem = new CViewportItem();

    //connect(this,SIGNAL(sceneRectChanged(QRectF)),this,SLOT(updateFixedObjects()),Qt::QueuedConnection);
}

CSection::~CSection()
{
    delete m_pViewportItem;
}

int CSection::objectCount()
{
    return m_ObjectsCatalog.size();
}

void CSection::addLayer(CLayer* layer/*,bool active*/)
{
    m_Layers.append(layer);
    /*if (active)
        m_pActiveLayer = layer;*/
    //addItem(layer);
}

int CSection::layerCount()
{
    return m_Layers.size();
}

CLayer* CSection::layer(int index)
{
    if (index < 0 || index >= m_Layers.size())
        return 0;

    return m_Layers[index];
}
/*
void CSection::setActiveLayer(CLayer* layer)
{
    m_pActiveLayer = layer;
}

CLayer* CSection::activeLayer()
{
    return m_pActiveLayer;
}
*/
CBaseObject* CSection::objectByID(QString id)
{
    if (!m_ObjectsCatalog.contains(id))
        return 0;

    return m_ObjectsCatalog[id];
}

bool CSection::isHidden()
{
    return m_bHidden;
}

QString CSection::id()
{
    return m_sID;
}

CDocument* CSection::document()
{
    return m_pDoc;
}

QObjectList CSection::layers()
{
    QObjectList list;
    for (int i=0;i<m_Layers.size();i++)
        list.append((QObject*)m_Layers[i]);
    return list;
}
/*
void CSection::setActiveLayer(QObject* obj)
{
    CLayer* layer = dynamic_cast<CLayer*>(obj);
    if (layer)
        setActiveLayer(layer);
}

QObject* CSection::getActiveLayer()
{
    return m_pActiveLayer;
}
*/
QObject* CSection::getObjectByID(QString id)
{
    return getObjectByID(id);
}
/*
QImage& CSection::rendered()
{
    return m_imgRendered;
}*/
/*
void CSection::updateRendered(const QList<QRectF> &region)
{
    //qDebug() << "CSection::updateRendered()";
    if (m_imgRendered.isNull() || m_imgRendered.height() != height() || m_imgRendered.width() != width())
    {
        //qDebug() << width() << height();
        m_imgRendered = QImage(width(),height(),QImage::Format_RGB32);
        m_imgRendered.fill(Qt::black);

        QPainter p;
        p.begin(&m_imgRendered);
        render(&p);
        p.end();
        return;
    }

    QPainter p;
    p.begin(&m_imgRendered);
    for (int i=0;i<region.size();i++)
    {
        render(&p,region[i],region[i]);
    }
    p.end();

}
*/
void CSection::layout(int height, int width)
{
    m_pViewportItem->setSize(height,width);
    CSS::Stylesheet* css = document()->stylesheet();

    //qDebug() << "CSection::layout" << width << height;

    int docheight=0;

    int i,n,h;
    CLayer* l;
    CBaseObject* obj;
    QRectF relrect;
    QString pos;
    for (i=0;i<layerCount();i++) //first layout all static and relative objects which are relative to the document
    {
        l = layer(i);
        obj=0;
        relrect = QRectF(0,0,0,0);
        for (n=0;n<l->objectCount();n++)
        {
            obj = l->object(n);
            pos = css->property(obj,"position")->toString();
            //qDebug() << "position" << pos;
            if ((pos == "static" || pos == "relative") && dynamic_cast<CLayer*/*CLayer*/>(obj->parent()))
            {
                obj->layout(relrect);
                relrect.setTop(obj->boundingRect().bottom());
            }
        }
        if (obj)
        {
            h=obj->boundingRect().bottom() + obj->marginBottom();
            if (h>docheight)
                docheight = h;
        }
    }

    const QRectF docbound(0,0,width,docheight);

    for (i=0;i<layerCount();i++) //now layout all objects relative to the document but not static
    {
        l = layer(i);
        for (n=0;n<l->objectCount();n++)
        {
            obj = l->object(n);
            pos = css->property(obj,"position")->toString();
            if (pos != "static" && pos != "relative" && dynamic_cast<CLayer*/*CLayer*/>(obj->parent()))
            {
                if (pos == "fixed")
                {
                    //obj->setParent(m_pViewportItem);
                    relrect = QRectF(0,0,width,height);
                }
                else
                    relrect = docbound;
                obj->layout(relrect);
            }
        }
    }
}

CViewportItem* CSection::viewportItem()
{
    return m_pViewportItem;
}
/*
void CSection::scrollSection(int dx, int dy)
{
    //m_pViewportItem->setPos(dx,dy);
}
*/
void CSection::render(QPainter *p)
{
    m_mRenderMutex.lock();

    CSS::Stylesheet* css = document()->stylesheet();
    CBaseObject* obj;
    CLayer* l;

    for (int i=0;i<layerCount();i++)
    {
        l = layer(i);
        for (int n=0;n<l->objectCount();n++)
        {
            obj = l->object(n);
            p->save();
            /*if (css->property(obj,"position")->toString() == "fixed")
                p->translate();
            else*/
                p->translate(obj->boundingRect().x(),obj->boundingRect().y());
            obj->paintBuffered(p);
            p->restore();
        }
    }

    m_mRenderMutex.unlock();
}
