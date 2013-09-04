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

#ifndef COVERLAY_H
#define COVERLAY_H

#include "csection.h"
#include <QList>
#include <QMap>

class COverlay : public CSection
{
public:
    COverlay(QString id,CDocument* doc,bool visible);
    virtual ~COverlay();


public slots:

    virtual bool isVisible();
    virtual void setVisibility(bool bVisible);
    void setActive(bool bActive);
    bool active();

    virtual void onEPFEvent(EPFEvent *ev);

private:

    bool m_bVisible;
    bool m_bActive;

};

#endif // COVERLAY_H
