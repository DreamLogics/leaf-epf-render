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

#include "coverlay.h"
#include "css/css_style.h"
#include "cdocument.h"
#include <QDebug>

COverlay::COverlay(QString id,CDocument* doc,bool visible) : CSection(id,doc,true), m_bVisible(visible)
{
    m_bActive = false;
}

COverlay::~COverlay()
{

}

bool COverlay::isVisible()
{
    return m_bVisible;
}

void COverlay::setVisibility(bool b)
{
    m_bVisible = b;
    document()->updateRenderView();
}

void COverlay::onEPFEvent(EPFEvent *ev)
{
    //qDebug() << "overlay event" << ev->event() << ev->parameter(0);
    if (ev->event() == "setActive")
    {
        if (ev->parameter(0) == "true" || ev->parameter(0) == "1")
            setActive(true);
        else
            setActive(false);
    }
    else if (ev->event() == "setVisible")
    {
        if (ev->parameter(0) == "true" || ev->parameter(0) == "1")
            setVisibility(true);
        else
            setVisibility(false);
    }
}

void COverlay::setActive(bool bActive)
{
    m_bActive = bActive;
}

bool COverlay::active()
{
    return m_bActive;
}
