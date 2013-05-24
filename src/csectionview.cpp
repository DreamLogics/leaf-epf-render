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

#include "csectionview.h"
#include "csection.h"
#include <QPainter>

CSectionView::CSectionView(CSection* section) : m_pSection(section)
{
    connect(section,SIGNAL(changed(QList<QRectF>)),this,SLOT(sectionChanged()),Qt::QueuedConnection);
}

CSection* CSectionView::sectionObject()
{
    return m_pSection;
}

void CSectionView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(0,0,m_pSection->rendered());
}

void CSectionView::sectionChanged()
{
    update();
}

QRectF CSectionView::boundingRect() const
{
    return QRectF(0,0,m_pSection->rendered().width(),m_pSection->rendered().height());
}
