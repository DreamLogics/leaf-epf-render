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

#include "clayer.h"
#include <QPainter>

CLayer::CLayer(QString id, CSection* s) :
    m_sID(id), m_pSection(s)
{
    setParent((QObject*)s);
}

QString CLayer::id()
{
    return m_sID;
}

CBaseObject* CLayer::object(int index)
{
    if (index < 0 || index >= m_Objects.size())
        return 0;

    return m_Objects[index];
}

int CLayer::objectCount()
{
    return m_Objects.size();
}

CSection* CLayer::section()
{
    return m_pSection;
}

void CLayer::addObject(CBaseObject *obj)
{
    m_Objects.append(obj);
}

QRectF CLayer::boundingRect() const
{
    //return childrenBoundingRect();
    return QRectF();
}

void CLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->fillRect(0,0,100,100,QColor("red"));
}
