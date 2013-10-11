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

#ifndef CSS_ANIMATION_H
#define CSS_ANIMATION_H

#include <QMap>
#include "css_style.h"

namespace CSS
{

enum easing_function
{
    efNone=0,
    efEase,
    efEaseIn,
    efEaseOut
};

enum direction
{
    dirNormal=0,
    dirReverse,
    dirAlternate,
    dirAlternateReverse

};

class KeyFrame
{
public:
    ~KeyFrame();
    Property property(QString prop);

    KeyFrame* clone();
    QStringList properties();

    void addProperty(QString key, Property value);
    void removeProperty(QString key);

private:

    QMap<QString, Property> m_properties;
};

class Animation
{
public:
    Animation(QString name);
    ~Animation();

    QString name() const;
    Property keyedProperty(QString property, double position);
    void generateFrames(KeyFrame* startframe);
    QStringList properties();
    void removePropertiesFromKeyFrames(QStringList props);

private:
    void addKeyFrame(KeyFrame* kf, int key);
    KeyFrame* keyFrame(int key);

    QMap<int,KeyFrame*> m_keyframes;
    QMap<int,KeyFrame*> m_keyframes_def;
    QString m_sName;
    friend class Stylesheet;
    friend class Transitioner;
};

}

#endif // CSS_ANIMATION_H
