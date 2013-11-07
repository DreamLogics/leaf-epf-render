#include "cscrollareaobject.h"
#include "../idevice.h"
#include <QTimer>
#include "../csection.h"
#include "../cdocument.h"
#include <QPainter>
#include <QDebug>

CBaseObject* CScrollAreaObjectFactory::create(QString id, CLayer *layer)
{
    return new CScrollAreaObject(id,layer);
}

CScrollAreaObject::CScrollAreaObject(QString id, CLayer* layer) : CBaseObject(id, layer)
{
    m_iScrollX = 0;
    m_iScrollY = 0;
    m_iScrollXMax = 0;
    m_iScrollYMax = 0;

    m_iMomentumPos = -1;
    m_iMomentumDistance = 0;
    m_iMomentumStart = 0;


    if (Device::currentDevice()->deviceFlags() & IDevice::dfTouchScreen)
    {
        m_MomentumTimer = new QTimer();

        connect(m_MomentumTimer,SIGNAL(timeout()),this,SLOT(momentum()));

        m_MomentumTimer->start(10);
    }
    else
        m_MomentumTimer = 0;
}

CScrollAreaObject::~CScrollAreaObject()
{
    if (m_MomentumTimer)
        delete m_MomentumTimer;
}

void CScrollAreaObject::preload()
{

}

void CScrollAreaObject::paint(QPainter *painter)
{

}

void CScrollAreaObject::paintBuffered(QPainter *p)
{
    //paint children
    QString pos;
    QObjectList clist = children();
    CBaseObject* cobj;
    QRectF clipping(boundingRect().x() - section()->scrollX(),boundingRect().y() - section()->scrollY(),boundingRect().width(),boundingRect().height());

    p->resetTransform();

    for (int i=0;i<clist.size();i++)
    {

        cobj = dynamic_cast<CBaseObject*>(clist[i]);
        if (cobj)
        {
            pos = document()->stylesheet()->property(cobj,"position").toString();
            //qDebug() << "CBaseObject::layout" << "#"+section()->id()+"::"+cobj->id() << pos;

            if (!(pos == "fixed" || cobj->fixedParent()))
            {
                p->save();
                p->setClipRect(clipping);
                p->translate(cobj->boundingRect().x() - section()->scrollX() - scrollX(),cobj->boundingRect().y() - section()->scrollY() - scrollY());
                cobj->paintBuffered(p);
                p->restore();
            }
        }
    }
}

void CScrollAreaObject::mousePressEvent( QPoint pos )
{
    int x = pos.x();
    int y = pos.y();
    m_ClickStartPoint.setX(x);
    m_ClickStartPoint.setY(y);

    qDebug() << "scrollarea";

    x += scrollX();
    y += scrollY();

    if (Device::currentDevice()->deviceFlags() & IDevice::dfTouchScreen)
    {

        m_iMomentumPos = -1;
        m_SwipeTimer.start();
        m_iScrollYStart = scrollY();
        return;
    }
    else
    {
        /*if (scrollYMax() > 0 && m_rVertScroller.contains(m_ClickStartPoint))
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
            m_iScrollYStart = -1;*/

        //m_iScrollXStart = scrollX();
    }

    CBaseObject* obj = section()->objectOnPos(x,y,this,this);
    if (!obj)
        return;

    x -= obj->boundingRect().x();
    y -= obj->boundingRect().y();

    obj->mousePressEvent(QPoint(x,y));
}

void CScrollAreaObject::mouseReleaseEvent( QPoint pos )
{
    int x = pos.x();
    int y = pos.y();
    QPoint endpoint(pos);

    qDebug() << "scrollarea";

    if (Device::currentDevice()->deviceFlags() & IDevice::dfTouchScreen)
    {
        if (m_SwipeTimer.elapsed() > 500)
            return;
        int pdist = endpoint.y() - m_ClickStartPoint.y();
        if (pdist < 0)
            pdist *= -1;
        if (pdist > 10)
        {
            int swipetime = m_SwipeTimer.elapsed();
            //1 second momentum
            int dist = m_ClickStartPoint.y() - y;
            double speed = ((double)dist / swipetime)/**0.1*/;
            m_iMomentumDistance = speed * 1000;
            /*if (m_iMomentumDistance < 10)
                return;*/
            m_iMomentumPos = 0;
            m_iMomentumStart = scrollY();
            return;
        }

    }
    else
    {
        /*if (m_iScrollYStart == -2)
        {
            if (m_ClickStartPoint.y() < m_rVertScroller.top())
                setScrollY(scrollY() - m_rRect.height()/5);
            else
                setScrollY(scrollY() + m_rRect.height()/5);
            document()->updateRenderView();
            return;
        }
        else if (m_iScrollYStart >= 0)
            return;*/
    }

    x += scrollX();
    y += scrollY();



    m_ClickStartPoint.setX(0);
    m_ClickStartPoint.setY(0);
}

void CScrollAreaObject::mouseMoveEvent( QPoint pos )
{
    int x = pos.x();
    int y = pos.y();
    QPoint endpoint(pos);

    if (Device::currentDevice()->deviceFlags() & IDevice::dfTouchScreen)
    {
        int pdist = endpoint.y() - m_ClickStartPoint.y();
        if (pdist < 0)
            pdist *= -1;
        if (pdist > 10)
        {
            int delta = y - m_ClickStartPoint.y();
            setScrollY(m_iScrollYStart - delta);
            document()->updateRenderView();
            return;
        }
    }
    else
    {
        /*if (scrollYMax() > 0 && m_iScrollYStart >= 0)
        {
            int delta = y - m_ClickStartPoint.y();
            setScrollY(m_iScrollYStart + delta);
            document()->updateRenderView();
            return;
        }*/
    }

    x += scrollX();
    y += scrollY();
}

void CScrollAreaObject::setScrollX(int val)
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

void CScrollAreaObject::setScrollY(int val)
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

void CScrollAreaObject::setScrollXMax(int val)
{
    if (val < 0)
        return;
    m_mScrollMutex.lock();
    if (m_iScrollX > val)
        m_iScrollX = val;
    m_iScrollXMax = val;
    m_mScrollMutex.unlock();
}

void CScrollAreaObject::setScrollYMax(int val)
{
    if (val < 0)
        return;
    m_mScrollMutex.lock();
    if (m_iScrollY > val)
        m_iScrollY = val;
    m_iScrollYMax = val;
    m_mScrollMutex.unlock();
}

int CScrollAreaObject::scrollX()
{
    m_mScrollMutex.lock();
    int i = m_iScrollX;
    m_mScrollMutex.unlock();
    return i;
}

int CScrollAreaObject::scrollY()
{
    m_mScrollMutex.lock();
    int i = m_iScrollY;
    m_mScrollMutex.unlock();
    return i;
}

int CScrollAreaObject::scrollXMax()
{
    m_mScrollMutex.lock();
    int i = m_iScrollXMax;
    m_mScrollMutex.unlock();
    return i;
}

int CScrollAreaObject::scrollYMax()
{
    m_mScrollMutex.lock();
    int i = m_iScrollYMax;
    m_mScrollMutex.unlock();
    return i;
}

void CScrollAreaObject::momentum()
{

}
