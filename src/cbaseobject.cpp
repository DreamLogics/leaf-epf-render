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

#include "cbaseobject.h"
//#include "css/css_style.h"
#include "clayer.h"
#include "csection.h"
#include "cdocument.h"
#include "cepfview.h"
#include <QDebug>
#include <QGLFramebufferObject>
#include <QTime>
#include "canimator.h"
#include "css/css_transition.h"

CBaseObject::CBaseObject(QString id, CLayer* layer) : QObject(),
    m_sID(id), m_pLayer(layer)
{
    m_iMarginTop = 0;
    m_iMarginRight = 0;
    m_iMarginBottom = 0;
    m_iMarginLeft = 0;
    m_bEnabled = false;
    m_iRotation = 0;
    m_bFixedParent = false;
    m_bNeedsRedraw = true;
    //m_pBuffer = 0;
    m_bChanged = false;
    m_iAnimation = -1;
    m_iTransitionTime = 0;

    //setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

CBaseObject::~CBaseObject()
{
    //if (m_pBuffer)
    //    delete m_pBuffer;

    //unregister from animator
    if (m_iAnimation != -1)
        CAnimator::get(this->thread())->unregisterAnimation(m_iAnimation,this);
}

bool CBaseObject::enabled() const
{
    return m_bEnabled;
}

void CBaseObject::setEnabled(bool b)
{
    m_bEnabled = b;
}

CLayer* CBaseObject::layer()
{
    return m_pLayer;
}

void CBaseObject::setBoundingRect(const QRectF &r)
{
    if (r != m_rRect)
    {
        //prepareGeometryChange();

        m_rRect = r;

        if (m_rRect.size() != r.size())
        {
            //m_bNeedsRedraw = true;
            buffer();
        }
    }
}

QRectF CBaseObject::boundingRect() const
{
    /*QRectF r = m_rRect;
    r.translate(m_iDX,m_iDY);
    return r;*/
    return m_rRect;
}

void CBaseObject::preload()
{
    qDebug() << "preload func";
}

QString CBaseObject::id() const
{
    return m_sID;
}

const char* CBaseObject::objectType() const
{
    return "base";
}

void CBaseObject::layout(QRectF relrect)
{
    /*QGraphicsItem* r = parentItem();

    if (!r)
        return;

    CBaseObject* obj = dynamic_cast<CBaseObject*>(r);
    if (obj)
        qDebug() << "child of:" << obj->id();*/




    QRectF oldrect = m_rRect;
    QRectF newrect = m_rRect;
    //QRectF relrect = r->boundingRect();

    CSS::Stylesheet* css = document()->stylesheet();

    m_RenderPropsMutex.lock();

    m_dOpacity = css->property(this,"opacity").toDouble();

    m_RenderPropsMutex.unlock();

    setMargin(css->property(this,"margin-top").toInt(),css->property(this,"margin-left").toInt(),css->property(this,"margin-bottom").toInt(),css->property(this,"margin-right").toInt());
    //setPadding(css->property(this,"padding-top").toInt(),css->property(this,"padding-left").toInt(),css->property(this,"padding-bottom").toInt(),css->property(this,"padding-right").toInt());

    newrect.setHeight(css->property(this,"height").toInt()+css->property(this,"mod-height").toInt());
    newrect.setWidth(css->property(this,"width").toInt()+css->property(this,"mod-width").toInt());

    //qDebug() << "CBaseObject::layout" << m_rRect.size() << "#"+section()->id()+"::"+id() << css->property(this,"height").toString();

    /*if (css->property(this,"relative-to").toString() == "")
    {
        m_rRect.setTop(relrect.top() + r->outerHeight() + marginTop());
        m_rRect.setLeft(css->property(this,"left").toInt() + marginLeft());

        //top/left/bottom/right negeren
    }
    else*/
    QString pos = css->property(this,"position").toString();


    if (pos != "static" && pos != "relative")
    {
        newrect.moveTop(relrect.top() + css->property(this,"top").toInt());
        newrect.moveLeft(relrect.left() + css->property(this,"left").toInt());

        //qDebug() << "CBaseObject::layout" << newrect << "#"+section()->id()+"::"+id() << css->property(this,"top").toInt();

        //m_rRect.setHeight(css->property(this,"height").toInt());
        //m_rRect.setWidth(css->property(this,"width").toInt());

        if (!css->property(this,"bottom").isNull() && !css->property(this,"top").isNull())
        {
            newrect.setBottom(relrect.bottom() - css->property(this,"bottom").toInt());
            newrect.setTop(newrect.bottom() - css->property(this,"height").toInt());
        }
        else if (!css->property(this,"bottom").isNull())
            newrect.moveBottom(relrect.bottom() - css->property(this,"bottom").toInt());

        if (!css->property(this,"right").isNull() && !css->property(this,"left").isNull())
        {
            newrect.setRight(relrect.right() - css->property(this,"right").toInt());
            newrect.setLeft(newrect.left() - css->property(this,"width").toInt());
        }
        else if (!css->property(this,"right").isNull())
            newrect.moveRight(relrect.right() - css->property(this,"right").toInt());
    }
    else
    {
        newrect.moveTop(relrect.top()+marginTop());
        newrect.moveLeft(marginLeft());
    }

    //mod position
    CSS::Property modtop = css->property(this,"mod-top");
    CSS::Property modbot = css->property(this,"mod-bottom");
    CSS::Property modleft = css->property(this,"mod-left");
    CSS::Property modright = css->property(this,"mod-right");

    if (!modtop.isNull() && !modbot.isNull())
    {
        newrect.setTop(newrect.top()+modtop.toInt());
        newrect.setBottom(newrect.bottom()+modbot.toInt());
    }
    else if (!modbot.isNull())
        newrect.moveBottom(newrect.bottom()+modbot.toInt());
    else if (!modtop.isNull())
        newrect.moveTop(newrect.top()+modtop.toInt());



    if (!modleft.isNull() && !modright.isNull())
    {
        newrect.setLeft(newrect.left()+modleft.toInt());
        newrect.setRight(newrect.right()+modright.toInt());
    }
    else if (!modright.isNull())
        newrect.moveRight(newrect.right()+modright.toInt());
    else if (!modleft.isNull())
        newrect.moveLeft(newrect.left()+modleft.toInt());



    //min/max height/width
    if (!css->property(this,"min-height").isNull() && newrect.height() < css->property(this,"min-height").toInt())
        newrect.setHeight(css->property(this,"min-height").toInt());

    if (!css->property(this,"min-width").isNull() && newrect.width() < css->property(this,"min-width").toInt())
        newrect.setHeight(css->property(this,"min-width").toInt());

    if (!css->property(this,"max-height").isNull() && newrect.height() > css->property(this,"max-height").toInt())
        newrect.setHeight(css->property(this,"max-height").toInt());

    if (!css->property(this,"max-width").isNull() && newrect.width() > css->property(this,"max-width").toInt())
        newrect.setHeight(css->property(this,"max-width").toInt());


    if (oldrect != newrect)
    {
        //prepareGeometryChange();
        m_rRect = newrect;

        m_mChangedMutex.lock();
        m_bChanged = true;
        m_mChangedMutex.unlock();

        if (oldrect.size() != newrect.size())
        {
            //m_bNeedsRedraw = true;
            buffer();
        }
        else if (m_bNeedsRedraw)
            buffer();
    }
    else if (m_bNeedsRedraw)
        buffer();

    m_FPMutex.lock();

    CBaseObject* obj = dynamic_cast<CBaseObject*>(parent());
    if (obj)
    {
        if (css->property(obj,"position").toString() == "fixed" || obj->fixedParent())
            m_bFixedParent = true;
    }

    m_FPMutex.unlock();

    //animation
    CSS::Property anim = css->property(this,"animation-name");

    if (!anim.isNull() && anim.toString() != "")
    {
        CSS::Property anim_duration = css->property(this,"animation-duration");
        CSS::Property anim_timing_function = css->property(this,"animation-timing-function");
        CSS::Property anim_delay = css->property(this,"animation-delay");
        CSS::Property anim_iteration_count = css->property(this,"animation-iteration-count");
        CSS::Property anim_direction = css->property(this,"animation-direction");

        int iter = 1;

        if (anim_iteration_count.toString() == "infinite")
            iter = -1;
        else
            iter = anim_iteration_count.toInt();

        CSS::easing_function ef;
        if (anim_timing_function.toString() == "ease-in")
            ef = CSS::efEaseIn;
        else if (anim_timing_function.toString() == "ease-out")
            ef = CSS::efEaseOut;
        else if (anim_timing_function.toString() == "none" || anim_timing_function.toString() == "normal" || anim_timing_function.toString() == "linear")
            ef = CSS::efNone;
        else
            ef = CSS::efEase;

        CSS::direction dir = CSS::dirNormal;
        if (anim_direction.toString() == "reverse")
            dir = CSS::dirReverse;
        else if (anim_direction.toString() == "alternate")
            dir = CSS::dirAlternate;
        else if (anim_direction.toString() == "alternate-reverse")
            dir = CSS::dirAlternateReverse;

        m_iAnimation = CAnimator::get(this->thread())->registerAnimation(this,anim.toString(),CSS::stringToMsTime(anim_duration.toString()),ef,CSS::stringToMsTime(anim_delay.toString()),iter,dir,m_iAnimation);
    }
    else if (m_iAnimation != -1)
        CAnimator::get(this->thread())->unregisterAnimation(m_iAnimation,this);

    //transitions
    CSS::Property transition = css->property(this,"transition-duration");
    if (!transition.isNull())
    {
        m_iTransitionTime = CSS::stringToMsTime(transition.value());
        CSS::Property transition_timing_function = css->property(this,"transition-timing-function");
        CSS::Property transition_delay = css->property(this,"transition-delay");
        CSS::Property transition_properties = css->property(this,"transition-property");

        if (transition_delay.isNull())
            m_iTransitionDelay = 0;
        else
            m_iTransitionDelay = CSS::stringToMsTime(transition_delay.value());

        if (transition_timing_function.toString() == "ease-in")
            m_TransitionEasing = CSS::efEaseIn;
        else if (transition_timing_function.toString() == "ease-out")
            m_TransitionEasing = CSS::efEaseOut;
        else if (transition_timing_function.toString() == "none" || transition_timing_function.toString() == "normal" || transition_timing_function.toString() == "linear")
            m_TransitionEasing = CSS::efNone;
        else
            m_TransitionEasing = CSS::efEase;

        m_TransitionProps.clear();
        m_TransitionProps = transition_properties.value().split(",");
    }
    else
        m_iTransitionTime = 0;

    //qDebug() << "CBaseObject::layout parent" << "#"+section()->id()+"::"+id() << pos;

    QObjectList clist = children();
    CBaseObject* cobj;
    QRectF relr = m_rRect;
    for (int i=0;i<clist.size();i++)
    {
        if (document()->shouldStopLayout())
            return;
        cobj = dynamic_cast<CBaseObject*>(clist[i]);
        if (cobj)
        {
            pos = css->property(cobj,"position").toString();
            //qDebug() << "CBaseObject::layout" << "#"+section()->id()+"::"+cobj->id() << pos;

            if (pos == "absolute")
                cobj->layout(m_rRect);
            else
            {
                cobj->layout(relr);
                relr.setTop(cobj->boundingRect().bottom());
            }
        }
    }


}
/*
CBaseObject* CBaseObject::relative()
{
    return (CBaseObject*)parent();
}
QString CBaseObject::relativeID()
{
    return relative()->id();
}
*/
QString CBaseObject::property(QString key)
{
    if (!m_Props.contains(key))
        return QString();
    return m_Props[key];
}
void CBaseObject::setProperty(QString key, QString value)
{
    if (m_Props.contains(key))
    {
        m_Props[key] = value;
        return;
    }

    m_Props.insert(key,value);
}

void CBaseObject::mouseDoubleClickEvent ( QPoint contentpos )
{

}
void CBaseObject::mousePressEvent(QPoint contentpos)
{

}
void CBaseObject::mouseReleaseEvent(QPoint contentpos)
{
    sendEvent("clicked");
    emit clicked();
}
void CBaseObject::mouseMoveEvent(QPoint contentpos)
{


}

void CBaseObject::setCSSOverride(QString ss)
{
    //m_bNeedsRedraw = true;

    m_CSSOverrideProps.clear();

    CSS::Stylesheet* css = document()->stylesheet();
    QStringList props = ss.split(";",QString::SkipEmptyParts);
    QStringList propv;
    CSS::Property prop;

    //create css props
    for (int i=0;i<props.size();i++)
    {
        propv = props[i].split(":");
        if (propv.size() == 2)
        {
            //qDebug() << "add override prop" << propv[0] << propv[1];
            prop = CSS::Property(propv[0],propv[1],css,css->property(this,propv[0]).scaleMode(),CSS::height_props.contains(propv[0]));
            m_CSSOverrideProps.insert(propv[0],prop);
        }
        else
        {
            qDebug() << "prop without : " << props[i] << ss;
        }
    }

    //m_sCSSOverrides = css;
    buffer();
}

CSS::Property CBaseObject::cssOverrideProp(QString prop)
{
    //qDebug() << "requested css override" << prop;
    QMap<QString,CSS::Property>::Iterator it;
    it=m_CSSOverrideProps.find(prop);
    if (it==m_CSSOverrideProps.end())
        return CSS::Property();
    //CSS::Property cprop = it.value();
    //QString t = cprop.toString();
    //qDebug() << "override prop" <<  prop  << it.value().toString();
    return it.value();
}

void CBaseObject::setCSSOverrideProp(QString key, CSS::Property value)
{
    qDebug() << "override prop for object" << id() << "prop" << key << "value" << value.toString();
    if (!value.isNull())
        m_bNeedsRedraw = true;
    if (m_CSSOverrideProps.contains(key))
    {
        if (value.isNull())
            m_CSSOverrideProps.remove(key);
        else
            m_CSSOverrideProps[key] = value;
        return;
    }
    m_CSSOverrideProps.insert(key,value);
}

/*QString CBaseObject::cssOverrides()
{
    return m_sCSSOverrides;
}*/

int CBaseObject::marginTop() const
{
    return m_iMarginTop;
}
int CBaseObject::marginBottom() const
{
    return m_iMarginBottom;
}
int CBaseObject::marginLeft() const
{
    return m_iMarginLeft;
}
int CBaseObject::marginRight() const
{
    return m_iMarginRight;
}

void CBaseObject::setMargin(int top, int left, int bottom, int right)
{
    m_iMarginTop = top;
    m_iMarginBottom = bottom;
    m_iMarginLeft = left;
    m_iMarginRight = right;
}
void CBaseObject::setMarginTop(int top)
{
    m_iMarginTop = top;
}
void CBaseObject::setMarginLeft(int left)
{
    m_iMarginLeft = left;
}
void CBaseObject::setMarginBottom(int bottom)
{
    m_iMarginBottom = bottom;
}
void CBaseObject::setMarginRight(int right)
{
    m_iMarginRight = right;
}

int CBaseObject::outerHeight() const
{
    return m_rRect.height() + m_iMarginTop + m_iMarginBottom;
}

int CBaseObject::outerWidth() const
{
    return m_rRect.width() + m_iMarginLeft + m_iMarginRight;
}
/*
int CBaseObject::paddingTop() const
{


}
int CBaseObject::paddingBottom() const
{


}
int CBaseObject::paddingLeft() const
{


}
int CBaseObject::paddingRight() const
{


}

void CBaseObject::setPadding(int top, int left, int bottom, int right)
{


}
void CBaseObject::setPaddingTop(int top)
{


}
void CBaseObject::setPaddingLeft(int left)
{


}
void CBaseObject::setPaddingBottom(int bottom)
{


}
void CBaseObject::setPaddingRight(int right)
{


}

int CBaseObject::innerHeight() const
{


}
int CBaseObject::innerWidth() const
{


}*/

QStringList CBaseObject::styleClasses() const
{
    return m_StyleClasses;
}
void CBaseObject::addStyleClass(QString classname)
{
    if (m_StyleClasses.contains(classname))
        return;

    if (m_iTransitionTime > 0)
    {
        //create transition
        CSS::Selector* sel;
        QList<CSS::Property> deltaprops;
        QStringList props,props2;
        sel = document()->stylesheet()->selector("#"+section()->id()+":"+layer()->id()+":"+id()+"."+classname);
        if (sel->isEmpty())
            sel = document()->stylesheet()->selector("#"+section()->id()+"::"+id()+"."+classname);
        if (!sel->isEmpty())
        {
            props = sel->properties();
            for (int i=0;i<props.size();i++)
            {
                if (!sel->property(props[i]).isNull())
                    deltaprops.append(sel->property(props[i]));
            }
        }
        sel = document()->stylesheet()->selector("."+classname);
        if (!sel->isEmpty())
        {
            props2 = sel->properties();
            for (int i=0;i<props2.size();i++)
            {
                if (!props.contains(props2[i]) && !sel->property(props2[i]).isNull())
                    deltaprops.append(sel->property(props2[i]));
            }
        }
        CSS::Transitioner::get(thread())->createTransition(id()+"_"+classname,this,deltaprops,m_TransitionProps,m_TransitionEasing,m_iTransitionTime,m_iTransitionDelay);
        m_StyleClasses.append(classname);
    }
    else
    {
        m_StyleClasses.append(classname);
        m_bNeedsRedraw = true;
        section()->layout();
    }
}
void CBaseObject::toggleStyleClass(QString classname)
{
    if (m_StyleClasses.contains(classname))
    {
        removeStyleClass(classname);
        return;
    }
    addStyleClass(classname);
}
void CBaseObject::removeStyleClass(QString classname)
{
    if (!m_StyleClasses.contains(classname))
        return;
    if (m_iTransitionTime > 0)
    {
        //remove transition
        if (!CSS::Transitioner::get(thread())->undoTransition(id()+"_"+classname))
        {
            //create transition
            qDebug() << "create reversed transition";
            CSS::Selector* sel;
            QList<CSS::Property> deltaprops;
            QStringList props,props2;
            sel = document()->stylesheet()->selector("#"+section()->id()+":"+layer()->id()+":"+id()+"."+classname);
            if (sel->isEmpty())
                sel = document()->stylesheet()->selector("#"+section()->id()+"::"+id()+"."+classname);
            if (!sel->isEmpty())
            {
                props = sel->properties();
                for (int i=0;i<props.size();i++)
                {
                    if (!sel->property(props[i]).isNull())
                    {
                        deltaprops.append(document()->stylesheet()->property(this,props[i],true));
                        //qDebug() << props[i];
                    }
                }
            }
            sel = document()->stylesheet()->selector("."+classname);
            if (!sel->isEmpty())
            {
                props2 = sel->properties();
                for (int i=0;i<props2.size();i++)
                {
                    if (!props.contains(props2[i]) && !sel->property(props2[i]).isNull())
                        deltaprops.append(document()->stylesheet()->property(this,props2[i],true));
                }
            }
            CSS::Transitioner::get(thread())->createTransition(id()+"_"+classname,this,deltaprops,m_TransitionProps,m_TransitionEasing,m_iTransitionTime,m_iTransitionDelay);
        }
        m_StyleClasses.removeAll(classname);
    }
    else
    {
        m_StyleClasses.removeAll(classname);
        m_bNeedsRedraw = true;
        section()->layout();
    }
}

CSection* CBaseObject::section()
{
    return m_pLayer->section();
}

CDocument* CBaseObject::document()
{
    return section()->document();
}
/*
QVariant CBaseObject::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemParentHasChanged)
        layout();
    return QGraphicsObject::itemChange(change,value);
}
*/
void CBaseObject::onEPFEvent(EPFEvent *ev)
{
    if (ev->event() == "addStyleClass")
    {
        //qDebug() << "addstyleclass" << ev->parameter(0);
        if (ev->parameter(0) != "")
            addStyleClass(ev->parameter(0));
    }
    else if (ev->event() == "removeStyleClass")
    {
        if (ev->parameter(0) != "")
            removeStyleClass(ev->parameter(0));
    }
    else if (ev->event() == "toggleStyleClass")
    {
        //qDebug() << "toggleStyleClass" << ev->parameter(0);
        if (ev->parameter(0) != "")
            toggleStyleClass(ev->parameter(0));
    }
}
/*
void CBaseObject::sheduleRepaint()
{
    m_bNeedsRedraw = true;
}
*/
void CBaseObject::paintBuffered(QPainter *p)
{
    m_RenderMutex.lock();
    if (m_iRenderMode == CSS::rmNone)
    {
        m_RenderMutex.unlock();
        return;
    }
    switch (m_iRenderMode)
    {
    case CSS::rmOverlay:
        p->setCompositionMode(QPainter::CompositionMode_Overlay);
        break;
    case CSS::rmMultiply:
        p->setCompositionMode(QPainter::CompositionMode_Multiply);
        break;
    case CSS::rmReplace:
        p->setCompositionMode(QPainter::CompositionMode_SourceIn);
        break;
    case CSS::rmScreen:
        p->setCompositionMode(QPainter::CompositionMode_Screen);
        break;
    default:
        p->setCompositionMode(QPainter::CompositionMode_SourceOver);
        break;
    }
    p->rotate(m_iRotation);

    m_RenderPropsMutex.lock();
    p->setOpacity(p->opacity() * m_dOpacity);

    m_RenderPropsMutex.unlock();

    int cx = p->transform().dx();
    int dw = p->device()->width();
    int cy = p->transform().dy();
    int dh = p->device()->height();

    //draw in chunks
    //int chunksize = 512;
    qDebug() << "drawing" << id();
    QTime t = QTime::currentTime();

    if (m_qiRenderBuffer.width() <= dw && m_qiRenderBuffer.height() <= dh)
        p->drawImage(0,0,m_qiRenderBuffer);
    else
    {
        QImage chunk;
        //const uchar* bufferdata = m_qiRenderBuffer.bits();
        for (int x=0;x<m_qiRenderBuffer.width();x+=dw)
        {
            if (cx + x+dw < 0)
                continue;
            if (cx + x > dw)
                break;
            for (int y=0;y<m_qiRenderBuffer.height();y+=dh)
            {
                if (cy + y+dh < 0)
                    continue;
                if (cy + y > dh)
                    break;

                chunk = m_qiRenderBuffer.copy(x,y,dw,dh);
                //for (int line=0;line<dh;line++)
                {
                    //const uchar* buffer = bufferdata + (4 * (x + (m_qiRenderBuffer.width()*(y+line))));
                    //chunk = QImage(buffer,dw,1,m_qiRenderBuffer.format());
                    p->drawImage(x,y,chunk);
                }
                //p->drawImage(x,y,m_qiRenderBuffer,x,y,1024,1024);
            }
        }
    }
    //paint(p);

    //p->drawText(2,20,id());

    qDebug() << "rendertime:" << t.msecsTo(QTime::currentTime());

    m_RenderMutex.unlock();
}

void CBaseObject::buffer()
{
    m_RenderMutex.lock();
    m_bNeedsRedraw = false;
    /*if (m_pBuffer)
        delete m_pBuffer;*/

    //QGLFramebufferObject fbo(m_rRect.size().toSize());

    //m_pBuffer = new QGLFramebufferObject();

    m_qiRenderBuffer = QImage(m_rRect.size().toSize(),QImage::Format_ARGB32_Premultiplied);
    m_qiRenderBuffer.fill(0x00000000);

    QPainter bp;
    //bp.setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);
    //bp.begin(&fbo);
    bp.begin(&m_qiRenderBuffer);

    //bp.fillRect(QRectF(0,0,m_rRect.width(),m_rRect.height()),QColor(0,0,0,0));

    paint(&bp);

    bp.end();

    CSS::Stylesheet* css = document()->stylesheet();


    m_iRotation = css->property(this,"rotation").toInt();

    //m_qiRenderBuffer = fbo.toImage();
    m_RenderMutex.unlock();
}

void CBaseObject::keyEvent(int key, QString val)
{

}

bool CBaseObject::fixedParent()
{
    m_FPMutex.lock();
    bool b = m_bFixedParent;
    m_FPMutex.unlock();
    return b;
}

void CBaseObject::saveBuffer()
{
    m_RenderMutex.lock();
    m_qiRenderBuffer.save("buffer/"+section()->id()+"-"+id()+".png","PNG");
    m_RenderMutex.unlock();
}

bool CBaseObject::changed()
{
    bool b;
    m_mChangedMutex.lock();
    b=m_bChanged;
    m_bChanged = false;
    m_mChangedMutex.unlock();
    return b;
}

void CBaseObject::clearBuffers()
{
    m_RenderMutex.lock();
    m_qiRenderBuffer = QImage();
    m_RenderMutex.unlock();
}

int CBaseObject::renderMode()
{
    int rm;
    m_RMMutex.lock();
    rm = m_iRenderMode;
    m_RMMutex.unlock();
    return rm;
}

void CBaseObject::updateRenderMode()
{
    m_RMMutex.lock();
    m_iRenderMode = CSS::renderModeFromString(document()->stylesheet()->property(this,"render-mode").toString());
    m_RMMutex.unlock();
}
