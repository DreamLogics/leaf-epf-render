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

#include "css_animation.h"
#include <qmath.h>
#include <QDebug>
#include <QColor>

using namespace CSS;

Animation::Animation(QString name) : m_sName(name)
{
}

Animation::~Animation()
{
    QMap<int,KeyFrame*>::Iterator it;
    for (it = m_keyframes.begin();it != m_keyframes.end();it++)
        delete it.value();
}

QString Animation::name() const
{
    return m_sName;
}

Property Animation::keyedProperty(QString property, double position)
{
    double percent = position*100;
    int startframe = qFloor(percent);
    int endframe = qCeil(percent);

    //qDebug() << "creating keyed prop" << property << position << startframe << endframe;

    if (startframe == endframe)
        return m_keyframes[startframe]->property(property);

    //interpolate
    KeyFrame* start = m_keyframes[startframe];
    KeyFrame* end = m_keyframes[endframe];

    Property propstart = start->property(property);
    Property propend = end->property(property);
    Property newprop = propstart.clone();

    ValueType vts = valueTypeFromString(propstart.toString());
    ValueType vte = valueTypeFromString(propend.toString());

    if (vts != vte)
        return propstart;

    switch (vts)
    {
    case vtInt:
        newprop.setValue(propstart.toInt(false) * (1-position) + propend.toInt(false) * position,propstart.scaleMode());
        break;
    case vtDouble:
        newprop.setValue(propstart.toDouble(false) * (1-position) + propend.toDouble(false) * position,propstart.scaleMode());
        break;
    case vtColor:
        QColor c1 = propstart.toColor();
        QColor c2 = propend.toColor();
        QColor c3(c1.red() * (1-position) + c2.red() * position,c1.green() * (1-position) + c2.green() * position,
                  c1.blue() * (1-position) + c2.blue() * position,c1.alpha() * (1-position) + c2.alpha() * position);
        newprop.setValue(c3,cfRgba);
        break;
    }

    //qDebug() << propstart.toString() << propend.toString() << newprop.toString();

    return newprop;
}

void Animation::addKeyFrame(KeyFrame* kf, int key)
{
    m_keyframes_def.insert(key,kf);
}

QStringList Animation::properties()
{
    QMap<int,KeyFrame*>::Iterator it;
    QStringList props;
    KeyFrame* kf;

    for (it=m_keyframes_def.begin();it != m_keyframes_def.end();it++)
    {
        kf = it.value();
        props += kf->properties();
    }

    props.removeDuplicates();

    return props;
}

void Animation::generateFrames(KeyFrame* startframe)
{
    m_keyframes = m_keyframes_def;
    KeyFrame* kf;
    KeyFrame* lf;
    KeyFrame* nf;
    double pos;
    QStringList props;
    int nextframe=0;
    int lastrealframe=0;
    for (int i=0;i<=100;i++)
    {
        if (i == 0 && !m_keyframes.contains(0))
            m_keyframes.insert(0,startframe->clone());
        else if (!m_keyframes.contains(i))
        {
            if (i > nextframe && nextframe != -1)
            {
                lastrealframe = nextframe;
                for (int n=i;n<=100;n++)
                {
                    if (m_keyframes.contains(n))
                        nextframe = n;
                }
                if (nextframe == lastrealframe)
                    nextframe = -1;
            }

            if (nextframe == -1)
                m_keyframes.insert(i,m_keyframes[i-1]->clone());
            else
            {
                lf = m_keyframes[lastrealframe];
                nf = m_keyframes[nextframe];
                kf = new KeyFrame();
                props = lf->properties();
                props += nf->properties();
                props.removeDuplicates();

                pos = (double)(i - lastrealframe) / (double)(nextframe - lastrealframe);
                for (int a=0;a<props.size();a++)
                {
                    Property propstart = lf->property(props[a]);
                    Property propend = nf->property(props[a]);

                    //qDebug() << "generating keyframe" << props[a] << propstart.toString() << propend.toString() << pos;

                    if (propend.isNull())
                    {
                        kf->addProperty(props[a],propstart);
                        qDebug() << "empty value";
                        continue;
                    }

                    ValueType vts = valueTypeFromString(propstart.toString());
                    ValueType vte = valueTypeFromString(propend.toString());

                    if (vts != vte)
                    {
                        kf->addProperty(props[a],propstart);
                        qDebug() << "value type mismatch";
                        continue;
                    }

                    Property newprop = propstart.clone();
                    QColor c1,c2,c3;

                    if (vts == vtUndefined)
                    {
                        //check if we are a mixed prop
                        //qDebug() << "undefined mixprop" << propstart.name() << propstart.value() << propend.value();
                        QStringList mixedstart = propstart.value().split(QRegExp("[ ]+"));
                        QStringList mixedend = propend.value().split(QRegExp("[ ]+"));
                        QString newpropval = "";


                        if (mixedstart.size() == mixedend.size())
                        {
                            for (int t=0;t<mixedstart.size();t++)
                            {
                                vts = valueTypeFromString(mixedstart[t]);
                                vte = valueTypeFromString(mixedend[t]);

                                if (vts != vte)
                                    break;

                                //qDebug() << "in da mix" << mixedstart[t] << vts;

                                switch (vts)
                                {
                                    case vtInt:
                                        newpropval += QString::number(mixedstart[t].toInt() * (1-pos) + mixedend[t].toInt() * pos) + " ";
                                        break;
                                    case vtDouble:
                                        newpropval += QString::number(mixedstart[t].toDouble() * (1-pos) + mixedend[t].toDouble() * pos) + " ";
                                        break;
                                    case vtColor:
                                        c1 = stringToColor(mixedstart[t]);
                                        c2 = stringToColor(mixedend[t]);
                                        c3 = QColor(c1.red() * (1-pos) + c2.red() * pos,c1.green() * (1-pos) + c2.green() * pos,
                                                  c1.blue() * (1-pos) + c2.blue() * pos,c1.alpha() * (1-pos) + c2.alpha() * pos);
                                        newpropval += colorToString(c3,cfRgba) + " ";
                                        break;
                                    case vtString:
                                    case vtUndefined:
                                        newpropval += mixedend[t] + " ";
                                        break;
                                 }

                            }

                            newpropval = newpropval.left(newpropval.size()-1);
                            //qDebug() << "new val" << newpropval;
                            newprop.setValue(newpropval,propstart.scaleMode());
                        }
                    }
                    else
                    {
                        switch (vts)
                        {
                            case vtInt:
                                newprop.setValue(propstart.toInt(false) * (1-pos) + propend.toInt(false) * pos,propstart.scaleMode());
                                break;
                            case vtDouble:
                                newprop.setValue(propstart.toDouble(false) * (1-pos) + propend.toDouble(false) * pos,propstart.scaleMode());
                                break;
                            case vtColor:
                                c1 = propstart.toColor();
                                c2 = propend.toColor();
                                c3 = QColor(c1.red() * (1-pos) + c2.red() * pos,c1.green() * (1-pos) + c2.green() * pos,
                                          c1.blue() * (1-pos) + c2.blue() * pos,c1.alpha() * (1-pos) + c2.alpha() * pos);
                                newprop.setValue(c3,cfRgba);
                                break;
                         }
                    }
                    kf->addProperty(props[a],newprop);

                }
                m_keyframes.insert(i,kf);
            }
        }
    }
}

void Animation::removePropertiesFromKeyFrames(QStringList props)
{
    QMap<int,KeyFrame*>::Iterator it;
    for (it=m_keyframes.begin();it!=m_keyframes.end();it++)
    {
        for (int i=0;i<props.size();i++)
            it.value()->removeProperty(props[i]);
    }
}

KeyFrame* Animation::keyFrame(int key)
{
    if (!m_keyframes_def.contains(key))
        return 0;
    return m_keyframes_def[key];
}

KeyFrame::~KeyFrame()
{

}

Property KeyFrame::property(QString prop)
{
    if (m_properties.contains(prop))
        return m_properties[prop];
    return Property();
}

void KeyFrame::addProperty(QString key, Property value)
{
    m_properties.insert(key,value);
}

void KeyFrame::removeProperty(QString key)
{
    m_properties.remove(key);
}

QStringList KeyFrame::properties()
{
    return m_properties.keys();
}

KeyFrame* KeyFrame::clone()
{
    KeyFrame* clone = new KeyFrame();
    clone->m_properties = m_properties;
    return clone;
}
