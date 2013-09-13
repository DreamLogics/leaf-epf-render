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

    KeyFrame* clone();
    QStringList properties();

    void addProperty(QString key, Property value);

private:

    QMap<QString, Property> m_properties;
};

class Animation
{
public:
    Animation();
    ~Animation();

    Property keyedProperty(QString property, double position);
    void generateFrames(KeyFrame* startframe);
    QStringList properties();

private:
    void addKeyFrame(KeyFrame* kf, int key);

    QMap<int,KeyFrame*> m_keyframes;
    QMap<int,KeyFrame*> m_keyframes_def;
    friend class Stylesheet;
};

}

#endif // CSS_ANIMATION_H
