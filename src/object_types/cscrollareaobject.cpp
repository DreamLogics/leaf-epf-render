#include "cscrollareaobject.h"
#include "../idevice.h"
#include <QTimer>
#include "../csection.h"
#include "../cdocument.h"
#include <QPainter>
#include <QDebug>
#include <QEasingCurve>
#include "../css/css_painters.h"

CBaseObject* CScrollAreaObjectFactory::create(QString id, CLayer *layer)
{
    return new CScrollAreaObject(id,layer);
}

CScrollAreaObject::CScrollAreaObject(QString id, CLayer* layer) : CBaseObject(id, layer)
{

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

void CScrollAreaObject::layout(QRectF relativeTo, QList<CBaseObject *> updatelist)
{
    CBaseObject::layout(relativeTo,updatelist);
    int h,w;
    QString pos;
    QObjectList clist = children();
    CBaseObject* cobj;

    h = w = 0;

    for (int i=0;i<clist.size();i++)
    {

        cobj = dynamic_cast<CBaseObject*>(clist[i]);
        if (cobj)
        {
            pos = document()->stylesheet()->property(cobj,"position").toString();
            //qDebug()() << "CBaseObject::layout" << "#"+section()->id()+"::"+cobj->id() << pos;

            if (!(pos == "fixed"/* || cobj->fixedParent()*/))
            {
                //qDebug() << "height for" << cobj->id() << cobj->boundingRect().height();
                if (h<cobj->boundingRect().height())
                    h = cobj->boundingRect().height();
                if (w<cobj->boundingRect().width())
                    w = cobj->boundingRect().width();
            }
        }
    }

    //qDebug() << "tha height" << h << boundingRect().height() << h-boundingRect().height();

    if (w > boundingRect().width())
        setScrollXMax(w-boundingRect().width());
    else
    {
        setScrollXMax(0);
        setScrollX(0);
    }
    if (h > boundingRect().height())
        setScrollYMax(h-boundingRect().height());
    else
    {
        setScrollYMax(0);
        setScrollY(0);
    }
}

void CScrollAreaObject::paint(QPainter *painter)
{

}

void CScrollAreaObject::paintBuffered(QPainter *p)
{
    QRectF r(boundingRect().x() - section()->scrollX(),boundingRect().y() - section()->scrollY(),boundingRect().width(),boundingRect().height());

    //paint children
    //qDebug() << "paint children";
    QString pos;
    QObjectList clist = children();
    CBaseObject* cobj;
    QRectF clipping = r;

    p->save();

    p->resetTransform();


    //qDebug() << "whaaaa" << r;

    for (int i=0;i<clist.size();i++)
    {

        cobj = dynamic_cast<CBaseObject*>(clist[i]);
        if (cobj)
        {
            pos = document()->stylesheet()->property(cobj,"position").toString();
            //qDebug() << "CScrollAreaObject::paintBuffered" << "#"+section()->id()+"::"+cobj->id() << pos;

            if (pos == "fixed")
            {
                p->save();
                p->translate(cobj->boundingRect().x(),cobj->boundingRect().y());
                cobj->paintBuffered(p);
                p->restore();
            }
            else
            {
                p->save();
                p->setClipRect(clipping);
                p->translate(cobj->boundingRect().x() - section()->scrollX() - scrollX(),cobj->boundingRect().y() - section()->scrollY() - scrollY());
                cobj->paintBuffered(p);
                p->restore();
            }
        }
    }

    p->restore();
    CSS::paintBorder(p,r,styleProperty("border-top"),styleProperty("border-bottom"),styleProperty("border-left"),styleProperty("border-right"));

    int shadeheight = 45;

    //draw shades
    if (scrollYMax() > 0)
    {
        CSS::Property bggradtop("background-gradient","(0,rgba(24,24,24,128)) (1,rgba(24,24,24,0))",0,CSS::vtMixed,CSS::smNone,false);
        CSS::Property bggradbottom("background-gradient","(0,rgba(24,24,24,0)) (1,rgba(24,24,24,128))",0,CSS::vtMixed,CSS::smNone,false);
        CSS::Property gradtype("background-gradient-type","linear",0,CSS::vtString,CSS::smNone,false);
        CSS::Property gradspread("background-gradient-spread","pad",0,CSS::vtString,CSS::smNone,false);
        CSS::Property anglegrad("background-gradient-angle","0",0,CSS::vtInt,CSS::smNone,false);
        CSS::Property gradcenter("background-gradient-center","0 0",0,CSS::vtMixed,CSS::smNone,false);
        CSS::Property gradfocal("background-gradient-focal","0 0",0,CSS::vtMixed,CSS::smNone,false);

        QRectF topshade(0,0,r.width(),shadeheight);
        QRectF bottomshade(0,0,r.width(),shadeheight);

        p->save();
        if (scrollY() > 0)
        {
            if (scrollY() < shadeheight)
                p->setOpacity((double)scrollY() / (double)shadeheight);
            CSS::paintBackgroundGradient(p,topshade,bggradtop,gradtype,gradspread,anglegrad,gradcenter,gradfocal);
        }
        if (scrollY() < scrollYMax())
        {
            if (scrollY() > scrollYMax()-shadeheight)
                p->setOpacity((double)(scrollYMax()-scrollY())/(double)shadeheight);
            else
                p->setOpacity(1.0);
            p->translate(0,r.height()-shadeheight);
            CSS::paintBackgroundGradient(p,bottomshade,bggradbottom,gradtype,gradspread,anglegrad,gradcenter,gradfocal);
        }
        p->restore();

    }

    drawScrollbar(p,r.width(),r.height());
}

void CScrollAreaObject::mousePressEvent( QPoint pos )
{
    int x = pos.x();
    int y = pos.y();
    m_ClickStartPoint.setX(x);
    m_ClickStartPoint.setY(y);

    //qDebug() << "scrollarea press" << pos << verticalScroller().toRect() << m_ClickStartPoint;

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
        if (scrollYMax() > 0 && verticalScroller().contains(m_ClickStartPoint))
        {
            m_iScrollYStart = scrollY();
            return;
        }
        else if (scrollYMax() > 0 && verticalScrollerBar().contains(m_ClickStartPoint))
        {
            m_iScrollYStart = -2;
            return;
        }
        else
            m_iScrollYStart = -1;

        m_iScrollXStart = scrollX();
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
        if (m_iScrollYStart == -2)
        {
            if (m_ClickStartPoint.y() < verticalScroller().top())
                setScrollY(scrollY() - boundingRect().height()/5);
            else
                setScrollY(scrollY() + boundingRect().height()/5);
            document()->updateRenderView();
            return;
        }
        else if (m_iScrollYStart >= 0)
            return;
    }

    x += scrollX();
    y += scrollY();



    m_ClickStartPoint.setX(0);
    m_ClickStartPoint.setY(0);

    x += boundingRect().x();
    y += boundingRect().y();

    CBaseObject* obj = section()->objectOnPos(x,y,this,this);
    if (!obj)
        return;

    x -= obj->boundingRect().x();
    y -= obj->boundingRect().y();

    obj->mouseReleaseEvent(QPoint(x,y));
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
        if (scrollYMax() > 0 && m_iScrollYStart >= 0)
        {
            int delta = y - m_ClickStartPoint.y();
            delta = delta * (scrollYMax()+boundingRect().height()) / boundingRect().height();
            setScrollY(m_iScrollYStart + delta);
            document()->updateRenderView();
            return;
        }
    }

    x += scrollX();
    y += scrollY();

    x += boundingRect().x();
    y += boundingRect().y();

    CBaseObject* obj = section()->objectOnPos(x,y,this,this);
    if (!obj)
        return;

    x -= obj->boundingRect().x();
    y -= obj->boundingRect().y();

    obj->mouseMoveEvent(QPoint(x,y));
}

void CScrollAreaObject::momentum()
{
    if (m_iMomentumPos != -1)
    {
        if (m_iMomentumPos > 100)
        {
            m_iMomentumPos = -1;
            return;
        }
        if (m_iMomentumDistance < 10 && m_iMomentumDistance > -10)
        {
            m_iMomentumPos = -1;
            return;
        }
        double pos = (double)m_iMomentumPos / 100;
        QEasingCurve ec(QEasingCurve::OutQuad);
        pos = ec.valueForProgress(pos);
        int delta = m_iMomentumStart + (m_iMomentumDistance * pos);
        //delta *= -1;

        if (delta < 0)
        {
            m_iMomentumPos = -1;
            delta = 0;
        }
        else if (delta > scrollYMax())
        {
            m_iMomentumPos = -1;
            delta = scrollYMax();
        }
        else
            m_iMomentumPos++;

        setScrollY(delta);
        document()->updateRenderView();
        //qDebug()() << m_iMomentumPos << m_iMomentumDistance << delta << m_iMomentumStart;

    }
}
