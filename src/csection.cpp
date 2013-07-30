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
#include <QTime>


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


CSection::CSection(QString id, CDocument* doc,bool hidden,int x, int y) : QObject(doc), m_sID(id), m_pDoc(doc), m_bHidden(hidden), m_pFocusObj(0)
{
    setParent(doc);

    int fx=x;
    while (doc->sectionByPosition(fx,y) != 0)
        fx++;

    m_iX = fx;
    m_iY = y;

    m_iScrollX = 0;
    m_iScrollY = 0;
    m_iScrollXMax = 0;
    m_iScrollYMax = 0;

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

    qDebug() << "CSection::layout" << width << height;

    QTime t = QTime::currentTime();

    int docheight=0;
    int docwidth=0;

    int i,n,h,w;
    CLayer* l;
    CBaseObject* obj,*lobj;
    QRectF relrect;
    QString pos;
    for (i=0;i<layerCount();i++) //first layout all static and relative objects which are relative to the document
    {
        l = layer(i);
        obj=0;
        lobj=0;
        relrect = QRectF(0,0,0,0);
        for (n=0;n<l->objectCount();n++)
        {
            if (document()->shouldStopLayout())
                return;
            obj = l->object(n);
            pos = css->property(obj,"position")->toString();
            //qDebug() << "position" << pos;
            if ((pos == "static" || pos == "relative") && dynamic_cast<CLayer*/*CLayer*/>(obj->parent()))
            {
                obj->layout(relrect);
                relrect.setTop(obj->boundingRect().bottom());
                lobj = obj;
            }
        }
        if (lobj)
        {
            h=obj->boundingRect().bottom() + obj->marginBottom();
            w=obj->boundingRect().right() + obj->marginRight();
            if (h>docheight)
                docheight = h;
            if (w>docwidth)
                docwidth=w;
        }
    }

    const QRectF docbound(0,0,docwidth,docheight);

    for (i=0;i<layerCount();i++) //now layout all objects relative to the document but not static
    {
        l = layer(i);
        for (n=0;n<l->objectCount();n++)
        {
            if (document()->shouldStopLayout())
                return;
            obj = l->object(n);
            pos = css->property(obj,"position")->toString();
            if (pos != "static" && pos != "relative" && dynamic_cast<CLayer*/*CLayer*/>(obj->parent()))
            {
                if (pos == "fixed")
                {
                    //obj->setParent(m_pViewportItem);
                    relrect = QRectF(0,0,width,height);
                    obj->layout(relrect);
                }
                else
                {
                    relrect = docbound;
                    obj->layout(relrect);
                    h=obj->boundingRect().bottom() + obj->marginBottom();
                    if (h>docheight)
                        docheight = h;
                }

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
    m_mRectMutex.lock();
    m_rRect = QRectF(m_iX*width,m_iY*height,width,height);
    m_mRectMutex.unlock();

    //qDebug() << "scrollmax" << docheight - height;
    setScrollYMax(docheight - height);

    qDebug() << "time elapsed: " << t.msecsTo(QTime::currentTime());

    document()->updateRenderView();
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
    QRectF sectionrect;
    QTime t = QTime::currentTime();

    m_mRectMutex.lock();
    if (!region.intersects(m_rRect))
    {
        m_mRectMutex.unlock();
        return;
    }

    sectionrect = m_rRect;
    //p->setClipRect(region);
    m_mRectMutex.unlock();



    CSS::Stylesheet* css = document()->stylesheet();
    CBaseObject* obj;
    CLayer* l;
    QRectF absreg(QPointF(0,0),region.size());
    QRectF relreg(QPointF(scrollX(),scrollY()),region.size());
    //QList<QRectF> changedregions;


    //qDebug() << id() << "section render" << relreg;

    //TODO load from css
    p->fillRect(absreg,QColor("white"));

    for (int i=0;i<layerCount();i++)
    {
        l = layer(i);
        for (int n=0;n<l->objectCount();n++)
        {
            obj = l->object(n);
            /*QRegion clipregion;

            if (!obj->changed())
            {
                bool bSkip=true;
                //check all changed regions to see if a redraw of the area is needed
                for (int r=0;r<changedregions.size();r++)
                {
                    if (changedregions[r].intersects(obj->boundingRect()))
                    {
                        clipregion+=changedregions[r].toRect();
                        bSkip = false;
                    }
                }
                if (bSkip)
                    continue;
            }
            else
            {
                clipregion += obj->boundingRect().toRect();
                changedregions.append(obj->boundingRect());
            }

            p->setClipRegion(clipregion);*/

            if ((css->property(obj,"position")->toString() == "fixed" && obj->boundingRect().intersects(absreg)) || obj->fixedParent())
            {
                p->save();
                p->translate(obj->boundingRect().x(),obj->boundingRect().y());
                obj->paintBuffered(p);
                p->restore();
            }
            else if (obj->boundingRect().intersects(relreg))
            {
                p->save();
                p->translate(obj->boundingRect().x() - relreg.x(),obj->boundingRect().y() - relreg.y());
                obj->paintBuffered(p);
                p->restore();
            }
            else
            {
                //qDebug() << "dont render" << obj->id();
            }
        }
    }

    drawScrollbar(p);

    qDebug() << "Section render time elapsed: " << t.msecsTo(QTime::currentTime());

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
    m_pFocusObj = 0;
    CBaseObject* obj = objectOnPos(x,y);
    if (!obj)
        return;

    m_pFocusObj = obj;

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

CSection::TransitionFx CSection::transitionType()
{
    CSS::Stylesheet* css = document()->stylesheet();
    QString type = css->property(this,"section-transition")->toString();

    if (type == "slide")
        return SlideFx;
    else if (type == "fade")
        return FadeFx;

    return NoneFx;
}

void CSection::keyEvent(int key, QString val)
{
    if (!m_pFocusObj)
        m_pFocusObj->keyEvent(key,val);
}

void CSection::setFocus(CBaseObject *obj)
{
    m_pFocusObj = obj;
}

CBaseObject* CSection::focus()
{
    return m_pFocusObj;
}

void CSection::drawScrollbar(QPainter *p)
{
    int h = p->device()->height();
    int w = p->device()->width();

    int scrollx = scrollX();
    int scrolly = scrollY();
    int scrollxmax = scrollXMax();
    int scrollymax = scrollYMax();

    QPen pen(QColor(128,128,128));
    p->setPen(pen);

    if (scrollxmax > 0 && scrollymax == 0) //horizontal scrollbar
    {
        int scrollerwidth = w - scrollxmax;

        if (scrollerwidth < 100)
            scrollerwidth = 100;

        int scrollareax = w - scrollerwidth;
        int scrolleroffsetx = scrollx * scrollareax / scrollxmax;

        p->fillRect(0,h-10,w,10,QColor(128,128,128,128));
    }
    else if (scrollymax > 0 && scrollxmax == 0) //vertical scrollbar
    {
        int scrollerheight = h - scrollymax;

        if (scrollerheight < 100)
            scrollerheight = 100;

        int scrollareay = h - scrollerheight;
        int scrolleroffsety = scrolly * scrollareay / scrollymax;

        p->fillRect(w-10,0,10,h,QColor(128,128,128,128));

        p->fillRect(w-10,scrolleroffsety,10,scrollerheight,QColor(40,40,40,160));
        p->drawRect(w-10,scrolleroffsety,w-1,scrollerheight-1);
    }
    else if (scrollxmax > 0) //y > 0 already implied / both scrollbars
    {
        int scrollerheight = h - scrollymax;
        int scrollerwidth = w - scrollxmax;

        if (scrollerheight < 100)
            scrollerheight = 100;
        if (scrollerwidth < 100)
            scrollerwidth = 100;

        int scrollareay = h - scrollerheight;
        int scrollareax = w - scrollerwidth;
        int scrolleroffsety = scrolly * scrollareay / scrollymax;
        int scrolleroffsetx = scrollx * scrollareax / scrollxmax;

        p->fillRect(0,h-10,w,10,QColor(128,128,128,128));
        p->fillRect(w-10,0,10,h,QColor(128,128,128,128));
    }

}

void CSection::setScrollX(int val)
{
    m_mScrollMutex.lock();
    if (val >= 0 && val <= m_iScrollXMax)
        m_iScrollX = val;
    else if (val < 0)
        m_iScrollX = 0;
    else if (val > m_iScrollXMax)
        m_iScrollX = m_iScrollXMax;
    m_mScrollMutex.unlock();
}

void CSection::setScrollY(int val)
{
    m_mScrollMutex.lock();
    if (val >= 0 && val <= m_iScrollYMax)
        m_iScrollY = val;
    else if (val < 0)
        m_iScrollY = 0;
    else if (val > m_iScrollYMax)
        m_iScrollY = m_iScrollYMax;
    m_mScrollMutex.unlock();
}

void CSection::setScrollXMax(int val)
{
    if (val < 0)
        return;
    m_mScrollMutex.lock();
    if (m_iScrollX > val)
        m_iScrollX = val;
    m_iScrollXMax = val;
    m_mScrollMutex.unlock();
}

void CSection::setScrollYMax(int val)
{
    if (val < 0)
        return;
    m_mScrollMutex.lock();
    if (m_iScrollY > val)
        m_iScrollY = val;
    m_iScrollYMax = val;
    m_mScrollMutex.unlock();
}

int CSection::scrollX()
{
    m_mScrollMutex.lock();
    int i = m_iScrollX;
    m_mScrollMutex.unlock();
    return i;
}

int CSection::scrollY()
{
    m_mScrollMutex.lock();
    int i = m_iScrollY;
    m_mScrollMutex.unlock();
    return i;
}

int CSection::scrollXMax()
{
    m_mScrollMutex.lock();
    int i = m_iScrollXMax;
    m_mScrollMutex.unlock();
    return i;
}

int CSection::scrollYMax()
{
    m_mScrollMutex.lock();
    int i = m_iScrollYMax;
    m_mScrollMutex.unlock();
    return i;
}
