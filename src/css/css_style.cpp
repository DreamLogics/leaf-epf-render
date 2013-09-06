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

#include "css_style.h"
#include <QRegExp>
#include <QStringList>
#include "coverlay.h"
#include "csection.h"
#include "css_default.h"
#include <QDebug>
#include <qmath.h>

using namespace CSS;

Stylesheet::Stylesheet(QString css,CDocument* doc) : m_pDocument(doc)
{
    //parse default css first
    parse(pszDefaultCSS);
    parse(css);
}

Stylesheet::Stylesheet(CLayout *layout, int target_height, int target_width, CDocument* doc) : m_pDocument(doc)
{
    double w,h;
    w = layout->width();
    h = layout->height();

    w = target_width / w;
    h = target_height / h;

    setScale(h,w);
    parse(pszDefaultCSS);
    parse(layout->css());
}

Stylesheet::~Stylesheet()
{
    QMap<QString,Selector*>::Iterator it;
    for (it=m_selectors.begin();it != m_selectors.end();it++)
        delete it.value();

}

double Stylesheet::widthScaleFactor() const
{
    return m_dWSF;
}

double Stylesheet::heightScaleFactor() const
{
    return m_dHSF;
}

void Stylesheet::setScale(double height_factor, double width_factor)
{
    m_dHSF = height_factor;
    m_dWSF = width_factor;
}

Property* Stylesheet::property(QString selector, QString key)
{
    if (!m_selectors.contains(selector))
    {
        //create the selector and prop
        bool hp = false;
        if (height_props.contains(key))
            hp = true;
        Property* prop = new Property("",this,smNone,hp,true);
        Selector* s = new Selector(this);
        s->setProperty(key,prop);
        m_selectors.insert(selector,s);
        return prop;
    }

    return m_selectors[selector]->property(key);
}

void Stylesheet::setVariable(QString key, QString val)
{
    if (m_variables.contains(key))
        m_variables[key] = val;
    else
        m_variables.insert(key,val);
}

QString Stylesheet::variable(QString key)
{
    if (m_variables.contains(key))
        return m_variables[key];
    return QString();
}

Property* Stylesheet::property(CBaseObject *obj, QString key)
{
    Property* prop = 0;
    //QString selector;
    QStringList classes = obj->styleClasses();
    QMap<QString,Selector*>::Iterator it;

    //cascade from overrides > style class > object

    //overrides

    prop = obj->cssOverrideProp(key);
    if (prop)
    {
        //qDebug() << "CSS: got override for prop:" << key << "object" <<obj->id();
        return prop;
    }

    //class selectors

    for (int i=0;i<classes.size();i++)
    {
        it=m_selectors.find("#"+obj->section()->id()+"::"+obj->id()+"."+classes[i]);
        if (it != m_selectors.end())
        {
            prop = property(it.key(),key);
        }
        else
        {
            it=m_selectors.find("."+classes[i]);
            if (it != m_selectors.end())
            {
                prop = property(it.key(),key);
            }
        }
        /*for (it=m_selectors.begin();it != m_selectors.end();it++)
        {
            selector = it.key();
            if (selector == "#"+obj->id()+"."+classes[i] || selector == classes[i]
                    || selector == "#" + obj->layer()->id() + " #"+obj->id()+"."+classes[i]
                    || selector == "#" + obj->layer()->section()->id() + " #" + obj->layer()->id() + " #"+obj->id()+"."+classes[i])
            {
                prop = property(selector,key);
            }
        }*/
    }

    if (prop && prop->isNull())
        prop = 0;

    if (prop)
    {
        //indirect selector, make read only
        //qDebug() << "CSS: got class style for prop:" << key << "object" <<obj->id();
        prop->m_bReadOnly = true;
        return prop;
    }

    //object id selectors
    it=m_selectors.find("#"+obj->section()->id()+"::"+obj->id());
    if (it != m_selectors.end())
    {
        prop = property(it.key(),key);
        prop->m_bReadOnly = false;
        return prop;
    }


    return property("#"+obj->section()->id()+"::"+obj->id(),key);
}

Property* Stylesheet::property(CLayer *l, QString key)
{
    return property("#"+l->section()->id()+":"+l->id(),key);
}

Property* Stylesheet::property(CSection *s, QString key)
{
    return property("#"+s->id(),key);
}

Selector::Selector(Stylesheet* s) : m_pCSS(s)
{

}

Selector::~Selector()
{
    QMap<QString,Property*>::Iterator it;
    for (it=m_props.begin();it != m_props.end();it++)
        delete it.value();
}

Property* Selector::property(QString key)
{
    if (m_props.contains(key))
        return m_props[key];

    //create prop
    Property* prop = new Property("",m_pCSS,smNone,height_props.contains(key),true);
    m_props.insert(key,prop);
    return prop;
}

void Selector::setProperty(QString key, Property *prop)
{
    if (m_props.contains(key))
    {
        delete m_props[key];
        m_props[key] = prop;
        return;
    }

    m_props.insert(key,prop);
}

Property::Property(QString value, Stylesheet* css, ScaleMode scale, bool isHeightProp, bool null) : m_bNull(null), m_eScale(scale), m_sValue(value), m_pCSS(css), m_bHeightProp(isHeightProp)
{
    m_bReadOnly = false;
}

/*bool Property::scales()
{
    return m_bScale;
}*/

ScaleMode Property::scaleMode()
{
    return m_eScale;
}

bool Property::isNull()
{
    return m_bNull;
}

QString Property::toString()
{
    if (m_eScale != smNone)
    {
        QRegExp n("[0-9]+\\.*[0-9]*");
        int o = n.indexIn(value());
        double v = n.cap(0).toDouble();

        if (o == -1)
            return value();

        if (m_eScale == smScale)
        {
            if (m_bHeightProp)
                v *= m_pCSS->heightScaleFactor();
            else
                v *= m_pCSS->widthScaleFactor();
        }
        else if (m_eScale == smScaleWidth)
            v *= m_pCSS->widthScaleFactor();
        else if (m_eScale == smScaleHeight)
            v *= m_pCSS->heightScaleFactor();

        if (n.cap(0).indexOf(".") == -1)
        {
            return value().mid(0,o) + QString::number(qCeil(v)) + value().mid(o+n.cap(0).size());
        }

        return value().mid(0,o) + QString::number(v) + value().mid(o+n.cap(0).size());

    }
    return value();
}

QString Property::value() const
{
    QRegExp varreg("\\$([a-zA-Z_-0-9]+)");
    int offset=0;
    QString val = m_sValue;

    while (varreg.indexIn(m_sValue,offset) != -1)
    {
        val = val.replace(varreg.cap(0),m_pCSS->variable(varreg.cap(1)));
        offset += varreg.cap(0).size();
    }

    return val;
}

int Property::toInt()
{
    QString val = toString();
    QRegExp nr("([0-9]+)");
    if (nr.indexIn(val) == -1)
        return 0;

    int i=nr.cap(1).toInt();

    if (val.left(1) == "-")
        i*=-1;
    return i;
}

double Property::toDouble()
{
    QString val = toString();
    QRegExp nr("([0-9\\.]+)");
    if (nr.indexIn(val) == -1)
        return 0;
    return nr.cap(1).toDouble();
}

QColor Property::toColor()
{
    return stringToColor(value());
}

void Property::setValue(double val, ScaleMode scale)
{
    if (m_bReadOnly)
        return;
    m_bNull = false;
    m_eScale = scale;
    m_sValue = QString::number(val);
}

void Property::setValue(int val, ScaleMode scale)
{
    m_bNull = false;
    m_eScale = scale;
    m_sValue = QString::number(val);
}

void Property::setValue(QString val, ScaleMode scale)
{
    m_bNull = false;
    m_eScale = scale;
    m_sValue = val;
}

void Property::setValue(QColor val, ColorFormat format)
{
    m_bNull = false;
    m_eScale = smNone;
    m_sValue = colorToString(val,format);
}

void Stylesheet::parse(QString css)
{
    QRegExp newlines("[\n\r]+");
    QRegExp tabs("[\t]+");
    css.replace(newlines,"");
    css.replace(tabs," ");

    QString ss,propdata,propkey,propvalue,proprules;
    QStringList proplist,proper;
    Selector* s;
    Property* prop;

    int f;
    bool /*bScale,*/bHeightProp;
    ScaleMode scale;

    QRegExp outerspaces("(^ +| +$)");

    QRegExp propgroupfinder("([^\\{]+)\\{([^\\}]+)\\}");
    QRegExp varfinder("\\$([a-zA-Z_-0-9]+) *= *([^;]+);");

    int offset = 0;

    while (css.indexOf(varfinder,offset) != -1)
    {
        ss = varfinder.cap(2).replace(outerspaces,"");
        ss.replace("\"","");
        setVariable(varfinder.cap(1),ss);
        offset += varfinder.cap(0).size();
        qDebug() << "css var added" << varfinder.cap(1) << ss;
    }

    offset = 0;

    while (css.indexOf(propgroupfinder,offset) != -1)
    {
        ss = propgroupfinder.cap(1).replace(outerspaces,"");
        propdata = propgroupfinder.cap(2);

        //qDebug() << "selector found: " <<ss;

        if (m_selectors.contains(ss))
            s = m_selectors[ss];
        else
        {
            s = new Selector(this);
            m_selectors.insert(ss,s);
        }

        proplist = propdata.split(";");

        for (int i=0;i<proplist.size();i++)
        {
            proper = proplist[i].split(":");
            if (proper.size() == 2)
            {
                propkey = proper[0].replace(outerspaces,"");
                propvalue = proper[1].replace(outerspaces,"");

                //qDebug() << "prop found: " << propkey << propvalue;

                f = propvalue.indexOf("!");
                if (f != -1)
                {
                    proprules = propvalue.mid(f).toLower();
                    propvalue = propvalue.left(f).toLower();
                    propvalue = propvalue.replace(outerspaces,"");

                    //bScale = false;
                    scale = smNone;
                    bHeightProp = false;

                    if (height_props.contains(propkey))
                        bHeightProp = true;

                    /*if (proprules.indexOf("!scale") != -1)
                        bScale = true;*/

                    if (proprules.indexOf("!scale") != -1)
                        scale = smScale;

                    if (proprules.indexOf("!scale-width") != -1)
                        scale = smScaleWidth;

                    if (proprules.indexOf("!scale-height") != -1)
                        scale = smScaleHeight;


                }
                else
                {
                    bHeightProp = false;

                    if (height_props.contains(propkey))
                        bHeightProp = true;

                    //bScale = false;
                    scale = smNone;
                }

                if (propkey == "margin")
                {
                    QStringList marginprops = propvalue.split(QRegExp(" +"));
                    if (marginprops.size() == 4)
                    {
                        prop = new Property(marginprops[0],this,scale,true);
                        s->setProperty("margin-top",prop);

                        prop = new Property(marginprops[1],this,scale,false);
                        s->setProperty("margin-right",prop);

                        prop = new Property(marginprops[2],this,scale,true);
                        s->setProperty("margin-bottom",prop);

                        prop = new Property(marginprops[3],this,scale,false);
                        s->setProperty("margin-left",prop);
                    }
                    else if (marginprops.size() == 3)
                    {
                        prop = new Property(marginprops[0],this,scale,true);
                        s->setProperty("margin-top",prop);

                        prop = new Property(marginprops[1],this,scale,false);
                        s->setProperty("margin-right",prop);

                        prop = new Property(marginprops[2],this,scale,true);
                        s->setProperty("margin-bottom",prop);

                        prop = new Property(marginprops[1],this,scale,false);
                        s->setProperty("margin-left",prop);
                    }
                    else if (marginprops.size() == 2)
                    {
                        prop = new Property(marginprops[0],this,scale,true);
                        s->setProperty("margin-top",prop);

                        prop = new Property(marginprops[1],this,scale,false);
                        s->setProperty("margin-right",prop);

                        prop = new Property(marginprops[0],this,scale,true);
                        s->setProperty("margin-bottom",prop);

                        prop = new Property(marginprops[1],this,scale,false);
                        s->setProperty("margin-left",prop);
                    }
                    else
                    {
                        prop = new Property(propvalue,this,scale,true);
                        s->setProperty("margin-top",prop);

                        prop = new Property(propvalue,this,scale,false);
                        s->setProperty("margin-right",prop);

                        prop = new Property(propvalue,this,scale,true);
                        s->setProperty("margin-bottom",prop);

                        prop = new Property(propvalue,this,scale,false);
                        s->setProperty("margin-left",prop);
                    }
                }
                else if (propkey == "padding")
                {
                    QStringList paddingprops = propvalue.split(QRegExp(" +"));
                    if (paddingprops.size() == 4)
                    {
                        prop = new Property(paddingprops[0],this,scale,true);
                        s->setProperty("padding-top",prop);

                        prop = new Property(paddingprops[1],this,scale,false);
                        s->setProperty("padding-right",prop);

                        prop = new Property(paddingprops[2],this,scale,true);
                        s->setProperty("padding-bottom",prop);

                        prop = new Property(paddingprops[3],this,scale,false);
                        s->setProperty("padding-left",prop);
                    }
                    else if (paddingprops.size() == 3)
                    {
                        prop = new Property(paddingprops[0],this,scale,true);
                        s->setProperty("padding-top",prop);

                        prop = new Property(paddingprops[1],this,scale,false);
                        s->setProperty("padding-right",prop);

                        prop = new Property(paddingprops[2],this,scale,true);
                        s->setProperty("padding-bottom",prop);

                        prop = new Property(paddingprops[1],this,scale,false);
                        s->setProperty("padding-left",prop);
                    }
                    else if (paddingprops.size() == 2)
                    {
                        prop = new Property(paddingprops[0],this,scale,true);
                        s->setProperty("padding-top",prop);

                        prop = new Property(paddingprops[1],this,scale,false);
                        s->setProperty("padding-right",prop);

                        prop = new Property(paddingprops[0],this,scale,true);
                        s->setProperty("padding-bottom",prop);

                        prop = new Property(paddingprops[1],this,scale,false);
                        s->setProperty("padding-left",prop);
                    }
                    else
                    {
                        prop = new Property(propvalue,this,scale,true);
                        s->setProperty("padding-top",prop);

                        prop = new Property(propvalue,this,scale,false);
                        s->setProperty("padding-right",prop);

                        prop = new Property(propvalue,this,scale,true);
                        s->setProperty("padding-bottom",prop);

                        prop = new Property(propvalue,this,scale,false);
                        s->setProperty("padding-left",prop);
                    }
                }
                else if (propkey == "border")
                {
                    prop = new Property(propvalue,this,scale,false);
                    s->setProperty("border-top",prop);

                    prop = new Property(propvalue,this,scale,false);
                    s->setProperty("border-right",prop);

                    prop = new Property(propvalue,this,scale,false);
                    s->setProperty("border-bottom",prop);

                    prop = new Property(propvalue,this,scale,false);
                    s->setProperty("border-left",prop);
                }
                else
                {
                    prop = new Property(propvalue,this,scale,bHeightProp);
                    s->setProperty(propkey,prop);
                }
            }
        }

        offset += propgroupfinder.cap(0).size();
    }

    //propagate properties

    CLayer* l;
    CBaseObject* obj;
    Selector* base;
    QStringList baseprops,oriprops;

    //overlays
    COverlay* o;

    for (int i=0;i<m_pDocument->overlayCount();i++)
    {
        o=m_pDocument->overlay(i);
        //qDebug() << "propagating section" << cs->id();

        s = selector("overlay");

        //propagate from base section props to overlay
        base = selector("section");
        baseprops = base->properties();

        for (int n=0;n<baseprops.size();n++)
        {
            if (s->property(baseprops[n])->isNull())
                s->property(baseprops[n])->setValue(base->property(baseprops[n])->toString());
        }

        //propagate from base overlay props for overlay specific props
        s = selector("#"+o->id());
        base = selector("section");
        baseprops = base->properties();

        for (int n=0;n<baseprops.size();n++)
        {
            if (s->property(baseprops[n])->isNull())
                s->property(baseprops[n])->setValue(base->property(baseprops[n])->toString());
        }


        for (int n=0;n<o->layerCount();n++)
        {
            l=o->layer(n);
            //qDebug() << "propagating layer" << l->id();

            s = selector("#"+o->id()+" layer");

            //first propagate from the layer base
            base = selector("layer");
            baseprops = base->properties();

            for (int n=0;n<baseprops.size();n++)
            {
                if (s->property(baseprops[n])->isNull())
                    s->property(baseprops[n])->setValue(base->property(baseprops[n])->toString());
            }

            //propagate from layer base per section
            s = selector("#"+o->id()+":"+l->id());
            base = selector("#"+o->id()+" layer");
            baseprops = base->properties();

            for (int n=0;n<baseprops.size();n++)
            {
                if (s->property(baseprops[n])->isNull())
                    s->property(baseprops[n])->setValue(base->property(baseprops[n])->toString());
            }

            for (int j=0;j<l->objectCount();j++)
            {
                obj=l->object(j);
                //qDebug() << "propagating object" << obj->id();

                // propagate the section specific object base
                s = selector("#"+o->id()+" object");
                base = selector("object");
                baseprops = base->properties();

                //qDebug() << "propagating obj baseprops size" << baseprops.size();

                for (int n=0;n<baseprops.size();n++)
                {
                    //qDebug() << "#"+o->id()+" object" << baseprops[n];
                    if (s->property(baseprops[n])->isNull() || !oriprops.contains(baseprops[n]))
                        s->property(baseprops[n])->setValue(base->property(baseprops[n])->toString());
                }

                s = selector("#"+o->id()+"::"+obj->id());
                oriprops = s->properties();

                // also propagate from section object
                base = selector("section object");
                baseprops = base->properties();

                for (int n=0;n<baseprops.size();n++)
                {
                    if (s->property(baseprops[n])->isNull())
                        s->property(baseprops[n])->setValue(base->property(baseprops[n])->toString());
                }

                // now propagate from the layer base
                base = selector("#"+o->id()+" layer");
                baseprops = base->properties();

                for (int n=0;n<baseprops.size();n++)
                {
                    if (s->property(baseprops[n])->isNull() || !oriprops.contains(baseprops[n]))
                        s->property(baseprops[n])->setValue(base->property(baseprops[n])->toString());
                }

                // and propagate from the section specific object base
                base = selector("#"+o->id()+" object");
                baseprops = base->properties();

                for (int n=0;n<baseprops.size();n++)
                {
                    //qDebug() << "#"+o->id()+"::"+obj->id() << baseprops[n];
                    if (s->property(baseprops[n])->isNull() || !oriprops.contains(baseprops[n]))
                        s->property(baseprops[n])->setValue(base->property(baseprops[n])->toString());
                }

            }
        }
    }


    //sections
    CSection* cs;

    for (int i=0;i<m_pDocument->sectionCount();i++)
    {
        cs=m_pDocument->section(i);
        //qDebug() << "propagating section" << cs->id();

        s = selector("section");

        //propagate from base section props to overlay
        base = selector("section");
        baseprops = base->properties();

        for (int n=0;n<baseprops.size();n++)
        {
            if (s->property(baseprops[n])->isNull())
                s->property(baseprops[n])->setValue(base->property(baseprops[n])->toString());
        }

        //propagate from base overlay props for overlay specific props
        s = selector("#"+cs->id());
        base = selector("section");
        baseprops = base->properties();

        for (int n=0;n<baseprops.size();n++)
        {
            if (s->property(baseprops[n])->isNull())
                s->property(baseprops[n])->setValue(base->property(baseprops[n])->toString());
        }


        for (int n=0;n<cs->layerCount();n++)
        {
            l=cs->layer(n);
            //qDebug() << "propagating layer" << l->id();

            s = selector("#"+cs->id()+" layer");

            //first propagate from the layer base
            base = selector("layer");
            baseprops = base->properties();

            for (int n=0;n<baseprops.size();n++)
            {
                if (s->property(baseprops[n])->isNull())
                    s->property(baseprops[n])->setValue(base->property(baseprops[n])->toString());
            }

            //propagate from layer base per section
            s = selector("#"+cs->id()+":"+l->id());
            base = selector("#"+cs->id()+" layer");
            baseprops = base->properties();

            for (int n=0;n<baseprops.size();n++)
            {
                if (s->property(baseprops[n])->isNull())
                    s->property(baseprops[n])->setValue(base->property(baseprops[n])->toString());
            }

            for (int j=0;j<l->objectCount();j++)
            {
                obj=l->object(j);
                //qDebug() << "propagating object" << obj->id();

                // propagate the section specific object base
                s = selector("#"+cs->id()+" object");
                base = selector("object");
                baseprops = base->properties();

                //qDebug() << "propagating obj baseprops size" << baseprops.size();

                for (int n=0;n<baseprops.size();n++)
                {
                    //qDebug() << "#"+cs->id()+" object" << baseprops[n];
                    if (s->property(baseprops[n])->isNull() || !oriprops.contains(baseprops[n]))
                        s->property(baseprops[n])->setValue(base->property(baseprops[n])->toString());
                }

                s = selector("#"+cs->id()+"::"+obj->id());
                oriprops = s->properties();

                // also propagate from section object
                base = selector("section object");
                baseprops = base->properties();

                for (int n=0;n<baseprops.size();n++)
                {
                    if (s->property(baseprops[n])->isNull())
                        s->property(baseprops[n])->setValue(base->property(baseprops[n])->toString());
                }

                // now propagate from the layer base
                base = selector("#"+cs->id()+" layer");
                baseprops = base->properties();

                for (int n=0;n<baseprops.size();n++)
                {
                    if (s->property(baseprops[n])->isNull() || !oriprops.contains(baseprops[n]))
                        s->property(baseprops[n])->setValue(base->property(baseprops[n])->toString());
                }

                // and propagate from the section specific object base
                base = selector("#"+cs->id()+" object");
                baseprops = base->properties();

                for (int n=0;n<baseprops.size();n++)
                {
                    //qDebug() << "#"+cs->id()+"::"+obj->id() << baseprops[n];
                    if (s->property(baseprops[n])->isNull() || !oriprops.contains(baseprops[n]))
                        s->property(baseprops[n])->setValue(base->property(baseprops[n])->toString());
                }

            }
        }
    }

    //reverse
    /*
    QMap<QString,Selector*>::Iterator it;
    QString test;

    for (it=m_selectors.begin();it!=m_selectors.end();it++)
    {
        test += it.key();
        test += "\n{\n";
        s = it.value();
        QStringList list = s->properties();
        for (int i=0;i<list.size();i++)
            test += "\t"+list[i]+": "+s->property(list[i])->toString()+";\n";
        test += "}\n";
    }
    qDebug() << test;*/
}

void Stylesheet::addCSS(QString css)
{
    //TODO check valid
    parse(css);
}


Selector* Stylesheet::selector(QString selector)
{
    if (!m_selectors.contains(selector))
    {
        //the selector does not exist, create it
        Selector* s = new Selector(this);
        m_selectors.insert(selector,s);
        return s;
    }

    //qDebug() << m_selectors.keys();

    return m_selectors[selector];
}

Selector* Stylesheet::selector(CBaseObject *obj)
{
    return selector("#"+obj->section()->id()+"::"+obj->id());
}

QStringList Selector::properties()
{
    return QStringList(m_props.keys());
}

namespace CSS
{
    RenderMode renderModeFromString(QString str)
    {
        if (str == "normal")
            return rmNormal;
        else if (str == "overlay")
            return rmOverlay;
        else if (str == "multiply")
            return rmMultiply;
        else if (str == "replace")
            return rmReplace;
        else if (str == "screen")
            return rmScreen;
        else
            return rmNormal;
    }

    QColor stringToColor(QString color)
    {
        QRegExp rgbreg("rgb *\\( *([0-9]+) *, *([0-9]+) *, *([0-9]+) *\\)");
        QRegExp rgbareg("rgba *\\( *([0-9]+) *, *([0-9]+) *, *([0-9]+) *, *([0-9]+) *\\)");
        QRegExp hexreg("#[0-9a-fA-F]{3,6}");

        QColor c(0,0,0);

        if (hexreg.indexIn(color) != -1)
        {
            c.setNamedColor(color);
        }
        else if (rgbreg.indexIn(color) != -1)
        {
            c.setRgb(rgbreg.cap(1).toInt(),rgbreg.cap(2).toInt(),rgbreg.cap(3).toInt());
        }
        else if (rgbareg.indexIn(color) != -1)
        {
            c.setRgb(rgbareg.cap(1).toInt(),rgbareg.cap(2).toInt(),rgbareg.cap(3).toInt(),rgbareg.cap(4).toInt());
        }

        return c;
    }

    QString colorToString(QColor color, ColorFormat format)
    {
        QString s;
        switch (format)
        {
        case cfRgb:
            s = "rgb("+QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue())+")";
            break;
        case cfRgba:
            s = "rgba("+QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue())+","+QString::number(color.alpha())+")";
            break;
        default:
            s = color.name();
            break;
        }

        return s;
    }
}
