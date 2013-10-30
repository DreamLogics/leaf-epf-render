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

#ifndef CANIMATOR_H
#define CANIMATOR_H

#include <QObject>
#include <QMutex>
#include <QMap>
#include "css/css_animation.h"

namespace CSS {
    class Stylesheet;
}

class CBaseObject;
class QTimer;
class CSection;
class QThread;

class CAnimator : public QObject
{
    Q_OBJECT
public:
    explicit CAnimator(QObject *parent = 0);

    static CAnimator* get(QThread*);

    struct registered_animation
    {
        CBaseObject* m_pObject;
        QString m_sAnimation;
        int m_iTime;
        int m_iDelay;
        int m_iIterations;
        CSS::easing_function m_efEasing;
        CSS::direction m_dirDirection;

        CSS::Animation* m_pAnim;

        int m_iStartTime;
        int m_iFrames;
        int m_iCurFrame;
        bool m_bAlternate;
        bool m_bFinished;
        bool m_bTransition;
        bool m_bStarted;
    };

    int registerAnimation(CBaseObject* obj, QString animation, int ms_time, CSS::easing_function ef, int ms_delay, int iterations, CSS::direction direction, int current_anim=-1);
    int registerAnimation(CBaseObject *obj, CSS::Animation* animation, int ms_time, CSS::easing_function ef, int ms_delay, int iterations, CSS::direction direction, bool bTransition, int current_anim);
    void unregisterAnimation(int ranim, CBaseObject* obj=0);
    bool isRegistered(int ranim, CBaseObject* obj=0);

    void reverseAnimation(int ranim, CBaseObject* obj=0);

    void stylesheetChange(CSS::Stylesheet* css);

    void setSection(CSection* section);
    
signals:
    
public slots:

    void update();
    
private:

    QMap<int,registered_animation> m_Animations;
    QTimer* m_pTimer;
    unsigned int m_iTime;
    CSection* m_pSection;
};

#endif // CANIMATOR_H
