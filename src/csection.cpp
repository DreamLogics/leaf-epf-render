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

QMutex m_gRectMutex;

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


CSection::CSection(QString id, CDocument* doc,bool hidden,int x, int y) : QObject(doc), m_sID(id), m_pDoc(doc), m_bHidden(hidden)
{
    setParent(doc);

    int fx=x;
    while (doc->sectionByPosition(fx,y) != 0)
        fx++;

    m_iX = fx;
    m_iY = y;

    m_pViewportItem = new CViewportItem();
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

    CBaseObject* obj;
    for (int i=0;i<layer->objectCount();i++)
    {
        obj = layer->object(i);
        m_ObjectsCatalog.insert(obj->id(),obj);
    }

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
void CSection::rendered(QPainter *p)
{
    m_gRenderMutex.lock();
    //qDebug() << "rendered lock";
    p->drawImage(0,0,m_imgRendered);
    m_gRenderMutex.unlock();
}*/
/*
void CSection::updateRendered(QRectF view)
{
    if (view.size() != m_rView.size())
        layout(view.height(),view.width());

    m_rView = view;
    QPainter p;
    m_imgRendered = QImage(view.size().toSize(),QImage::Format_RGB32);
    p.begin(&m_imgRendered);
    render(&p,view);
    p.end();
}

void CSection::updateRendered()
{
    if (m_rView.isNull())
        return;
    updateRendered(m_rView);
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
/*
    m_gRenderMutex.lock();
    //qDebug() << "layout lock";
    QPainter p;
    m_imgRendered = QImage(m_rView.size().toSize(),QImage::Format_RGB32);
    p.begin(&m_imgRendered);
    render(&p,m_rView);
    p.end();
    m_gRenderMutex.unlock();
*/
    //document()->updateRenderView();
    m_gRectMutex.lock();
    m_rRect = QRectF(m_iX*width,m_iY*height,width,height);
    m_gRectMutex.unlock();
}

int CSection::x()
{
    return m_iX;
}

int CSection::y()
{
    return m_iY;
}
/*
void CSection::setViewOffset(int dx, int dy)
{
    m_rView.moveTo(dx,dy);
}*/

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
void CSection::render(QPainter *p,QRectF region)
{
    //m_mRenderMutex.lock();

    m_gRectMutex.lock();
    if (!region.intersects(m_rRect))
    {
        m_gRectMutex.unlock();
        return;
    }

    //p->setClipRect(region);
    p->translate(m_rRect.x(),m_rRect.y());
    m_gRectMutex.unlock();

    CSS::Stylesheet* css = document()->stylesheet();
    CBaseObject* obj;
    CLayer* l;
    QRectF absreg(QPointF(0,0),region.size());

    for (int i=0;i<layerCount();i++)
    {
        l = layer(i);
        for (int n=0;n<l->objectCount();n++)
        {
            obj = l->object(n);

            if (css->property(obj,"position")->toString() == "fixed" && obj->boundingRect().intersects(absreg))
            {
                p->save();
                p->translate(obj->boundingRect().x(),obj->boundingRect().y());
                obj->paintBuffered(p);
                p->restore();
            }
            else if (obj->boundingRect().intersects(region))
            {
                p->save();
                p->translate(obj->boundingRect().x() - region.x(),obj->boundingRect().y() - region.y());
                obj->paintBuffered(p);
                p->restore();
            }
        }
    }

    //m_mRenderMutex.unlock();
}

CBaseObject* CSection::objectOnPos(int x, int y)
{
    QPoint pos(x,y);

    CLayer* l;
    CBaseObject* obj;

    for (int i=0;i<layerCount();i++)
    {
        l = layer(i);
        for (int n=l->objectCount()-1;n>=0;n--)
        {
            obj = l->object(n);
            if (obj->enabled())
            {
                if (obj->boundingRect().contains(pos))
                    return obj;
            }
        }
    }

    return 0;
}

void CSection::mouseDoubleClickEvent ( int x, int y )
{
    CBaseObject* obj = objectOnPos(x,y);
    if (!obj)
        return;

    x -= obj->boundingRect().x();
    y -= obj->boundingRect().y();

    obj->mouseDoubleClickEvent(QPoint(x,y));
}

void CSection::mousePressEvent( int x, int y )
{
    CBaseObject* obj = objectOnPos(x,y);
    if (!obj)
        return;

    x -= obj->boundingRect().x();
    y -= obj->boundingRect().y();

    obj->mousePressEvent(QPoint(x,y));
}

void CSection::mouseReleaseEvent( int x, int y )
{
    CBaseObject* obj = objectOnPos(x,y);
    if (!obj)
        return;

    x -= obj->boundingRect().x();
    y -= obj->boundingRect().y();

    obj->mouseReleaseEvent(QPoint(x,y));
}

void CSection::mouseMoveEvent( int x, int y )
{
    CBaseObject* obj = objectOnPos(x,y);
    if (!obj)
        return;

    x -= obj->boundingRect().x();
    y -= obj->boundingRect().y();

    obj->mouseMoveEvent(QPoint(x,y));
}
