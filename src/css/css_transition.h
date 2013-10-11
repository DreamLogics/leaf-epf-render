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

#ifndef CSS_TRANSITION_H
#define CSS_TRANSITION_H

#include "css_animation.h"
#include <QMap>
#include <QThread>
#include <QStringList>

namespace CSS
{
    class Transitioner
    {
    public:

        struct transition
        {
            Animation* m_pAnimation;
            CBaseObject* m_pObj;
            int m_iAnimation;
        };

        static Transitioner* get(QThread* th);

        void createTransition(QString identifier,CBaseObject* obj, QList<Property> deltaprops, QStringList transitionable, easing_function easing, int ms_time, int ms_delay);
        bool undoTransition(QString identifier);
        void removeTransitions(CBaseObject* obj);
        void removeTransitioningProps(CBaseObject* obj, QStringList props);
        void transitionAnimDone(int animid);

    private:
        QMap<QString,transition> m_Transitions;
    };

}
#endif // CSS_TRANSITION_H
