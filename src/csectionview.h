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

#ifndef CSECTIONVIEW_H
#define CSECTIONVIEW_H

#include <QGraphicsObject>

class CSection;

class CSectionView : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit CSectionView(CSection* section);

    CSection* sectionObject();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual QRectF boundingRect() const;
    
signals:
    
public slots:
    
    void sectionChanged();

private:

    CSection* m_pSection;
};

#endif // CSECTIONVIEW_H
