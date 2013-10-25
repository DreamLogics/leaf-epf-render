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
#include <QElapsedTimer>


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

    /*int fx=x;
    while (doc->sectionByPosition(fx,y) != 0)
        fx++;*/

    m_iX = x;
    m_iY = y;

    m_iScrollX = 0;
    m_iScrollY = 0;
    m_iScrollXMax = 0;
    m_iScrollYMax = 0;

    m_pFocusObj = 0;
    m_pControlObj = 0;

    m_iLayoutHeight = -1;
    m_iLayoutWidth = -1;

    m_pViewportItem = new CViewportItem();
}

CSection::~CSection()
{
    delete m_pViewportItem;

    /*for (int i=0;i<m_Layers.size();i++)
        delete m_Layers[i];*/
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
    return objectByID(id);
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
void CSection::layout(int height, int width, QList<CBaseObject *> updatelist)
{
    m_iLayoutWidth = width;
    m_iLayoutHeight = height;

    m_pViewportItem->setSize(height,width);
    CSS::Stylesheet* css = document()->stylesheet();

    qDebug() << "CSection::layout" << width << height;

    QElapsedTimer t;
    t.start();
    QElapsedTimer objt;

    int docheight=0;
    int docwidth=0;

    int i,n,h,w;
    CLayer* l;
    CBaseObject* obj,*lobj;
    QRectF relrect;
    QString pos;
    const QRectF docbound(0,0,docwidth,docheight);
    //QList<CBaseObject*> ignore;
    QMap<CBaseObject*,QString> objpos;
    QMap<CBaseObject*,QString>::iterator it;

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
            objt.start();
            obj = l->object(n);
            obj->updateRenderMode();
            if (obj->renderMode() == CSS::rmNone)
                continue;
            if (dynamic_cast<CLayer*>(obj->parent()))
            {
                pos = css->property(obj,"position").toString();
                //qDebug() << "position" << pos;
                if (pos == "static" || pos == "relative")
                {
                    obj->layout(relrect,updatelist);
                    relrect.setTop(obj->boundingRect().bottom());
                    lobj = obj;
                    //ignore.append(obj);
                }
                else
                    objpos.insert(obj,pos);
            }
            qDebug() << "layout time for object" << obj->id() << objt.nsecsElapsed();
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

    //needs 2 loops, we need to know the docheight first

    //now layout all objects relative to the document but not static


    for (it=objpos.begin();it != objpos.end();it++)
    {
        if (document()->shouldStopLayout())
            return;
        objt.start();
        obj = it.key();
        if (obj->renderMode() == CSS::rmNone)
            continue;
        if (dynamic_cast<CLayer*>(obj->parent()))
        {
            pos = it.value();
            if (pos != "static" && pos != "relative")
            {
                if (pos == "fixed")
                {
                    //obj->setParent(m_pViewportItem);
                    relrect = QRectF(0,0,width,height);
                    obj->layout(relrect,updatelist);
                }
                else
                {
                    relrect = docbound;
                    obj->layout(relrect,updatelist);
                    h=obj->boundingRect().bottom() + obj->marginBottom();
                    if (h>docheight)
                        docheight = h;
                }

            }
        }
        qDebug() << "layout time for object" << obj->id() << objt.nsecsElapsed();
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

    QString type = css->property(this,"section-transition").toString();

    m_mTransitionFxMutex.lock();
    if (type == "slide")
        m_iTransitionFx = SlideFx;
    else if (type == "fade")
        m_iTransitionFx = FadeFx;
    else
        m_iTransitionFx = NoneFx;
    m_mTransitionFxMutex.unlock();

    //qDebug() << "scrollmax" << docheight - height;
    setScrollYMax(docheight - height);

    qDebug() << "time elapsed: " << t.nsecsElapsed();

    document()->updateRenderView();
}

void CSection::layout(QList<CBaseObject*> updatelist)
{
    if (m_iLayoutHeight == -1 || m_iLayoutWidth == -1)
        return;

    layout(m_iLayoutHeight,m_iLayoutWidth,updatelist);
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
    QElapsedTimer t;
    t.start();

    m_mRectMutex.lock();
    if (!region.intersects(m_rRect))
    {
        m_mRectMutex.unlock();
        return;
    }

    sectionrect = m_rRect;
    //p->setClipRect(region);
    m_mRectMutex.unlock();



    CSS::Stylesheet* css = document()->stylesheet(true);
    CBaseObject* obj;
    CLayer* l;
    QRectF absreg(QPointF(0,0),region.size());
    QRectF relreg(QPointF(scrollX(),scrollY()),region.size());
    //QList<QRectF> changedregions;


    //qDebug() << id() << "section render" << relreg;

    //TODO load from css
    //p->fillRect(absreg,QColor("white"));

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

            if ((css->property(obj,"position").toString() == "fixed" && obj->boundingRect().intersects(absreg)) || obj->fixedParent())
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

    document()->releaseStylesheet();

    drawScrollbar(p);

    qDebug() << "Section render time elapsed: " << t.nsecsElapsed();

    //m_mRenderMutex.unlock();
}

CBaseObject* CSection::objectOnPos(int x, int y)
{
    QPoint pos(x,y);
    x -= scrollX();
    y -= scrollY();
    QPoint posfixed(x,y);
    CSS::Stylesheet* css = document()->stylesheet();

    CLayer* l;
    CBaseObject* obj;
    QString posi;
    bool isfixed;

    for (int i=0;i<layerCount();i++)
    {
        l = layer(i);
        for (int n=l->objectCount()-1;n>=0;n--)
        {
            obj = l->object(n);
            if (obj->enabled())
            {
                isfixed = false;
                posi = css->property(obj,"position").toString();
                if (posi == "fixed")
                    isfixed = true;
                else if (obj->fixedParent())
                    isfixed = true;

                if ((obj->boundingRect().contains(pos) && !isfixed) || (obj->boundingRect().contains(posfixed) && isfixed))
                    return obj;
            }
        }
    }

    return 0;
}

void CSection::mouseDoubleClickEvent ( int x, int y )
{   
    x += scrollX();
    y += scrollY();

    if (m_pControlObj)
    {
        m_pControlObj->mouseDoubleClickEvent(QPoint(x,y));
        return;
    }

    CBaseObject* obj = objectOnPos(x,y);
    if (!obj)
        return;

    x -= obj->boundingRect().x();
    y -= obj->boundingRect().y();

    obj->mouseDoubleClickEvent(QPoint(x,y));
}

void CSection::mousePressEvent( int x, int y )
{
    m_ClickStartPoint.setX(x);
    m_ClickStartPoint.setY(y);

    x += scrollX();
    y += scrollY();

    if (scrollYMax() > 0 && m_rVertScroller.contains(m_ClickStartPoint))
    {
        m_iScrollYStart = scrollY();
        return;
    }
    else if (scrollYMax() > 0 && m_rVertScrollerBar.contains(m_ClickStartPoint))
    {
        m_iScrollYStart = -2;
        return;
    }
    else
        m_iScrollYStart = -1;

    //m_iScrollXStart = scrollX();

    if (m_pControlObj)
    {
        m_pControlObj->mousePressEvent(QPoint(x,y));
        return;
    }

    CBaseObject* obj = objectOnPos(x,y);
    if (!obj)
        return;

    x -= obj->boundingRect().x();
    y -= obj->boundingRect().y();

    obj->mousePressEvent(QPoint(x,y));
}

void CSection::mouseReleaseEvent( int x, int y )
{
    x += scrollX();
    y += scrollY();

    if (m_iScrollYStart == -2)
    {
        if (m_ClickStartPoint.y() < m_rVertScroller.top())
            setScrollY(scrollY() - m_rRect.height()/5);
        else
            setScrollY(scrollY() + m_rRect.height()/5);
        document()->updateRenderView();
        return;
    }
    else if (m_iScrollYStart >= 0)
        return;

    m_ClickStartPoint.setX(0);
    m_ClickStartPoint.setY(0);

    m_pFocusObj = 0;

    if (m_pControlObj)
    {
        qDebug() << "mouseRelease controlobj";
        m_pControlObj->mouseReleaseEvent(QPoint(x,y));
        return;
    }


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
    if (scrollYMax() > 0 && m_iScrollYStart >= 0)
    {
        int delta = y - m_ClickStartPoint.y();//scrollYMax() * (y - m_ClickStartPoint.y()) / m_rRect.height();
        //qDebug() << m_iScrollYStart << y << m_ClickStartPoint.y() << delta;
        setScrollY(m_iScrollYStart + delta);
        document()->updateRenderView();
        return;
    }

    x += scrollX();
    y += scrollY();

    if (m_pControlObj)
    {
        m_pControlObj->mouseMoveEvent(QPoint(x,y));
        return;
    }

    CBaseObject* obj = objectOnPos(x,y);
    if (!obj)
        return;

    x -= obj->boundingRect().x();
    y -= obj->boundingRect().y();

    obj->mouseMoveEvent(QPoint(x,y));
}

CSection::TransitionFx CSection::transitionType()
{
    TransitionFx fx;
    m_mTransitionFxMutex.lock();
    fx = m_iTransitionFx;
    m_mTransitionFxMutex.unlock();

    return fx;
}

void CSection::keyEvent(int key, QString val)
{
    if (m_pControlObj)
    {
        m_pControlObj->keyEvent(key,val);
        return;
    }

    if (m_pFocusObj)
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

        m_rVertScroller = QRectF(w-10,scrolleroffsety,10,scrollerheight);
        m_rVertScrollerBar = QRectF(w-10,0,10,h);

        p->fillRect(m_rVertScrollerBar,QColor(128,128,128,128));
        p->fillRect(m_rVertScroller,QColor(40,40,40,160));
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

void CSection::clearBuffers()
{
    CLayer* l;
    CBaseObject* obj;
    for (int i=0;i<layerCount();i++)
    {
        l = layer(i);
        for (int n=0;n<l->objectCount();n++)
        {
            obj = l->object(n);
            obj->clearBuffers();
        }
    }
}

void CSection::takeControl(CBaseObject *obj)
{
    m_pControlObj = obj;
}

void CSection::releaseControl(CBaseObject *obj)
{
    if (obj == m_pControlObj)
        m_pControlObj = 0;
}

void CSection::onEPFEvent(EPFEvent *ev)
{

}
