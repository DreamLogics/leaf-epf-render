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

CDocumentItem::CDocumentItem()
{
}

void CDocumentItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

}

QRectF CDocumentItem::boundingRect() const
{
    return m_qrRect;
}

void CDocumentItem::setSize(int height, int width)
{
    prepareGeometryChange();
    m_qrRect.setHeight(height);
    m_qrRect.setWidth(width);
}

void CDocumentItem::setHeight(int height)
{
    prepareGeometryChange();
    m_qrRect.setHeight(height);
}

void CDocumentItem::increaseHeight(int delta)
{
    prepareGeometryChange();
    m_qrRect.setHeight(m_qrRect.height()+delta);
}


CSection::CSection(QString id, CDocument* doc,bool hidden) : QGraphicsScene(doc),/*QObject(doc), */m_sID(id), m_pDoc(doc), m_bHidden(hidden)
{
    setParent((QObject*)doc);

    connect(this,SIGNAL(changed(QList<QRectF>)),this,SLOT(updateRendered(QList<QRectF>)));

    m_pDocumentItem = new CDocumentItem();
    m_pDocumentItem->setParent((QObject*)this);

    //connect(this,SIGNAL(sceneRectChanged(QRectF)),this,SLOT(updateFixedObjects()),Qt::QueuedConnection);
}

CSection::~CSection()
{

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
    addItem(layer);
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

QImage& CSection::rendered()
{
    return m_imgRendered;
}

void CSection::updateRendered(const QList<QRectF> &region)
{
    qDebug() << "CSection::updateRendered()";
    if (m_imgRendered.isNull() || m_imgRendered.height() != height() || m_imgRendered.width() != width())
    {
        qDebug() << width() << height();
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

void CSection::layout(int height, int width)
{
    m_pDocumentItem->setSize(height,width);
    CSS::Stylesheet* css = document()->stylesheet();

    qDebug() << "CSection::layout" << width << height;

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
            qDebug() << "position" << pos;
            if ((pos == "static" || pos == "relative") && dynamic_cast<CLayer*>(obj->parentItem()))
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

    m_pDocumentItem->setHeight(docheight);
    const QRectF docbound(0,0,width,docheight);

    for (i=0;i<layerCount();i++) //now layout all objects relative to the document but not static
    {
        l = layer(i);
        for (n=0;n<l->objectCount();n++)
        {
            obj = l->object(n);
            pos = css->property(obj,"position")->toString();
            if (pos != "static" && pos != "relative" && dynamic_cast<CLayer*>(obj->parentItem()))
            {
                if (pos == "fixed")
                    relrect = QRectF(0,0,width,height);
                else
                    relrect = docbound;
                obj->layout(relrect);
            }
        }
    }
}

CDocumentItem* CSection::documentItem()
{
    return m_pDocumentItem;
}
/*
void CSection::render(QPainter *painter, const QRectF &target, const QRectF &source,
                            Qt::AspectRatioMode aspectRatioMode)
{
    // ### Switch to using the recursive rendering algorithm instead.

    qDebug() << "render in section";

    // Default source rect = scene rect
    QRectF sourceRect = source;
    if (sourceRect.isNull())
        sourceRect = sceneRect();

    // Default target rect = device rect
    QRectF targetRect = target;
    if (targetRect.isNull()) {
        if (painter->device()->devType() == QInternal::Picture)
            targetRect = sourceRect;
        else
            targetRect.setRect(0, 0, painter->device()->width(), painter->device()->height());
    }

    // Find the ideal x / y scaling ratio to fit \a source into \a target.
    qreal xratio = targetRect.width() / sourceRect.width();
    qreal yratio = targetRect.height() / sourceRect.height();

    // Scale according to the aspect ratio mode.
    switch (aspectRatioMode) {
    case Qt::KeepAspectRatio:
        xratio = yratio = qMin(xratio, yratio);
        break;
    case Qt::KeepAspectRatioByExpanding:
        xratio = yratio = qMax(xratio, yratio);
        break;
    case Qt::IgnoreAspectRatio:
        break;
    }

    // Find all items to draw, and reverse the list (we want to draw
    // in reverse order).
    QList<QGraphicsItem *> itemList = items(sourceRect, Qt::IntersectsItemBoundingRect);

    //add fixed item,

    QGraphicsItem **itemArray = new QGraphicsItem *[itemList.size()];
    int numItems = itemList.size();
    for (int i = 0; i < numItems; ++i)
        itemArray[numItems - i - 1] = itemList.at(i);
    itemList.clear();

    painter->save();

    // Transform the painter.
    painter->setClipRect(targetRect, Qt::IntersectClip);
    QTransform painterTransform;
    painterTransform *= QTransform()
                        .translate(targetRect.left(), targetRect.top())
                        .scale(xratio, yratio)
                        .translate(-sourceRect.left(), -sourceRect.top());
    painter->setWorldTransform(painterTransform, true);

    // Two unit vectors.
    QLineF v1(0, 0, 1, 0);
    QLineF v2(0, 0, 0, 1);

    // Generate the style options
    QStyleOptionGraphicsItem *styleOptionArray = new QStyleOptionGraphicsItem[numItems];
    //for (int i = 0; i < numItems; ++i)
    //    itemArray[i]->d_ptr->initStyleOption(&styleOptionArray[i], painterTransform, targetRect.toRect());

    // Render the scene.
    drawBackground(painter, sourceRect);
    drawItems(painter, numItems, itemArray, styleOptionArray);
    drawForeground(painter, sourceRect);

    delete [] itemArray;
    delete [] styleOptionArray;

    painter->restore();
}
*/

void CSection::updateFixedObjects(int dy)
{
    qDebug() << "dd" << dy;
}
