#ifndef CSS_ANIMATION_H
#define CSS_ANIMATION_H

#include <QMap>
#include "css_style.h"

namespace CSS
{

class KeyFrame
{
public:
    ~KeyFrame();
    Property property(QString prop);

private:
    void addProperty(QString key, QString value);

    QMap<QString, Property> m_properties;
    friend class Stylesheet;
};

class Animation
{
public:
    Animation();
    ~Animation();

    Property keyedProperty(QString property, double position);

private:
    void addKeyFrame(KeyFrame* kf, int key);
    void generateFrames(KeyFrame* startframe);

    QMap<int,KeyFrame*> m_keyframes;
    friend class Stylesheet;
};

}

#endif // CSS_ANIMATION_H
