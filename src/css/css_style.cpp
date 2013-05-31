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
#include "css_default.h"

using namespace CSS;

Stylesheet::Stylesheet(QString css)
{
    m_HeightProps << "height" << "top" << "bottom" << "margin-top" << "margin-bottom" << "padding-top" << "padding-bottom";

    //parse default css first
    parse(pszDefaultCSS);
    parse(css);
}

Stylesheet::Stylesheet(CLayout *layout, int target_height, int target_width)
{
    double w,h;
    w = layout->width();
    h = layout->height();

    w = target_width / w;
    h = target_height / h;

    setScale(h,w);
    m_HeightProps << "height" << "top" << "bottom" << "margin-top" << "margin-bottom" << "padding-top" << "padding-bottom";
    parse(pszDefaultCSS);
    parse(layout->getStyle());
}

Stylesheet::~Stylesheet()
{
    QMap<QString,Selector*>::Iterator it;
    for (it=m_selectors.begin();it != m_selectors.end();it++)
        delete it.value();

}

Property* Stylesheet::property(QString selector, QString key)
{
    if (!m_selectors.contains(selector))
    {
        //create the selector and prop
        bool hp = false;
        if (m_HeightProps.contains(key))
            hp = true;
        Property* prop = new Property("",this,false,hp,true);
        Selector* s = new Selector();
        s->setProperty(key,prop);
        m_selectors.insert(selector,s);
        return prop;
    }

    return m_selectors[selector]->property(key);
}

Property* Stylesheet::property(CBaseObject *obj, QString key)
{
    Property* prop = 0;
    QString selector;
    QStringList classes = obj->styleClasses();
    QMap<QString,Selector*>::Iterator it;

    //cascade from style class > object > layer > section > document

    //class selectors

    for (int i=0;i<classes.size();i++)
    {
        for (it=m_selectors.begin();it != m_selectors.end();it++)
        {
            selector = it.key();
            if (selector == "#"+obj->id()+"."+classes[i] || selector == classes[i]
                    || selector == "#" + obj->layer()->id() + " #"+obj->id()+"."+classes[i]
                    || selector == "#" + obj->layer()->section()->id() + " #" + obj->layer()->id() + " #"+obj->id()+"."+classes[i])
            {
                prop = property(selector,key);
            }
        }
    }

    if (prop)
        return prop;

    //object id selectors

    for (it=m_selectors.begin();it != m_selectors.end();it++)
    {
        selector = it.key();
        if (selector == "#"+obj->id()
                || selector == "#" + obj->layer()->id() + " #"+obj->id()
                || selector == "#" + obj->layer()->section()->id() + " #" + obj->layer()->id() + " #"+obj->id())
        {
            prop = property(selector,key);
        }
    }

    if (prop)
        return prop;

    //layer id selectors

    for (it=m_selectors.begin();it != m_selectors.end();it++)
    {
        selector = it.key();
        if ( selector == "#" + obj->layer()->id()
                || selector == "#" + obj->layer()->section()->id() + " #" + obj->layer()->id() )
        {
            prop = property(selector,key);
        }
    }

    if (prop)
        return prop;

    //section id selector

    for (it=m_selectors.begin();it != m_selectors.end();it++)
    {
        selector = it.key();
        if ( selector == "#" + obj->layer()->section()->id() )
        {
            prop = property(selector,key);
        }
    }

    if (prop)
        return prop;

    //document selector

    for (it=m_selectors.begin();it != m_selectors.end();it++)
    {
        selector = it.key();
        if ( selector == "document" )
        {
            prop = property(selector,key);
        }
    }

    if (prop)
        return prop;

    //create the property
    prop = property("#" + obj->layer()->section()->id() + " #" + obj->layer()->id() + " #"+obj->id(),key);


    return prop;
}

Property* Stylesheet::property(CLayer *l, QString key)
{
    return 0;
}

Property* Stylesheet::property(CSection *s, QString key)
{
    return 0;
}

Selector::Selector()
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
    Property* prop = new Property("",true);
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

Property::Property(QString value, Stylesheet* css, bool scale, bool isHeightProp, bool null) : m_bNull(null), m_bScale(scale), m_sValue(value), m_pCSS(css), m_bHeightProp(isHeightProp)
{

}

QString Property::toString()
{
    if (m_bScale)
    {
        QRegExp n("[0-9]+\\.*[0-9]*");
        int o = n.indexIn(m_sValue);
        double v = n.cap(0).toDouble();

        if (o == -1)
            return m_sValue;

        if (m_bHeightProp)
            v *= m_pCSS->heightScaleFactor();
        else
            v *= m_pCSS->widthScaleFactor();

        if (n.cap(0).indexOf(".") == -1)
            return m_sValue.mid(0,o) + QString::number((int)(v)) + m_sValue(o+n.cap(0).size());

        return m_sValue.mid(0,o) + QString::number(v) + m_sValue(o+n.cap(0).size());

    }
    return m_sValue;
}

int Property::toInt()
{
    return toString().toInt();
}

double Property::toDouble()
{
    return toString().toDouble();
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
    bool bScale,bHeightProp;

    QRegExp outerspaces("(^ +| +$)");

    QRegExp propgroupfinder("([^\\{]+)\\{([^\\}]+)\\}");

    int offset = 0;


    while (css.indexOf(propgroupfinder,offset) != -1)
    {
        ss = propgroupfinder.cap(1).replace(outerspaces,"");
        propdata = propgroupfinder.cap(2);

        if (m_selectors.contains(ss))
            s = m_selectors[ss];
        else
            s = new Selector();

        proplist = propdata.split(";");

        for (int i=0;i<proplist.size();i++)
        {
            proper = proplist[i].split(":");
            if (proper.size() == 2)
            {
                propkey = proper[0].replace(outerspaces,"");
                propvalue = proper[1].replace(outerspaces,"");
                f = propvalue.indexOf("!");
                if (f != -1)
                {
                    proprules = propvalue.mid(f).toLower();
                    propvalue = propvalue.left(f).toLower();

                    bScale = false;
                    bHeightProp = false;

                    if (m_HeightProps.contains(propvalue))
                        bHeightProp = true;

                    if (proprules.indexOf("!scale") != -1)
                        bScale = true;


                }
                else
                {
                    bHeightProp = false;

                    if (m_HeightProps.contains(propvalue))
                        bHeightProp = true;

                    bScale = false;
                }

                if (propkey == "margin")
                {
                    QStringList marginprops = propvalue.split(QRegExp(" +"));
                    if (marginprops.size() == 4)
                    {
                        prop = new Property(marginprops[0],this,bScale,true);
                        s->setProperty("margin-top",prop);

                        prop = new Property(marginprops[1],this,bScale,false);
                        s->setProperty("margin-right",prop);

                        prop = new Property(marginprops[2],this,bScale,true);
                        s->setProperty("margin-bottom",prop);

                        prop = new Property(marginprops[3],this,bScale,false);
                        s->setProperty("margin-left",prop);
                    }
                    else if (marginprops.size() == 3)
                    {
                        prop = new Property(marginprops[0],this,bScale,true);
                        s->setProperty("margin-top",prop);

                        prop = new Property(marginprops[1],this,bScale,false);
                        s->setProperty("margin-right",prop);

                        prop = new Property(marginprops[2],this,bScale,true);
                        s->setProperty("margin-bottom",prop);

                        prop = new Property(marginprops[1],this,bScale,false);
                        s->setProperty("margin-left",prop);
                    }
                    else if (marginprops.size() == 2)
                    {
                        prop = new Property(marginprops[0],this,bScale,true);
                        s->setProperty("margin-top",prop);

                        prop = new Property(marginprops[1],this,bScale,false);
                        s->setProperty("margin-right",prop);

                        prop = new Property(marginprops[0],this,bScale,true);
                        s->setProperty("margin-bottom",prop);

                        prop = new Property(marginprops[1],this,bScale,false);
                        s->setProperty("margin-left",prop);
                    }
                    else
                    {
                        prop = new Property(propvalue,this,bScale,true);
                        s->setProperty("margin-top",prop);

                        prop = new Property(propvalue,this,bScale,false);
                        s->setProperty("margin-right",prop);

                        prop = new Property(propvalue,this,bScale,true);
                        s->setProperty("margin-bottom",prop);

                        prop = new Property(propvalue,this,bScale,false);
                        s->setProperty("margin-left",prop);
                    }
                }
                else if (propkey == "padding")
                {
                    QStringList paddingprops = propvalue.split(QRegExp(" +"));
                    if (paddingprops.size() == 4)
                    {
                        prop = new Property(paddingprops[0],this,bScale,true);
                        s->setProperty("padding-top",prop);

                        prop = new Property(paddingprops[1],this,bScale,false);
                        s->setProperty("padding-right",prop);

                        prop = new Property(paddingprops[2],this,bScale,true);
                        s->setProperty("padding-bottom",prop);

                        prop = new Property(paddingprops[3],this,bScale,false);
                        s->setProperty("padding-left",prop);
                    }
                    else if (marginprops.size() == 3)
                    {
                        prop = new Property(paddingprops[0],this,bScale,true);
                        s->setProperty("padding-top",prop);

                        prop = new Property(paddingprops[1],this,bScale,false);
                        s->setProperty("padding-right",prop);

                        prop = new Property(paddingprops[2],this,bScale,true);
                        s->setProperty("padding-bottom",prop);

                        prop = new Property(paddingprops[1],this,bScale,false);
                        s->setProperty("padding-left",prop);
                    }
                    else if (marginprops.size() == 2)
                    {
                        prop = new Property(paddingprops[0],this,bScale,true);
                        s->setProperty("padding-top",prop);

                        prop = new Property(paddingprops[1],this,bScale,false);
                        s->setProperty("padding-right",prop);

                        prop = new Property(paddingprops[0],this,bScale,true);
                        s->setProperty("padding-bottom",prop);

                        prop = new Property(paddingprops[1],this,bScale,false);
                        s->setProperty("padding-left",prop);
                    }
                    else
                    {
                        prop = new Property(propvalue,this,bScale,true);
                        s->setProperty("padding-top",prop);

                        prop = new Property(propvalue,this,bScale,false);
                        s->setProperty("padding-right",prop);

                        prop = new Property(propvalue,this,bScale,true);
                        s->setProperty("padding-bottom",prop);

                        prop = new Property(propvalue,this,bScale,false);
                        s->setProperty("padding-left",prop);
                    }
                }
                else
                {
                    prop = new Property(propvalue,this,bScale,bHeightProp);
                    s->setProperty(propkey,prop);
                }
            }
        }

        offset += propgroupfinder.cap(0).size();
    }
}

void Stylesheet::addCSS(QString css)
{
    //TODO check valid
    parse(css);
}

QStringList Stylesheet::properties(QString selector)
{
    if (!m_selectors.contains(selector))
        return QStringList();

    return m_selectors[selector]->properties();
}

QStringList Stylesheet::properties(CBaseObject* obj)
{
    QStringList props;
    QString s;
    QStringList classes = obj->styleClasses();
    QMap<QString,Selector*>::Iterator it;

    for (int i=0;i<classes.size();i++)
    {
        for (it=m_selectors.begin();it != m_selectors.end();it++)
        {
            s = it.key();
            if (s == "#"+obj->id()+"."+classes[i] || s == classes[i]
                    || s == "#" + obj->layer()->id() + " #"+obj->id()+"."+classes[i]
                    || s == "#" + obj->layer()->section()->id() + " #" + obj->layer()->id() + " #"+obj->id()+"."+classes[i])
            {
                props += properties(s);
            }
        }
    }

    for (it=m_selectors.begin();it != m_selectors.end();it++)
    {
        s = it.key();
        if (s == "#"+obj->id()
                || s == "#" + obj->layer()->id() + " #"+obj->id()
                || s == "#" + obj->layer()->section()->id() + " #" + obj->layer()->id() + " #"+obj->id())
        {
            props += properties(selector);
        }
    }


    props.removeDuplicates();

    return props;
}

QStringList Stylesheet::properties(CLayer* l)
{

}

QStringList Stylesheet::properties(CSection* s)
{

}

Selector* Stylesheet::selector(QString selector)
{
    if (!m_selectors.contains(selector))
        return 0;

    return m_selectors[selector];
}

QList<Selector*> Stylesheet::selectors(CBaseObject* obj)
{
    QList<Selector*> list;
    QString s;
    QStringList classes = obj->styleClasses();
    QMap<QString,Selector*>::Iterator it;

    for (int i=0;i<classes.size();i++)
    {
        for (it=m_selectors.begin();it != m_selectors.end();it++)
        {
            s = it.key();
            if (s == "#"+obj->id()+"."+classes[i] || s == classes[i]
                    || s == "#" + obj->layer()->id() + " #"+obj->id()+"."+classes[i]
                    || s == "#" + obj->layer()->section()->id() + " #" + obj->layer()->id() + " #"+obj->id()+"."+classes[i])
            {
                list.append(selector(s));
            }
        }
    }

    for (it=m_selectors.begin();it != m_selectors.end();it++)
    {
        s = it.key();
        if (s == "#"+obj->id()
                || s == "#" + obj->layer()->id() + " #"+obj->id()
                || s == "#" + obj->layer()->section()->id() + " #" + obj->layer()->id() + " #"+obj->id())
        {
            list.append(selector(s));
        }
    }

    return list;
}

QList<Selector*> Stylesheet::selectors(CLayer* l)
{

}

QList<Selector*> Stylesheet::selectors(CSection* s)
{

}
