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

#ifndef CLABELOBJECT_H
#define CLABELOBJECT_H

#include "../cbaseobject.h"
#include "../iepfobjectfactory.h"

class QPainter;

class CLabelObjectFactory : public IEPFObjectFactory
{
    virtual CBaseObject* create(QString id, CLayer *layer);
};

class CLabelObject : public CBaseObject
{
    Q_OBJECT
public:
    CLabelObject(QString id, CLayer* layer);

    virtual void preload();
    virtual void paint(QPainter *painter);

signals:

public slots:

private:

    QString m_sText;

};

#endif // CLABELOBJECT_H
