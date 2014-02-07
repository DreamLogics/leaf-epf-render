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
#include "../coverlay.h"
#include "../cbaseobject.h"
#include "../clayer.h"
#include "../csection.h"
#include "../clayout.h"
#include "../cdocument.h"
#include "css_default.h"
#include "css_animation.h"
#include <QDebug>
#include <qmath.h>

using namespace CSS;

Stylesheet::Stylesheet(QString css,CDocument* doc) : m_pDocument(doc), m_bOldState(false)
{
    //parse default css first
    parse(pszDefaultCSS);
    parse(css);
}

Stylesheet::Stylesheet(CLayout *layout, int target_height, int target_width, CDocument* doc) : m_pDocument(doc), m_bOldState(false)
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
    QHash<QString,Selector*>::Iterator it;
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

Property Stylesheet::property(QString selector, QString key)
{
    if (!m_selectors.contains(selector))
    {
        //create the selector and prop
        bool hp = false;
        if (height_props.contains(key))
            hp = true;
        Property prop = Property(key,"",this,vtUndefined,smNone,hp,true);
        Selector* s = new Selector(this);
        s->setProperty(key,prop);
        m_selectors.insert(selector,s);
        return prop;
    }

    return m_selectors[selector]->property(key);
}

void Stylesheet::setVariable(QString key, QString val, bool initial)
{
    if (m_variables.contains(key))
    {
        m_prevariables[key] = m_variables[key];
        m_variables[key] = val;
    }
    else
        m_variables.insert(key,val);
    if (!initial)
    {
        if (!m_alteredVars.contains(key))
            m_alteredVars.append(key);
    }
}

QString Stylesheet::variable(QString key)
{
    if (m_bOldState)
    {
        if (m_prevariables.contains(key))
            return m_prevariables[key];
    }
    else
    {
        if (m_variables.contains(key))
            return m_variables[key];
    }
    return QString();
}

QList<QString> Stylesheet::variables(bool altered_only)
{
    if (altered_only)
        return m_alteredVars;
    return m_variables.keys();
}

Property Stylesheet::property(CBaseObject *obj, QString key, bool bIgnoreOverrides)
{
    Property prop;
    //QString selector;
    QStringList classes = obj->styleClasses();
    QHash<QString,Selector*>::Iterator it;
    //QElapsedTimer bitch;
    //bitch.start();

    //cascade from overrides > style class > object

    //overrides
    if (!bIgnoreOverrides)
    {
        //qDebug() << "Stylesheet::property c1" << bitch.nsecsElapsed();
        if (m_cachedProperty.contains(obj))
        {
            //qDebug() << "Stylesheet::property c2" << bitch.nsecsElapsed();
            if (m_cachedProperty[obj].contains(key))
            {
                //qDebug() << "Stylesheet::property c3" << bitch.nsecsElapsed();
                //qDebug()() << "css from cache";
                return m_cachedProperty[obj][key];
            }
        }


        /*prop = obj->cssOverrideProp(key);
        if (!prop.isNull())
        {
            //qDebug()() << "CSS: got override for prop:" << key << "object" <<obj->id();
            return prop;
        }*/


        //class selectors

        for (int i=0;i<classes.size();i++)
        {
            it=m_selectors.find("#"+obj->section()->id()+"::"+obj->id()+"."+classes[i]);
            if (it != m_selectors.end())
            {
                prop = property(it.key(),key);
            }

            if (prop.isNull())
            {
                it=m_selectors.find("."+classes[i]);
                if (it != m_selectors.end())
                {
                    //qDebug()() << "from" <<it.key();
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

        if (!prop.isNull())
        {
            //indirect selector, make read only
            //qDebug()() << "CSS: got class style for prop:" << key << "object" <<obj->id();
            prop.m_pPrivate->m_bReadOnly = true;
            if (m_cachedProperty.contains(obj))
                m_cachedProperty[obj].insert(key,prop);
            else
            {
                m_cachedProperty.insert(obj,QMap<QString,Property>());
                m_cachedProperty[obj].insert(key,prop);
            }
            return prop;
        }
    }
    else if (m_cachedPropertyIgnOv.contains(obj))
    {
        if (m_cachedPropertyIgnOv[obj].contains(key))
            return m_cachedPropertyIgnOv[obj][key];
    }

    //object id selectors
    /*it=m_selectors.find("#"+obj->section()->id()+"::"+obj->id());
    if (it != m_selectors.end())
    {
        prop = property(it.key(),key);
        prop.m_pPrivate->m_bReadOnly = false;
        return prop;
    }*/
    Selector* sel = selector(obj);

    if (bIgnoreOverrides)
    {
        if (m_cachedPropertyIgnOv.contains(obj))
            m_cachedPropertyIgnOv[obj].insert(key,sel->property(key));
        else
        {
            m_cachedPropertyIgnOv.insert(obj,QMap<QString,Property>());
            m_cachedPropertyIgnOv[obj].insert(key,sel->property(key));
        }
    }
    else
    {
        if (m_cachedProperty.contains(obj))
            m_cachedProperty[obj].insert(key,sel->property(key));
        else
        {
            m_cachedProperty.insert(obj,QMap<QString,Property>());
            m_cachedProperty[obj].insert(key,sel->property(key));
        }
    }

    return sel->property(key);
    //return property("#"+obj->section()->id()+"::"+obj->id(),key);
}

QList<Property> Stylesheet::properties(CBaseObject *obj, bool bIgnoreOverrides)
{
    QList<Property> retprops;
    Selector* sel;
    QStringList classes = obj->styleClasses();
    QStringList props;

    //cascade from overrides > style class > object

    //overrides
    if (!bIgnoreOverrides)
    {
        if (m_cachedProplist.contains(obj))
        {
            return m_cachedProplist[obj];
        }

        //TODO override props


        //class selectors

        for (int i=0;i<classes.size();i++)
        {
            sel = selector("#"+obj->section()->id()+":"+obj->layer()->id()+":"+obj->id()+"."+classes[i]);
            if (sel->isEmpty())
                sel = selector("#"+obj->section()->id()+"::"+obj->id()+"."+classes[i]);
            if (!sel->isEmpty())
            {
                props += sel->properties();

            }
            sel = selector("."+classes[i]);
            if (!sel->isEmpty())
            {
                props += sel->properties();

            }
        }
    }
    else if (m_cachedProplistIgnOv.contains(obj))
        return m_cachedProplistIgnOv[obj];

    sel = selector(obj);
    props += sel->properties();
    props.removeDuplicates();

    for (int i=0;i<props.size();i++)
        retprops.append(property(obj,props[i],bIgnoreOverrides));

    if (bIgnoreOverrides)
        m_cachedProplistIgnOv.insert(obj,retprops);
    else
        m_cachedProplist.insert(obj,retprops);

    return retprops;
}

void Stylesheet::invalidateCache(CBaseObject *obj)
{
    if (obj)
    {
        if (m_cachedProperty.contains(obj))
            m_cachedProperty.remove(obj);
        if (m_cachedPropertyIgnOv.contains(obj))
            m_cachedPropertyIgnOv.remove(obj);
        if (m_cachedProplist.contains(obj))
            m_cachedProplist.remove(obj);
        if (m_cachedProplistIgnOv.contains(obj))
            m_cachedProplistIgnOv.remove(obj);
    }
    else
    {
        m_cachedProperty.clear();
        m_cachedPropertyIgnOv.clear();
        m_cachedProplist.clear();
        m_cachedProplistIgnOv.clear();
    }
}

Property Stylesheet::property(CLayer *l, QString key)
{
    return property("#"+l->section()->id()+":"+l->id(),key);
}

Property Stylesheet::property(CSection *s, QString key)
{
    return property("#"+s->id(),key);
}

Selector::Selector(Stylesheet* s) : m_pCSS(s)
{

}

Selector::~Selector()
{

}

Property Selector::property(QString key)
{
    if (m_props.contains(key))
        return m_props[key];

    //create prop
    Property prop = Property(key,"",m_pCSS,vtUndefined,smNone,height_props.contains(key),true);
    m_props.insert(key,prop);
    return prop;
}

void Selector::setProperty(QString key, const Property prop)
{
    if (m_props.contains(key))
    {
        m_props[key] = prop;
        return;
    }

    m_props.insert(key,prop);
}

Property::Property(QString name, QString value, Stylesheet* css, ValueType vt, ScaleMode scale, bool isHeightProp, bool null) : m_sName(name), m_eValueType(vt)
{
    m_pPrivate = new PropertyPrivate();
    m_pPrivate->m_sValue = value;
    m_pPrivate->m_pCSS = css;
    m_pPrivate->m_eScale = scale;
    m_pPrivate->m_bHeightProp = isHeightProp;
    m_pPrivate->m_bNull = null;
    m_pPrivate->m_bReadOnly = false;
}

Property::Property(QString name, Stylesheet* css, ValueType vt) : m_sName(name), m_eValueType(vt)
{
    m_pPrivate = new PropertyPrivate();
    m_pPrivate->m_sValue = QString();
    m_pPrivate->m_pCSS = css;
    m_pPrivate->m_eScale = smNone;
    m_pPrivate->m_bHeightProp = false;
    m_pPrivate->m_bNull = true;
    m_pPrivate->m_bReadOnly = false;
}

Property::Property() : m_sName(QString())
{
    m_pPrivate = new PropertyPrivate();
    m_pPrivate->m_sValue = QString();
    m_pPrivate->m_pCSS = 0;
    m_pPrivate->m_eScale = smNone;
    m_pPrivate->m_bHeightProp = false;
    m_pPrivate->m_bNull = true;
    m_pPrivate->m_bReadOnly = true;
}

Property::Property(const Property &p)
{
    m_sName = p.m_sName;
    m_eValueType = p.m_eValueType;
    m_pPrivate = p.m_pPrivate;
    m_pPrivate->registerUse();


    /*m_pPrivate = new PropertyPrivate();
    m_pPrivate->m_sValue = p.m_pPrivate->m_sValue;
    m_pPrivate->m_pCSS = p.m_pPrivate->m_pCSS;
    m_pPrivate->m_eScale = p.m_pPrivate->m_eScale;
    m_pPrivate->m_bHeightProp = p.m_pPrivate->m_bHeightProp;
    m_pPrivate->m_bNull = p.m_pPrivate->m_bNull;
    m_pPrivate->m_bReadOnly = p.m_pPrivate->m_bReadOnly;*/

}

Property Property::clone() const
{
    return Property(name(),value(),m_pPrivate->m_pCSS,valueType(),scaleMode(),m_pPrivate->m_bHeightProp,isNull());
}

bool Property::operator ==(const Property& other)
{
    return name() == other.name();
}

Property::~Property()
{
    m_pPrivate->unregisterUse();
    if (m_pPrivate->isUnreferenced())
        delete m_pPrivate;
}

Property& Property::operator=(const Property &other)
{
    m_pPrivate = other.m_pPrivate;
    m_sName = other.m_sName;
    m_pPrivate->registerUse();
    return *this;
}

QString Property::name() const
{
    return m_sName;
}

/*bool Property::scales()
{
    return m_bScale;
}*/

ScaleMode Property::scaleMode() const
{
    return m_pPrivate->m_eScale;
}

ValueType Property::valueType() const
{
    return m_eValueType;
}

bool Property::isNull() const
{
    return m_pPrivate->m_bNull;
}

bool Property::hasVariable(QString var) const
{
    QRegExp varreg("\\$([a-zA-Z_-0-9]+)");
    if (var.isNull())
    {
        if (varreg.indexIn(m_pPrivate->m_sValue) != -1)
            return true;
    }
    else
    {
        int offset = 0;
        int index;
        while ((index = varreg.indexIn(m_pPrivate->m_sValue,offset)) != -1)
        {
            if (varreg.cap(1) == var)
                return true;
            offset = index + varreg.cap(0).size();
        }
    }
    return false;
}

void Property::update(Stylesheet *css) const
{
    m_pPrivate->m_pCSS = css;
}

QString Property::toString(int part,bool scale) const
{
    if (m_pPrivate->m_eScale != smNone && scale)
    {
        /*QRegExp n("[0-9]+\\.*[0-9]*");
        int o = n.indexIn(value());
        double v = n.cap(0).toDouble();

        if (o == -1)
            return value();

        if (m_pPrivate->m_eScale == smScale)
        {
            if (m_pPrivate->m_bHeightProp)
                v *= m_pPrivate->m_pCSS->heightScaleFactor();
            else
                v *= m_pPrivate->m_pCSS->widthScaleFactor();
        }
        else if (m_pPrivate->m_eScale == smScaleWidth)
            v *= m_pPrivate->m_pCSS->widthScaleFactor();
        else if (m_pPrivate->m_eScale == smScaleHeight)
            v *= m_pPrivate->m_pCSS->heightScaleFactor();

        if (n.cap(0).indexOf(".") == -1)
        {
            return value().mid(0,o) + QString::number(qCeil(v)) + value().mid(o+n.cap(0).size());
        }

        return value().mid(0,o) + QString::number(v) + value().mid(o+n.cap(0).size());*/
        QRegExp n("[0-9]+\\.*[0-9]*");
        QString val = value();
        if (partCount() > 0)
        {
            QStringList parts = val.split(QRegExp(" +"));
        }
        else
        {
            int o = n.indexIn(val);
            double v = n.cap(0).toDouble();

            if (o == -1)
                return val;

            if (m_pPrivate->m_eScale == smScale)
            {
                if (m_pPrivate->m_bHeightProp)
                    v *= m_pPrivate->m_pCSS->heightScaleFactor();
                else
                    v *= m_pPrivate->m_pCSS->widthScaleFactor();
            }
            else if (m_pPrivate->m_eScale == smScaleWidth)
                v *= m_pPrivate->m_pCSS->widthScaleFactor();
            else if (m_pPrivate->m_eScale == smScaleHeight)
                v *= m_pPrivate->m_pCSS->heightScaleFactor();

            if (n.cap(0).indexOf(".") == -1)
            {
                return val.mid(0,o) + QString::number(qCeil(v)) + val.mid(o+n.cap(0).size());
            }

            return val.mid(0,o) + QString::number(v) + val.mid(o+n.cap(0).size());
        }

    }
    return value();
}

int Property::partCount() const
{
    return value().split(QRegExp(" +")).size();
}

QString Property::value() const
{
    if (m_pPrivate->m_bNull)
        return QString();
    QRegExp varreg("\\$([a-zA-Z_-0-9]+)");
    int offset;
    int i;
    QString cval = m_pPrivate->m_sValue;
    QString val = m_pPrivate->m_sValue;
    QString varval;

    if (!m_pPrivate->m_pCSS)
        return val;

    while (true)
    {
        offset = 0;
        while ((i=varreg.indexIn(cval,offset)) != -1)
        {
            varval = m_pPrivate->m_pCSS->variable(varreg.cap(1));
            if (varval != varreg.cap(1))
                val = val.replace(varreg.cap(0),varval);
            else
                val = val.replace(varreg.cap(0),"");//to prevent endless loop
            offset = i + varreg.cap(0).size();
        }

        if (varreg.indexIn(val) != -1)
        {
            cval = val;
        }
        else
            break;
    }

    return val;
}

int Property::toInt(int part,bool scale) const
{
    QString val = toString(scale);
    QRegExp nr("([0-9]+)");
    if (nr.indexIn(val) == -1)
        return 0;

    int i=nr.cap(1).toInt();

    if (val.left(1) == "-")
        i*=-1;
    return i;
}

bool Property::toBool(int part) const
{
    QString val = m_pPrivate->m_sValue;
    if (val == "true" || val == "1")
        return true;
    return false;
}

double Property::toDouble(int part,bool scale) const
{
    QString val = toString(scale);
    QRegExp nr("([0-9\\.]+)");
    if (nr.indexIn(val) == -1)
        return 0;
    return nr.cap(1).toDouble();
}

QColor Property::toColor() const
{
    return stringToColor(value());
}

void Property::setValue(double val, int part, ScaleMode scale) const
{
    if (m_pPrivate->m_bReadOnly)
        return;
    m_pPrivate->m_bNull = false;
    m_pPrivate->m_eScale = scale;

    QRegExp strfinder("([a-zA-Z%]+)$");

    if (strfinder.indexIn(m_pPrivate->m_sValue) != -1)
        m_pPrivate->m_sValue = QString::number(val) + strfinder.cap(1);
    else
        m_pPrivate->m_sValue = QString::number(val);
}

void Property::setValue(int val, int part, ScaleMode scale) const
{
    m_pPrivate->m_bNull = false;
    m_pPrivate->m_eScale = scale;

    QRegExp strfinder("([a-zA-Z%]+)$");

    if (strfinder.indexIn(m_pPrivate->m_sValue) != -1)
        m_pPrivate->m_sValue = QString::number(val) + strfinder.cap(1);
    else
    m_pPrivate->m_sValue = QString::number(val);
}

void Property::setValue(QString val, int part, ScaleMode scale) const
{
    m_pPrivate->m_bNull = false;
    m_pPrivate->m_eScale = scale;
    m_pPrivate->m_sValue = val;
}

void Property::setValue(QString val, int part) const
{
    m_pPrivate->m_bNull = false;
    m_pPrivate->m_sValue = val;
}

void Property::setValue(QColor val, int part, ColorFormat format) const
{
    m_pPrivate->m_bNull = false;
    m_pPrivate->m_eScale = smNone;
    m_pPrivate->m_sValue = colorToString(val,format);
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
    Property prop;

    int f;
    bool /*bScale,*/bHeightProp;
    ScaleMode scale;

    QRegExp outerspaces("(^ +| +$)");

    QRegExp propgroupfinder("([^\\{]+)\\{");
    QRegExp idgroupfinder("#([a-zA-Z_-0-9]+) *\\{");
    QRegExp varfinder("\\$([a-zA-Z_-0-9]+) *= *([^;]+);");
    QRegExp atrulesfinder("@([a-zA-Z_-0-9]+) *([^;]+) *;");
    QRegExp atrulesblockfinder("@([a-zA-Z_-0-9]+) *([^\\{]+) *\\{");

    QRegExp animkeyframefinder("([0-9]+)[% ]*\\{([^\\}]+)\\}");

    int offset = 0;
    int index = css.indexOf(atrulesfinder,offset);
    int ao;

    while (index != -1)
    {
        if (atrulesfinder.cap(1).toLower() == "import")
        {
            QString fname = atrulesfinder.cap(2);
            QByteArray data = m_pDocument->resource(fname);
            css.replace(atrulesfinder.cap(0),QString::fromUtf8(data));
        }

        offset = index + atrulesfinder.cap(0).size();
        index = css.indexOf(atrulesfinder,offset);
    }

    offset = 0;
    int fc;
    int ei;
    int ai;
    QString atcss = css;

    //ai = atcss.indexOf(atrulesblockfinder,offset);

    while ((ai = atcss.indexOf(atrulesblockfinder,offset)) != -1)
    {
        ei = 0;

        fc=1;
        for (ei=ai+atrulesblockfinder.cap(0).size();ei<atcss.size();ei++)
        {
            if (atcss[ei] == '{')
                fc++;
            else if (atcss[ei] == '}')
                fc--;
            if (fc==0)
                break;
        }
        QString block = atcss.mid(ai+atrulesblockfinder.cap(0).size(),ei - (ai+atrulesblockfinder.cap(0).size()));
        offset = ei+1;
        if (atrulesblockfinder.cap(1).toLower() == "keyframes")
        {
            //animation
            ao = 0;
            QString animname = atrulesblockfinder.cap(2);
            animname = animname.replace(outerspaces,"");
            Animation* ani =  new Animation(animname);
            int ki;
            KeyFrame* keyf;

            while (animkeyframefinder.indexIn(block,ao) != -1)
            {
                ki = animkeyframefinder.cap(1).replace(QRegExp("[^0-9]+"),"").toInt();
                proplist = animkeyframefinder.cap(2).split(";");
                keyf = new KeyFrame();
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

                        keyf->addProperty(propkey,Property(propkey,propvalue,this,scale,bHeightProp));
                    }
                }
                ani->addKeyFrame(keyf,ki);
                ao += animkeyframefinder.cap(0).size();
            }

            m_animations.insert(animname,ani);
        }



        //ai = atcss.indexOf(atrulesblockfinder,offset);
    }

    offset = 0;

    index = css.indexOf(varfinder,offset);
    while (index != -1)
    {
        ss = varfinder.cap(2).replace(outerspaces,"");
        ss.replace("\"","");
        setVariable(varfinder.cap(1),ss,true);
        offset = index + varfinder.cap(0).size();
        index = css.indexOf(varfinder,offset);
        //qDebug() << "css var added" << varfinder.cap(1) << ss;
    }

    m_prevariables = m_variables;


    //group parsing
    // #section { #object {} } to #section::object {}

    css = parseGroup(css);

    offset = 0;

    //index = css.indexOf(propgroupfinder,offset);
    while ((index = css.indexOf(propgroupfinder,offset)) != -1)
    {
        ss = propgroupfinder.cap(1).replace(outerspaces,"");
        //propdata = propgroupfinder.cap(2);
        //qDebug()() << "found propgrpup" << ss;

        fc=1;
        for (ei=index+propgroupfinder.cap(0).size();ei<css.size();ei++)
        {
            if (css[ei] == '{')
                fc++;
            else if (css[ei] == '}')
                fc--;
            if (fc==0)
                break;
        }

        propdata = css.mid(index+propgroupfinder.cap(0).size(),ei - (index + propgroupfinder.cap(0).size()));
        offset = ei+1;

        //qDebug()() << propdata;

        if (ss.indexOf(";") != -1)
            ss = ss.section(';',-1);

        if (ss.left(1) != "@")
        {


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

                    //qDebug()() << "prop found: " << propkey << propvalue;

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
                            prop = Property("margin-top",marginprops[0],this,scale,true);
                            s->setProperty("margin-top",prop);

                            prop = Property("margin-right",marginprops[1],this,scale,false);
                            s->setProperty("margin-right",prop);

                            prop = Property("margin-bottom",marginprops[2],this,scale,true);
                            s->setProperty("margin-bottom",prop);

                            prop = Property("margin-left",marginprops[3],this,scale,false);
                            s->setProperty("margin-left",prop);
                        }
                        else if (marginprops.size() == 3)
                        {
                            prop = Property("margin-top",marginprops[0],this,scale,true);
                            s->setProperty("margin-top",prop);

                            prop = Property("margin-right",marginprops[1],this,scale,false);
                            s->setProperty("margin-right",prop);

                            prop = Property("margin-bottom",marginprops[2],this,scale,true);
                            s->setProperty("margin-bottom",prop);

                            prop = Property("margin-left",marginprops[1],this,scale,false);
                            s->setProperty("margin-left",prop);
                        }
                        else if (marginprops.size() == 2)
                        {
                            prop = Property("margin-top",marginprops[0],this,scale,true);
                            s->setProperty("margin-top",prop);

                            prop = Property("margin-right",marginprops[1],this,scale,false);
                            s->setProperty("margin-right",prop);

                            prop = Property("margin-bottom",marginprops[0],this,scale,true);
                            s->setProperty("margin-bottom",prop);

                            prop = Property("margin-left",marginprops[1],this,scale,false);
                            s->setProperty("margin-left",prop);
                        }
                        else
                        {
                            prop = Property("margin-top",propvalue,this,scale,true);
                            s->setProperty("margin-top",prop);

                            prop = Property("margin-right",propvalue,this,scale,false);
                            s->setProperty("margin-right",prop);

                            prop = Property("margin-bottom",propvalue,this,scale,true);
                            s->setProperty("margin-bottom",prop);

                            prop = Property("margin-left",propvalue,this,scale,false);
                            s->setProperty("margin-left",prop);
                        }
                    }
                    else if (propkey == "padding")
                    {
                        QStringList paddingprops = propvalue.split(QRegExp(" +"));
                        if (paddingprops.size() == 4)
                        {
                            prop = Property("padding-top",paddingprops[0],this,scale,true);
                            s->setProperty("padding-top",prop);

                            prop = Property("padding-right",paddingprops[1],this,scale,false);
                            s->setProperty("padding-right",prop);

                            prop = Property("padding-bottom",paddingprops[2],this,scale,true);
                            s->setProperty("padding-bottom",prop);

                            prop = Property("padding-left",paddingprops[3],this,scale,false);
                            s->setProperty("padding-left",prop);
                        }
                        else if (paddingprops.size() == 3)
                        {
                            prop = Property("padding-top",paddingprops[0],this,scale,true);
                            s->setProperty("padding-top",prop);

                            prop = Property("padding-right",paddingprops[1],this,scale,false);
                            s->setProperty("padding-right",prop);

                            prop = Property("padding-bottom",paddingprops[2],this,scale,true);
                            s->setProperty("padding-bottom",prop);

                            prop = Property("padding-left",paddingprops[1],this,scale,false);
                            s->setProperty("padding-left",prop);
                        }
                        else if (paddingprops.size() == 2)
                        {
                            prop = Property("padding-top",paddingprops[0],this,scale,true);
                            s->setProperty("padding-top",prop);

                            prop = Property("padding-right",paddingprops[1],this,scale,false);
                            s->setProperty("padding-right",prop);

                            prop = Property("padding-bottom",paddingprops[0],this,scale,true);
                            s->setProperty("padding-bottom",prop);

                            prop = Property("padding-left",paddingprops[1],this,scale,false);
                            s->setProperty("padding-left",prop);
                        }
                        else
                        {
                            prop = Property("padding-top",propvalue,this,scale,true);
                            s->setProperty("padding-top",prop);

                            prop = Property("padding-right",propvalue,this,scale,false);
                            s->setProperty("padding-right",prop);

                            prop = Property("padding-bottom",propvalue,this,scale,true);
                            s->setProperty("padding-bottom",prop);

                            prop = Property("padding-left",propvalue,this,scale,false);
                            s->setProperty("padding-left",prop);
                        }
                    }
                    else if (propkey == "border")
                    {
                        prop = Property("border-top",propvalue,this,scale,false);
                        s->setProperty("border-top",prop);

                        prop = Property("border-right",propvalue,this,scale,false);
                        s->setProperty("border-right",prop);

                        prop = Property("border-bottom",propvalue,this,scale,false);
                        s->setProperty("border-bottom",prop);

                        prop = Property("border-left",propvalue,this,scale,false);
                        s->setProperty("border-left",prop);
                    }
                    else if (propkey == "animation")
                    {
                        QStringList animprops = propvalue.split(QRegExp(" +"));

                        if (animprops.size() >= 1)
                        {
                            prop = Property("animation-name",animprops[0],this,scale,false);
                            s->setProperty("animation-name",prop);

                            if (animprops.size() >= 2)
                                prop = Property("animation-duration",stringToMsTimeString(animprops[1]),this,scale,false);
                            else
                                prop = Property("animation-duration","1000ms",this,scale,false);
                            s->setProperty("animation-duration",prop);

                            if (animprops.size() >= 3)
                                prop = Property("animation-timing-function",animprops[2],this,scale,false);
                            else
                                prop = Property("animation-timing-function","ease",this,scale,false);
                            s->setProperty("animation-timing-function",prop);

                            if (animprops.size() >= 4)
                                prop = Property("animation-delay",animprops[3],this,scale,false);
                            else
                                prop = Property("animation-delay","0ms",this,scale,false);
                            s->setProperty("animation-delay",prop);

                            if (animprops.size() >= 5)
                                prop = Property("animation-iteration-count",animprops[4],this,scale,false);
                            else
                                prop = Property("animation-iteration-count","1",this,scale,false);
                            s->setProperty("animation-iteration-count",prop);

                            if (animprops.size() >= 6)
                                prop = Property("animation-direction",animprops[5],this,scale,false);
                            else
                                prop = Property("animation-direction","normal",this,scale,false);
                            s->setProperty("animation-direction",prop);
                        }
                    }
                    else
                    {
                        prop = Property(propkey,propvalue,this,scale,bHeightProp);
                        s->setProperty(propkey,prop);
                    }
                }
            }
        }

        //index = css.indexOf(propgroupfinder,offset);
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
        //qDebug()() << "propagating section" << cs->id();

        s = selector("overlay");

        //propagate from base section props to overlay
        base = selector("section");
        baseprops = base->properties();

        for (int n=0;n<baseprops.size();n++)
        {
            if (s->property(baseprops[n]).isNull())
                s->property(baseprops[n]).setValue(base->property(baseprops[n]).toString());
        }

        //propagate from base overlay props for overlay specific props
        s = selector("#"+o->id());
        base = selector("section");
        baseprops = base->properties();

        for (int n=0;n<baseprops.size();n++)
        {
            if (s->property(baseprops[n]).isNull())
                s->property(baseprops[n]).setValue(base->property(baseprops[n]).toString());
        }


        for (int n=0;n<o->layerCount();n++)
        {
            l=o->layer(n);
            //qDebug()() << "propagating layer" << l->id();

            s = selector("#"+o->id()+" layer");

            //first propagate from the layer base
            base = selector("layer");
            baseprops = base->properties();

            for (int n=0;n<baseprops.size();n++)
            {
                if (s->property(baseprops[n]).isNull())
                    s->property(baseprops[n]).setValue(base->property(baseprops[n]).toString());
            }

            //propagate from layer base per section
            s = selector("#"+o->id()+":"+l->id());
            base = selector("#"+o->id()+" layer");
            baseprops = base->properties();

            for (int n=0;n<baseprops.size();n++)
            {
                if (s->property(baseprops[n]).isNull())
                    s->property(baseprops[n]).setValue(base->property(baseprops[n]).toString());
            }

            for (int j=0;j<l->objectCount();j++)
            {
                obj=l->object(j);
                //qDebug()() << "propagating object" << obj->id();

                // propagate the section specific object base
                s = selector("#"+o->id()+" object");
                base = selector("object");
                baseprops = base->properties();

                //qDebug()() << "propagating obj baseprops size" << baseprops.size();

                for (int n=0;n<baseprops.size();n++)
                {
                    //qDebug()() << "#"+o->id()+" object" << baseprops[n];
                    if (s->property(baseprops[n]).isNull() || !oriprops.contains(baseprops[n]))
                        s->property(baseprops[n]).setValue(base->property(baseprops[n]).toString());
                }

                s = selector("#"+o->id()+"::"+obj->id());
                oriprops = s->properties();

                // also propagate from section object
                base = selector("section object");
                baseprops = base->properties();

                for (int n=0;n<baseprops.size();n++)
                {
                    if (s->property(baseprops[n]).isNull())
                        s->property(baseprops[n]).setValue(base->property(baseprops[n]).toString());
                }

                // now propagate from the layer base
                base = selector("#"+o->id()+" layer");
                baseprops = base->properties();

                for (int n=0;n<baseprops.size();n++)
                {
                    if (s->property(baseprops[n]).isNull() || !oriprops.contains(baseprops[n]))
                        s->property(baseprops[n]).setValue(base->property(baseprops[n]).toString());
                }

                // and propagate from the section specific object base
                base = selector("#"+o->id()+" object");
                baseprops = base->properties();

                for (int n=0;n<baseprops.size();n++)
                {
                    //qDebug()() << "#"+o->id()+"::"+obj->id() << baseprops[n];
                    if (s->property(baseprops[n]).isNull() || !oriprops.contains(baseprops[n]))
                        s->property(baseprops[n]).setValue(base->property(baseprops[n]).toString());
                }

            }
        }
    }


    //sections
    CSection* cs;

    for (int i=0;i<m_pDocument->sectionCount();i++)
    {
        cs=m_pDocument->section(i);
        //qDebug()() << "propagating section" << cs->id();

        s = selector("section");

        //propagate from base section props to overlay
        base = selector("section");
        baseprops = base->properties();

        for (int n=0;n<baseprops.size();n++)
        {
            if (s->property(baseprops[n]).isNull())
                s->property(baseprops[n]).setValue(base->property(baseprops[n]).toString());
        }

        //propagate from base overlay props for overlay specific props
        s = selector("#"+cs->id());
        base = selector("section");
        baseprops = base->properties();

        for (int n=0;n<baseprops.size();n++)
        {
            if (s->property(baseprops[n]).isNull())
                s->property(baseprops[n]).setValue(base->property(baseprops[n]).toString());
        }


        for (int n=0;n<cs->layerCount();n++)
        {
            l=cs->layer(n);
            //qDebug()() << "propagating layer" << l->id();

            s = selector("#"+cs->id()+" layer");

            //first propagate from the layer base
            base = selector("layer");
            baseprops = base->properties();

            for (int n=0;n<baseprops.size();n++)
            {
                if (s->property(baseprops[n]).isNull())
                    s->property(baseprops[n]).setValue(base->property(baseprops[n]).toString());
            }

            //propagate from layer base per section
            s = selector("#"+cs->id()+":"+l->id());
            base = selector("#"+cs->id()+" layer");
            baseprops = base->properties();

            for (int n=0;n<baseprops.size();n++)
            {
                if (s->property(baseprops[n]).isNull())
                    s->property(baseprops[n]).setValue(base->property(baseprops[n]).toString());
            }

            for (int j=0;j<l->objectCount();j++)
            {
                obj=l->object(j);
                //qDebug()() << "propagating object" << obj->id();

                // propagate the section specific object base
                s = selector("#"+cs->id()+" object");
                base = selector("object");
                baseprops = base->properties();

                //qDebug()() << "propagating obj baseprops size" << baseprops.size();

                for (int n=0;n<baseprops.size();n++)
                {
                    //qDebug()() << "#"+cs->id()+" object" << baseprops[n];
                    if (s->property(baseprops[n]).isNull() || !oriprops.contains(baseprops[n]))
                        s->property(baseprops[n]).setValue(base->property(baseprops[n]).toString());
                }

                s = selector("#"+cs->id()+"::"+obj->id());
                oriprops = s->properties();

                // also propagate from section object
                base = selector("section object");
                baseprops = base->properties();

                for (int n=0;n<baseprops.size();n++)
                {
                    if (s->property(baseprops[n]).isNull())
                        s->property(baseprops[n]).setValue(base->property(baseprops[n]).toString());
                }

                // now propagate from the layer base
                base = selector("#"+cs->id()+" layer");
                baseprops = base->properties();

                for (int n=0;n<baseprops.size();n++)
                {
                    if (s->property(baseprops[n]).isNull() || !oriprops.contains(baseprops[n]))
                        s->property(baseprops[n]).setValue(base->property(baseprops[n]).toString());
                }

                // and propagate from the section specific object base
                base = selector("#"+cs->id()+" object");
                baseprops = base->properties();

                for (int n=0;n<baseprops.size();n++)
                {
                    //qDebug()() << "#"+cs->id()+"::"+obj->id() << baseprops[n];
                    if (s->property(baseprops[n]).isNull() || !oriprops.contains(baseprops[n]))
                        s->property(baseprops[n]).setValue(base->property(baseprops[n]).toString());
                }

            }
        }
    }

    //reverse
    /*
    QHash<QString,Selector*>::Iterator it;
    QString test;

    for (it=m_selectors.begin();it!=m_selectors.end();it++)
    {
        test += it.key();
        test += "\n{\n";
        s = it.value();
        QStringList list = s->properties();
        for (int i=0;i<list.size();i++)
            test += "\t"+list[i]+": "+s->property(list[i]).toString()+";\n";
        test += "}\n";
    }
    //qDebug() << test;*/
}

QString Stylesheet::parseGroup(QString gcss)
{
    /*QRegExp newlines("[\n\r]+");
    QRegExp tabs("[\t]+");
    css.replace(newlines,"");
    css.replace(tabs," ");*/

    QString ss;

    QRegExp outerspaces("(^ +| +$)");

    QRegExp propgroupfinder("([\\.#][^\\{]+)\\{([^}]+)\\}");
    QRegExp idgroupfinder("#([a-zA-Z_\\-0-9]+) *\\{");

    int offset = 0;
    int index;
    int fc;
    int ei;
    int ps;
    int pd;

    //group parsing
    // #section { #object {} } to #section::object {}

    QString g,gg,p;
    offset = 0;


    int gindex,goff=0;

    while ((index = gcss.indexOf(idgroupfinder,offset)) != -1)
    {
        ss = idgroupfinder.cap(1).replace(outerspaces,"");
        fc=1;
        for (ei=index+idgroupfinder.cap(0).size();ei<gcss.size();ei++)
        {
            if (gcss[ei] == '{')
                fc++;
            else if (gcss[ei] == '}')
                fc--;
            if (fc==0)
                break;
        }

        g = gcss.mid(index+idgroupfinder.cap(0).size(),ei - (index + idgroupfinder.cap(0).size()));
        //g = parseGroup(g);
        goff = 0;

        while ((gindex = g.indexOf(propgroupfinder,goff)) != -1)
        {
            gg = propgroupfinder.cap(1).replace(outerspaces,"");
            ps = gindex - goff;
            pd = 0;
            p = "";
            if (ps > 0)
            {
                //the group also has properties
                p = g.mid(goff,ps);
                p = "#" + ss + "{" + p + "}";
                pd = p.size() - ps;
            }

            if (gg[0] == '#')
            {
                gg = "#" + ss + "::" + gg.mid(1);
            }
            else if (gg[0] == '.')
                gg = "#" + ss + gg;


            g = g.mid(0,gindex-ps) + p + gg + "{" + propgroupfinder.cap(2) + "}" + g.mid(gindex + propgroupfinder.cap(0).size());
            goff = gindex + pd + gg.size() + propgroupfinder.cap(2).size() + 2;
        }
        if (goff > 0)
        {
            if (g.size() - 1 - goff > 0)
            {
                p = g.mid(goff);
                p = "#" + ss + "{" + p + "}";
                g = g.mid(0,goff) + p;
            }
            gcss = gcss.mid(0,index) + g + gcss.mid(ei+1);
        }
        offset = index + idgroupfinder.cap(0).size() + g.size();

    }

    return gcss;
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

    //qDebug()() << m_selectors.keys();

    return m_selectors[selector];
}

Selector* Stylesheet::selector(CBaseObject *obj)
{
    Selector* s = selector("#"+obj->section()->id()+":"+obj->layer()->id()+":"+obj->id());
    if (s->isEmpty())
        s = selector("#"+obj->section()->id()+"::"+obj->id());
    return s;
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
        else if (str == "none")
            return rmNone;
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

    ValueType valueTypeFromString(QString str)
    {
        QRegExp intreg("^[0-9]+[a-zA-Z ]*$");
        QRegExp dblreg("^[0-9\\.]+$");
        QRegExp clrreg("^(#[0-9a-fA-F]{3,6}|rgb *\\( *([0-9]+) *, *([0-9]+) *, *([0-9]+) *\\)|rgba *\\( *([0-9]+) *, *([0-9]+) *, *([0-9]+) *, *([0-9]+) *\\))$");

        if (intreg.indexIn(str) != -1)
            return vtInt;
        if (dblreg.indexIn(str) != -1)
            return vtDouble;
        if (clrreg.indexIn(str) != -1)
            return vtColor;

        return vtUndefined;
    }

    QString stringToMsTimeString(QString timestr)
    {
        QRegExp timereg("([0-9\\.]+) *(s|ms)");
        QString s;
        if (timereg.indexIn(timestr) != -1)
        {
            if (timereg.cap(2) == "s")
                s = QString::number(timereg.cap(1).toDouble() * 1000) + "ms";
            else
                s = timestr;
        }
        else
            s = timestr;
        return s;
    }

    int stringToMsTime(QString timestr)
    {
        QRegExp timereg("([0-9\\.]+) *(s|ms)");
        double i;
        if (timereg.indexIn(timestr) != -1)
        {
            i = timereg.cap(1).toDouble();
            if (timereg.cap(2) == "s")
                i *= 1000;
        }
        else
            i = 0;

        //qDebug()() << "stringToMsTime" << timestr << i;

        return (int)i;
    }
}

PropertyPrivate::PropertyPrivate()
{
    m_iRefCount = 1;
    //qDebug()() << "proppriv c" << this << m_iRefCount << m_sValue;
}

void PropertyPrivate::registerUse()
{
    m_iRefCount++;
    //qDebug()() << "proppriv r" << this << m_iRefCount << m_sValue;
}

void PropertyPrivate::unregisterUse()
{
    m_iRefCount--;
    //qDebug()() << "proppriv u" << this << m_iRefCount << m_sValue;
}

bool PropertyPrivate::isUnreferenced()
{
    //qDebug()() << "proppriv isu" << this << m_iRefCount << m_sValue;
    if (m_iRefCount == 0)
        return true;
    return false;
}

Animation* Stylesheet::animation(QString animation)
{
    if (m_animations.contains(animation))
        return m_animations[animation];
    return 0;
}

bool Selector::isEmpty()
{
    return m_props.empty();
}

void Stylesheet::oldState(bool b)
{
    m_bOldState = b;
}
