#include "css_style.h"
#include <QRegExp>

using namespace CSS;

Stylesheet::Stylesheet(QString css)
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
    bool b;

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
                    proprules = propvalue.mid(f);
                    propvalue = propvalue.left(f);

                    b = false;

                    if (proprules.indexOf("!scale") != -1)
                        b = true;

                    prop = new CSSProperty(propvalue,b);

                }
                else
                    prop = new CSSProperty(propvalue,false);

                s->setProperty(propkey,prop);
            }
        }

        offset += propgroupfinder.cap(0).size();
    }
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
        CSSProperty* prop = new CSSProperty("",true);
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

CSSProperty::CSSProperty(QString value, Stylesheet* css, bool scale, bool null) : m_bNull(null), m_bScale(scale), m_sValue(value), m_pCSS(css)
{

}

QString CSSProperty::toString()
{
    if (m_bScale)
    {

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
