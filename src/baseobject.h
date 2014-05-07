/****************************************************************************
**
** LEAF EPF Render engine
** http://leaf.dreamlogics.com/
**
** Copyright (C) 2014 DreamLogics
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
#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <QPixmap>

namespace LEAFEPF
{
class BaseObject_Private;

class BaseObject
{
public:
    BaseObject(BaseObject_Private* p);
    ~BaseObject();

    virtual void preload() const;

    virtual void render(QPainter *painter) const;

    QPixmap mask() const;

    QString id() const;

    virtual const char* objectType() const = 0;

    bool enabled() const;
    void setEnabled(bool b) const;




protected:
    BaseObject_Private* m_p;
};

}

#endif // BASEOBJECT_H
