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
    {
        if (m_keyframes.contains(startframe))
            return m_keyframes[startframe]->property(property);
    }



    return Property();
}

void Animation::addKeyFrame(KeyFrame* kf, int key)
{
    m_keyframes.insert(key,kf);
}

void Animation::generateFrames(KeyFrame* startframe)
{
    KeyFrame* kf;
    for (int i=0;i<100;i++)
    {

    }
}

KeyFrame::~KeyFrame()
{

}

Property KeyFrame::property(QString prop)
{
    return Property();
}

void KeyFrame::addProperty(QString key, QString value)
{

}
