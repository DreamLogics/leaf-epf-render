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
#include <QGLPixelBuffer>
#include <QGuiApplication>

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
    m_iInTransition = 0;
    m_iTransitionStarted = 0;

    m_pJSProxy =  new JSBaseObjectProxy(this);

    //setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

CBaseObject::~CBaseObject()
{
    //if (m_pBuffer)
    //    delete m_pBuffer;
    delete m_pJSProxy;

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
    //qDebug() << "preload func";
}

QString CBaseObject::id() const
{
    return m_sID;
}

const char* CBaseObject::objectType() const
{
    return "base";
}

void CBaseObject::layout(QRectF relrect, QList<CBaseObject*> updatelist)
{
    QString pos;
    CSS::Stylesheet* css = document()->stylesheet();
    //QElapsedTimer bitch;
    //bitch.start();

    if ((updatelist.size() > 0 && updatelist.contains(this)) || updatelist.size() == 0)
    {
        //qDebug() << "layout" << id();
        QRectF oldrect = m_rRect;
        QRectF newrect = m_rRect;
        //QRectF relrect = r->boundingRect();

        updateRenderMode();

        //if (CSS::Transitioner::get(thread())->hasTransitions(this))
        //    css->oldState(true);
        

        

        m_RenderPropsMutex.lock();

        m_dOpacity = styleProperty("opacity").toDouble();
        //if (id() == "menu-sorting-text")
            //qDebug() << "whaaaaa" << m_dOpacity;

        m_RenderPropsMutex.unlock();

        //qDebug() << "t1" << bitch.nsecsElapsed();

        //qDebug()() << "test ret val" << styleProperty("opacity").value();

        setMargin(styleProperty("margin-top").toInt(),styleProperty("margin-left").toInt(),styleProperty("margin-bottom").toInt(),styleProperty("margin-right").toInt());
        //setPadding(styleProperty("padding-top").toInt(),styleProperty("padding-left").toInt(),styleProperty("padding-bottom").toInt(),styleProperty("padding-right").toInt());

        newrect.setHeight(styleProperty("height").toInt()+styleProperty("mod-height").toInt());
        newrect.setWidth(styleProperty("width").toInt()+styleProperty("mod-width").toInt());

        //qDebug()() << "CBaseObject::layout" << m_rRect.size() << "#"+section()->id()+"::"+id() << styleProperty("height").toString();

        /*if (styleProperty("relative-to").toString() == "")
        {
            m_rRect.setTop(relrect.top() + r->outerHeight() + marginTop());
            m_rRect.setLeft(styleProperty("left").toInt() + marginLeft());

            //top/left/bottom/right negeren
        }
        else*/
        //qDebug() << "t1a" << bitch.nsecsElapsed();
        pos = styleProperty("position").toString();

        //qDebug() << "t2" << bitch.nsecsElapsed();


        if (pos != "static" && pos != "relative")
        {
            newrect.moveTop(relrect.top() + styleProperty("top").toInt());
            newrect.moveLeft(relrect.left() + styleProperty("left").toInt());

            //qDebug()() << "CBaseObject::layout" << newrect << "#"+section()->id()+"::"+id() << styleProperty("top").toInt();

            //m_rRect.setHeight(styleProperty("height").toInt());
            //m_rRect.setWidth(styleProperty("width").toInt());

            if (!styleProperty("bottom").isNull() && !styleProperty("top").isNull())
            {
                newrect.setBottom(relrect.bottom() - styleProperty("bottom").toInt());
                newrect.setTop(newrect.bottom() - styleProperty("height").toInt());
            }
            else if (!styleProperty("bottom").isNull())
                newrect.moveBottom(relrect.bottom() - styleProperty("bottom").toInt());

            if (!styleProperty("right").isNull() && !styleProperty("left").isNull())
            {
                newrect.setRight(relrect.right() - styleProperty("right").toInt());
                newrect.setLeft(newrect.left() - styleProperty("width").toInt());
            }
            else if (!styleProperty("right").isNull())
                newrect.moveRight(relrect.right() - styleProperty("right").toInt());
        }
        else
        {
            newrect.moveTop(relrect.top()+marginTop());
            newrect.moveLeft(relrect.left()+marginLeft());
        }

        //qDebug() << "t3" << bitch.nsecsElapsed();

        //mod position
        CSS::Property modtop = styleProperty("mod-top");
        CSS::Property modbot = styleProperty("mod-bottom");
        CSS::Property modleft = styleProperty("mod-left");
        CSS::Property modright = styleProperty("mod-right");

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
        if (!styleProperty("min-height").isNull() && newrect.height() < styleProperty("min-height").toInt())
            newrect.setHeight(styleProperty("min-height").toInt());

        if (!styleProperty("min-width").isNull() && newrect.width() < styleProperty("min-width").toInt())
            newrect.setHeight(styleProperty("min-width").toInt());

        if (!styleProperty("max-height").isNull() && newrect.height() > styleProperty("max-height").toInt())
            newrect.setHeight(styleProperty("max-height").toInt());

        if (!styleProperty("max-width").isNull() && newrect.width() > styleProperty("max-width").toInt())
            newrect.setHeight(styleProperty("max-width").toInt());
        //qDebug() << "t4" << bitch.nsecsElapsed();

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

        //qDebug() << "t5" << bitch.nsecsElapsed();

        m_FPMutex.lock();

        CBaseObject* obj = dynamic_cast<CBaseObject*>(parent());
        if (obj)
        {
            if (css->property(obj,"position").toString() == "fixed" || obj->fixedParent())
                m_bFixedParent = true;
        }

        m_FPMutex.unlock();

        //qDebug() << "t6" << bitch.nsecsElapsed();

        //m_bCSSOldState = false;

        //css->oldState(false);

        if (m_iInTransition == 0)
        {

            //animation
            CSS::Property anim = styleProperty("animation-name");

            if (!anim.isNull() && anim.toString() != "")
            {
                CSS::Property anim_duration = styleProperty("animation-duration");
                CSS::Property anim_timing_function = styleProperty("animation-timing-function");
                CSS::Property anim_delay = styleProperty("animation-delay");
                CSS::Property anim_iteration_count = styleProperty("animation-iteration-count");
                CSS::Property anim_direction = styleProperty("animation-direction");

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

        }
    }

    //qDebug() << "t7" << bitch.nsecsElapsed();

    //qDebug()() << "CBaseObject::layout parent" << "#"+section()->id()+"::"+id() << pos;

    QObjectList clist = children();
    CBaseObject* cobj;
    QRectF relr = m_rRect;
    QRectF fixr(0,0,section()->width(),section()->height());
    for (int i=0;i<clist.size();i++)
    {
        if (document()->shouldStopLayout())
            return;
        cobj = dynamic_cast<CBaseObject*>(clist[i]);
        if (cobj)
        {
            pos = css->property(cobj,"position").toString();
            //qDebug()() << "CBaseObject::layout" << "#"+section()->id()+"::"+cobj->id() << pos;

            if (pos == "absolute")
                cobj->layout(m_rRect,updatelist);
            else if (pos == "fixed")
                cobj->layout(fixr,updatelist);
            else
            {
                cobj->layout(relr,updatelist);
                relr.setTop(cobj->boundingRect().bottom());
            }
        }
    }

    //qDebug() << "t8" << bitch.nsecsElapsed();
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
    m_pJSProxy->_clicked();
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
            //qDebug()() << "add override prop" << propv[0] << propv[1];
            prop = CSS::Property(propv[0],propv[1],css,CSS::valueTypeFromString(propv[1]),css->property(this,propv[0]).scaleMode(),CSS::height_props.contains(propv[0]));
            m_CSSOverrideProps.insert(propv[0],prop);
        }
        else
        {
            //qDebug() << "prop without : " << props[i] << ss;
        }
    }

    //m_sCSSOverrides = css;
    buffer();
}

CSS::Property CBaseObject::cssOverrideProp(QString prop)
{
    //qDebug()() << "requested css override" << prop;
    QMap<QString,CSS::Property>::Iterator it;

    it=m_CSSOverrideProps.find(prop);
    if (it==m_CSSOverrideProps.end())
        return CSS::Property();

    //CSS::Property cprop = it.value();
    //QString t = cprop.toString();
    //qDebug()() << "override prop" <<  prop  << it.value().toString();
    return it.value();
}

void CBaseObject::setCSSOverrideProp(QString key, CSS::Property value)
{
    //qDebug()() << "override prop for object" << id() << "prop" << key << "value" << value.toString();
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

    //invalidate css cache for this object
    if (document()->stylesheet())
        document()->stylesheet()->invalidateCache(this);

    if (m_iTransitionTime > 0)
    {
        //create transition
        CSS::Selector* sel;
        QList<CSS::Property> deltaprops;
        QStringList props,props2,props3;
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
                    deltaprops.append(sel->property(props[i]));
                    props3.append(props[i]);
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
                {
                    deltaprops.append(sel->property(props2[i]));
                    props3.append(props2[i]);
                }
            }
        }
        CSS::Transitioner::get(thread())->removeTransitioningProps(this,props3);
        m_iInTransition++;
        m_iTransitionStarted++;
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
    //invalidate css cache for this object
    if (document()->stylesheet())
        document()->stylesheet()->invalidateCache(this);
    if (m_iTransitionTime > 0)
    {
        //remove transition
        if (!CSS::Transitioner::get(thread())->undoTransition(id()+"_"+classname))
        {
            //create transition
            //qDebug() << "create reversed transition";
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
                        //qDebug()() << props[i];
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
            m_iInTransition++;
            m_iTransitionStarted++;
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

void CBaseObject::setStyleClass(QString classname)
{
    m_StyleClasses.clear();
    addStyleClass(classname);
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
        //qDebug()() << "addstyleclass" << ev->parameter(0);
        if (ev->parameter(0) != "")
            addStyleClass(ev->parameter(0));
    }
    else if (ev->event() == "removeStyleClass")
    {
        if (ev->parameter(0) != "")
            removeStyleClass(ev->parameter(0));
    }
    else if (ev->event() == "setStyleClass")
    {
        if (ev->parameter(0) != "")
            setStyleClass(ev->parameter(0));
    }
    else if (ev->event() == "toggleStyleClass")
    {
        //qDebug()() << "toggleStyleClass" << ev->parameter(0);
        if (ev->parameter(0) != "")
            toggleStyleClass(ev->parameter(0));
    }
}

bool CBaseObject::onStylesheetVariableChange(QString key, QString val, QString oval)
{
    QList<CSS::Property> propsold,dprops;
    //QString oldval;

    //QStringList propnames;
    QElapsedTimer t;
    t.start();
    propsold = document()->stylesheet()->properties(this);
    for (int i=0;i<propsold.size();i++)
    {
        if (!propsold[i].hasVariable(key))
            continue;

        if (m_iTransitionTime <= 0)
        {
            m_bNeedsRedraw = true;
            return true;
        }
        //document()->stylesheet()->oldState(true);
        //oldval = propsold[i].value();
        //document()->stylesheet()->oldState(false);
        //CSS::Property prop = document()->stylesheet()->property(this,propsold[i].name());
        //if (oldval != propsold[i].value()/*prop.value()*/)
        {
            //qDebug()() << "prop changed" << propsold[i].name() << oldval << prop.value();
            document()->stylesheet()->oldState(true);
            dprops.append(propsold[i].clone());
            document()->stylesheet()->oldState(false);
        }
    }

    //qDebug() << "onStylesheetVariableChange time" << t.nsecsElapsed();
    if (dprops.size() == 0)
        return false;
    //a variable has changes, update and possibly create a transition
    //if (m_iTransitionTime > 0)
    {
        //CSS::Transitioner::get(thread())->removeTransitions(this);
        //static int count=0;
        //count++;
        //qDebug()() << "onStylesheetVariableChange" << id() << ev->parameter(0) << ev->parameter(1) << count;
        CSS::Transitioner::get(thread())->removeTransition(id()+"_"+key);
        m_iInTransition++;
        m_iTransitionStarted++;
        CSS::Transitioner::get(thread())->createTransition(id()+"_"+key,this,dprops,m_TransitionProps,m_TransitionEasing,m_iTransitionTime,m_iTransitionDelay,true);

        //revert css var
        /*if (m_CSSVariableSetter.contains(id()+"_"+ev->parameter(0)))
            m_CSSVariableSetter[id()+"_"+ev->parameter(0)] = QPair<QString,QString>(ev->parameter(0),ev->parameter(1));
        else
            m_CSSVariableSetter.insert(id()+"_"+ev->parameter(0),QPair<QString,QString>(ev->parameter(0),ev->parameter(1)) );
        document()->stylesheet()->setVariable(ev->parameter(0),ev->parameter(2));*/
        //m_bNeedsRedraw = true;
        return false;
    }
    /*else
        m_bNeedsRedraw = true;*/

    return true;
}

/*
void CBaseObject::sheduleRepaint()
{
    m_bNeedsRedraw = true;
}
*/
void CBaseObject::paintBuffered(QPainter *p)
{
    if (m_iRenderMode == CSS::rmNone)
        return;

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

    p->setOpacity(p->opacity() * m_dOpacity);


    /*int cx = p->transform().dx();
    int dw = p->device()->width();
    int cy = p->transform().dy();
    int dh = p->device()->height();*/


    p->drawPixmap(0,0,m_qiRenderBuffer);


    //paint children
    QString pos;
    QObjectList clist = children();
    CBaseObject* cobj;


    p->resetTransform();

    for (int i=0;i<clist.size();i++)
    {

        cobj = dynamic_cast<CBaseObject*>(clist[i]);
        if (cobj)
        {
            pos = document()->stylesheet()->property(cobj,"position").toString();
            //qDebug()() << "CBaseObject::layout" << "#"+section()->id()+"::"+cobj->id() << pos;

            if (!(pos == "fixed"/* || cobj->fixedParent()*/))
            {
                p->save();
                p->translate(cobj->boundingRect().x() - section()->scrollX(),cobj->boundingRect().y() - section()->scrollY());
                cobj->paintBuffered(p);
                p->restore();
            }
        }
    }
}

void CBaseObject::buffer()
{
    //if (m_iInTransition > 0)
    //    return;
    m_RenderMutex.lock();
    m_bNeedsRedraw = false;
    /*if (m_pBuffer)
        delete m_pBuffer;*/
    QElapsedTimer t;
    t.start();
    //QGLFramebufferObject fbo(m_rRect.size().toSize());

    //m_pBuffer = new QGLFramebufferObject();

    if (useDevicePixels())
    {
        qreal dpr = qApp->devicePixelRatio();
        m_qiRenderBuffer = QPixmap(m_rRect.size().toSize()*dpr);
        //m_qiRenderBuffer = QImage(m_rRect.size().toSize()*dpr,QImage::Format_ARGB32_Premultiplied);
        m_qiRenderBuffer.setDevicePixelRatio(dpr);
    }
    else
    {
        m_qiRenderBuffer = QPixmap(m_rRect.size().toSize());
        //m_qiRenderBuffer = QImage(m_rRect.size().toSize(),QImage::Format_ARGB32_Premultiplied);
    }

    m_qiRenderBuffer.fill(QColor(0,0,0,0));
    //m_qiRenderBuffer.fill(0x00000000);
    //QGLFormat glf(QGLFormat::defaultFormat());
    //glf.setRgba(true);
    //QGLPixelBuffer glpb(m_rRect.size().toSize(),glf);

    QPainter bp;
    //bp.setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);
    //bp.begin(&fbo);
    bp.begin(&m_qiRenderBuffer);
    //bp.begin(&glpb);

    //bp.fillRect(QRectF(0,0,m_rRect.width(),m_rRect.height()),QColor(0,0,0,0));

    paint(&bp);

    bp.end();

    //create mask
    /*QImage src(m_qiRenderBuffer.toImage());
    QImage mask(src.size(),QImage::Format_ARGB32_Premultiplied);

    bp.begin(&mask);
    bp.drawImage(0,0,src);
    bp.setCompositionMode(QPainter::CompositionMode_SourceIn);
    bp.fillRect(src.rect(),QColor(0,0,0));
    bp.end();

    //determine size for post proccessed image
    //TODO: resize

    //post proccessing
    bp.begin(&m_qiRenderBuffer);
    paintPost(&bp,mask,src);
    bp.end();*/

    qDebug() << "buffertime" << t.nsecsElapsed();

    //m_qiRenderBuffer = glpb.toImage();

    //CSS::Stylesheet* css = document()->stylesheet();


    m_iRotation = styleProperty("rotation").toInt();

    //m_qiRenderBuffer = fbo.toImage();
    m_RenderMutex.unlock();
}

void CBaseObject::paintPost(QPainter *painter, QImage &mask, QImage &source)
{

}

void CBaseObject::keyPressEvent(int key, QString val)
{

}

void CBaseObject::keyReleaseEvent(int key, QString val)
{

}

bool CBaseObject::fixedParent()
{
    m_FPMutex.lock();
    bool b = m_bFixedParent;
    m_FPMutex.unlock();
    return b;
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

void CBaseObject::unload()
{
    m_RenderMutex.lock();
    m_qiRenderBuffer = QPixmap();
    //m_qiRenderBuffer = QImage();
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
    m_iRenderMode = CSS::renderModeFromString(styleProperty("render-mode").toString());
    m_RMMutex.unlock();
}

void CBaseObject::transitionDone(QString transition)
{
    //qDebug()() << "transition done" << transition;
    m_iInTransition--;
    /*if (m_CSSVariableSetter.contains(transition))
    {
        document()->stylesheet()->setVariable(m_CSSVariableSetter[transition].first,m_CSSVariableSetter[transition].second);
    }*/
}

CSS::Property CBaseObject::styleProperty(QString key)
{
    //QElapsedTimer bitch;
    //bitch.start();
    CSS::Stylesheet* css = document()->stylesheet();
    CSS::Property prop = cssOverrideProp(key);
    //qDebug() << "st1" << bitch.nsecsElapsed();
    //if (key == "opacity")
    //qDebug() << "request for opacity prop" << id() << styleClasses() << prop.isNull();
    if (prop.isNull())
        prop = css->property(this,key);
    //qDebug() << "st2" << bitch.nsecsElapsed();
    //qDebug() << prop.isNull() << prop.toString();
    if (prop.isNull())
        return prop;
    if (m_iTransitionStarted > 0)
    {
        CSS::Property nprop = prop.clone();
        css->oldState(true);
        nprop.setValue(prop.value());
        css->oldState(false);
        //qDebug() << "st3" << bitch.nsecsElapsed();
        return nprop;
    }
    return prop;
}

void CBaseObject::onStylesheetChange()
{
    //update all overrides with new stylesheet info
    CSS::Stylesheet* css = document()->stylesheet();
    QMap<QString,CSS::Property>::iterator it;

    for (it = m_CSSOverrideProps.begin();it != m_CSSOverrideProps.end();it++)
    {
        it.value().update(css);
    }
}

void CBaseObject::transitionStarted()
{
    m_iTransitionStarted--;
}

QObject* CBaseObject::jsProxy() const
{
    return m_pJSProxy;
}

bool CBaseObject::useDevicePixels() const
{
    return true;
}

JSBaseObjectProxy::JSBaseObjectProxy(CBaseObject* obj) : QObject(obj), m_pObject(obj)
{

}

void JSBaseObjectProxy::_clicked()
{
    emit clicked();
}
void JSBaseObjectProxy::_onMouseDown(int posx, int posy)
{
    emit onMouseDown( posx,  posy);
}
void JSBaseObjectProxy::_onMouseMove(int posx, int posy)
{
    emit onMouseMove( posx,  posy);
}
void JSBaseObjectProxy::_onMouseUp(int posx, int posy)
{
    emit onMouseUp( posx,  posy);
}
void JSBaseObjectProxy::_onMouseEnter()
{
    emit onMouseEnter();
}
void JSBaseObjectProxy::_onMouseLeave()
{
    emit onMouseLeave();
}
void JSBaseObjectProxy::_onSwipe(int direction)
{
    emit onSwipe( direction);
}
void JSBaseObjectProxy::_onFocus()
{
    emit onFocus();
}
void JSBaseObjectProxy::_onLoseFocus()
{
    emit onLoseFocus();
}

QStringList JSBaseObjectProxy::styleClasses() const
{
    return m_pObject->styleClasses();
}
void JSBaseObjectProxy::addStyleClass(QString classname)
{
    m_pObject->addStyleClass(classname);
}
void JSBaseObjectProxy::setStyleClass(QString classname)
{
    m_pObject->setStyleClass(classname);
}
void JSBaseObjectProxy::toggleStyleClass(QString classname)
{
    m_pObject->toggleStyleClass(classname);
}
void JSBaseObjectProxy::removeStyleClass(QString classname)
{
    m_pObject->removeStyleClass(classname);
}

void JSBaseObjectProxy::setBoundingRect(const QRectF &r)
{
    m_pObject->setBoundingRect(r);
}
QRectF JSBaseObjectProxy::boundingRect() const
{
    return m_pObject->boundingRect();
}
