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

using namespace CSS;

Stylesheet::Stylesheet(QString css)
{
    m_HeightProps << "height" << "top" << "bottom" << "margin-top" << "margin-bottom" << "padding-top" << "padding-bottom";
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
    parse(layout->getStyle());
}

Stylesheet::~Stylesheet()
{
    QMap<QString,CSSSelector*>::Iterator it;
    for (it=m_selectors.begin();it != m_selectors.end();it++)
        delete it.value();

}

CSSProperty* Stylesheet::property(QString selector, QString key)
{
    if (!m_selectors.contains(selector))
    {
        //create the selector and prop
        bool hp = false;
        if (m_HeightProps.contains(key))
            hp = true;
        CSSProperty* prop = new CSSProperty("",this,false,hp,true);
        CSSSelector* s = new CSSSelector();
        s->setProperty(key,prop);
        m_selectors.insert(selector,s);
        return prop;
    }

    return m_selectors[selector]->property(key);
}

CSSProperty* Stylesheet::property(CBaseObject *obj, QString key)
{
    CSSProperty* prop = 0;
    QString selector;
    QStringList classes = obj->styleClasses();
    QMap<QString,CSSSelector*>::Iterator it;

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

    //create the property
    prop = property("#" + obj->layer()->section()->id() + " #" + obj->layer()->id() + " #"+obj->id(),key);


    return prop;
}

CSSProperty* Stylesheet::property(CLayer *l, QString key)
{
    return 0;
}

CSSProperty* Stylesheet::property(CSection *s, QString key)
{
    return 0;
}

CSSSelector::CSSSelector()
{

}

CSSSelector::~CSSSelector()
{
    QMap<QString,CSSProperty*>::Iterator it;
    for (it=m_props.begin();it != m_props.end();it++)
        delete it.value();
}

CSSProperty* CSSSelector::property(QString key)
{
    if (m_props.contains(key))
        return m_props[key];

    //create prop
    CSSProperty* prop = new CSSProperty("",true);
    m_props.insert(key,prop);
    return prop;
}

void CSSSelector::setProperty(QString key, CSSProperty *prop)
{
    if (m_props.contains(key))
    {
        delete m_props[key];
        m_props[key] = prop;
        return;
    }

    m_props.insert(key,prop);
}

CSSProperty::CSSProperty(QString value, Stylesheet* css, bool scale, bool isHeightProp, bool null) : m_bNull(null), m_bScale(scale), m_sValue(value), m_pCSS(css), m_bHeightProp(isHeightProp)
{

}

QString CSSProperty::toString()
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

int CSSProperty::toInt()
{
    return toString().toInt();
}

double CSSProperty::toDouble()
{
    return toString().toDouble();
}

void Stylesheet::parse(QString css)
{


    QRegExp newlines("[\n\r]+");
    QRegExp tabs("[\t]+");
    css.replace(newlines,"");
    css.replace(tabs," ");

    QString selector,propdata,propkey,propvalue,proprules;
    QStringList proplist,proper;
    CSSSelector* s;
    CSSProperty* prop;

    int f;
    bool b,hp;

    QRegExp outerspaces("(^ +| +$)");

    QRegExp propgroupfinder("([^\\{]+)\\{([^\\}]+)\\}");

    int offset = 0;


    while (css.indexOf(propgroupfinder,offset) != -1)
    {
        selector = propgroupfinder.cap(1).replace(outerspaces,"");
        propdata = propgroupfinder.cap(2);

        if (m_selectors.contains(selector))
            s = m_selectors[selector];
        else
            s = new CSSSelector();

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

                    b = false;
                    hp = false;

                    if (m_HeightProps.contains(propvalue))
                        hp = true;

                    if (proprules.indexOf("!scale") != -1)
                        b = true;

                    prop = new CSSProperty(propvalue,this,b,hp);

                }
                else
                {
                    hp = false;

                    if (m_HeightProps.contains(propvalue))
                        hp = true;

                    prop = new CSSProperty(propvalue,this,false,hp,false);
                }

                s->setProperty(propkey,prop);
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
    QString selector;
    QStringList classes = obj->styleClasses();
    QMap<QString,CSSSelector*>::Iterator it;

    for (int i=0;i<classes.size();i++)
    {
        for (it=m_selectors.begin();it != m_selectors.end();it++)
        {
            selector = it.key();
            if (selector == "#"+obj->id()+"."+classes[i] || selector == classes[i]
                    || selector == "#" + obj->layer()->id() + " #"+obj->id()+"."+classes[i]
                    || selector == "#" + obj->layer()->section()->id() + " #" + obj->layer()->id() + " #"+obj->id()+"."+classes[i])
            {
                props += properties(selector);
            }
        }
    }

    for (it=m_selectors.begin();it != m_selectors.end();it++)
    {
        selector = it.key();
        if (selector == "#"+obj->id()
                || selector == "#" + obj->layer()->id() + " #"+obj->id()
                || selector == "#" + obj->layer()->section()->id() + " #" + obj->layer()->id() + " #"+obj->id())
        {
            props += properties(selector);
        }
    }


    props.removeDuplicates();

    return props;
}

QList<QString> Stylesheet::properties(CLayer* l)
{

}

QList<QString> Stylesheet::properties(CSection* s)
{

}
