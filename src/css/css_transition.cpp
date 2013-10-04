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

#include "css_transition.h"
#include "../canimator.h"
#include "../cbaseobject.h"
#include "../cdocument.h"

using namespace CSS;

Transitioner* Transitioner::get(QThread* th)
{
    static QMap<QThread*,Transitioner*> pmap;
    if (pmap.contains(th))
        return pmap[th];
    Transitioner* p = new Transitioner();
    pmap.insert(th,p);
    return p;
}

void Transitioner::createTransition(QString identifier, CBaseObject *obj, QList<Property> deltaprops, QStringList transitionable, easing_function easing, int ms_time, int ms_delay)
{
    if (m_Transitions.contains(identifier))
    {
        //already got one
        //re-reverse
        CAnimator::get(obj->thread())->reverseAnimation(m_Transitions[identifier].m_iAnimation,obj);
        return;
    }

    if (transitionable.size() == 0)
        transitionable.append("all");

    transition t;
    t.m_pObj = obj;
    t.m_pAnimation = new Animation("");

    Stylesheet* css = obj->document()->stylesheet();

    Property startprop;
    KeyFrame* kfs = new KeyFrame();
    KeyFrame* kfe = new KeyFrame();
    for (int i=0;i<deltaprops.size();i++)
    {
        if (transitionable[0] == "all" || transitionable.contains(deltaprops[i].name()))
        {
            startprop = css->property(obj,deltaprops[i].name());
            kfs->addProperty(deltaprops[i].name(),startprop);
        }
        else
            kfs->addProperty(deltaprops[i].name(),deltaprops[i]);
        kfe->addProperty(deltaprops[i].name(),deltaprops[i]);
    }

    t.m_pAnimation->addKeyFrame(kfs,0);
    t.m_pAnimation->addKeyFrame(kfe,100);

    t.m_pAnimation->generateFrames(0);

    t.m_iAnimation = CAnimator::get(obj->thread())->registerAnimation(obj,t.m_pAnimation,ms_time,easing,ms_delay,1,dirAlternate,true,-1);

    m_Transitions.insert(identifier,t);
}

void Transitioner::undoTransition(QString identifier)
{
    if (m_Transitions.contains(identifier))
    {
        //reverse
        CAnimator::get(m_Transitions[identifier].m_pObj->thread())->reverseAnimation(m_Transitions[identifier].m_iAnimation,m_Transitions[identifier].m_pObj);
    }
}

void Transitioner::transitionAnimDone(int animid)
{
    QMap<QString,transition>::iterator it;

    for (it=m_Transitions.begin();it!=m_Transitions.end();it++)
    {
        transition t = it.value();
        if (t.m_iAnimation == animid)
        {
            m_Transitions.remove(it.key());
            return;
        }
    }
}
