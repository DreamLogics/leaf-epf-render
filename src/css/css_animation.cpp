#include "css_animation.h"
#include <qmath.h>

using namespace CSS;

Animation::Animation()
{
}

Animation::~Animation()
{
    QMap<int,KeyFrame*>::Iterator it;
    for (it = m_keyframes.begin();it != m_keyframes.end();it++)
        delete it.value();
}

Property Animation::keyedProperty(QString property, double position)
{
    int startframe = qFloor(position);
    int endframe = qCeil(position);

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

    return newprop;
}

void Animation::addKeyFrame(KeyFrame* kf, int key)
{
    m_keyframes.insert(key,kf);
}

void Animation::generateFrames(KeyFrame* startframe)
{
    KeyFrame* kf;
    KeyFrame* lf;
    KeyFrame* nf;
    double pos;
    QStringList props;
    int nextframe=0;
    int lastrealframe=0;
    for (int i=0;i<100;i++)
    {
        if (i == 0 && !m_keyframes.contains(0))
            m_keyframes.insert(0,startframe->clone());
        else if (!m_keyframes.contains(i))
        {
            if (i > nextframe && nextframe != -1)
            {
                lastrealframe = nextframe;
                for (int n=i;n<100;n++)
                {
                    if (m_keyframes.contains(n))
                        nextframe = n;
                }
                if (nextframe == lastrealframe)
                    nextframe = -1;
            }

            if (nextframe == -1)
                m_keyframes.insert(i-1,m_keyframes[i-1]->clone());
            else
            {
                lf = m_keyframes[lastrealframe];
                nf = m_keyframes[nextframe];
                kf = new KeyFrame();
                props = lf->properties();
                props += nf->properties();
                props.removeDuplicates();
                pos = (i - lastrealframe) / (nextframe - lastrealframe);
                for (int a=0;a<props.size();a++)
                {
                    Property propstart = lf->property(props[a]);
                    Property propend = nf->property(props[a]);

                    if (propend.isNull())
                    {
                        kf->addProperty(props[a],propstart);
                        break;
                    }

                    ValueType vts = valueTypeFromString(propstart.toString());
                    ValueType vte = valueTypeFromString(propend.toString());

                    if (vts != vte)
                    {
                        kf->addProperty(props[a],propstart);
                        break;
                    }

                    Property newprop = propstart.clone();

                    switch (vts)
                    {
                        case vtInt:
                            newprop.setValue(propstart.toInt(false) * (1-pos) + propend.toInt(false) * pos,propstart.scaleMode());
                            break;
                        case vtDouble:
                            newprop.setValue(propstart.toDouble(false) * (1-pos) + propend.toDouble(false) * pos,propstart.scaleMode());
                            break;
                        case vtColor:
                            QColor c1 = propstart.toColor();
                            QColor c2 = propend.toColor();
                            QColor c3(c1.red() * (1-pos) + c2.red() * pos,c1.green() * (1-pos) + c2.green() * pos,
                                      c1.blue() * (1-pos) + c2.blue() * pos,c1.alpha() * (1-pos) + c2.alpha() * pos);
                            newprop.setValue(c3,cfRgba);
                            break;
                     }
                     kf->addProperty(props[a],newprop);

                }
            }
        }
    }
}

KeyFrame::~KeyFrame()
{

}

Property KeyFrame::property(QString prop)
{
    return Property();
}

void KeyFrame::addProperty(QString key, Property value)
{
    m_properties.insert(key,value);
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
