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

#ifndef CANIMFRAME_H
#define CANIMFRAME_H

#include <QObject>
#include <QMap>

class CSection;
class CBaseObject;

class CAnimFrame : public QObject
{
    Q_OBJECT
public:    
    CAnimFrame(CSection* s);

    void setPropertiesForObject(CBaseObject* object, QMap<QString,QString>);
    void apply();
    QMap<CBaseObject*,QMap<QString,QString> > objectPropMap();
    bool hasObjectProps(CBaseObject* obj);
    CSection* section();

private:
    QMap<CBaseObject*,QMap<QString,QString> > m_Objects;
    CSection* m_pSection;

signals:
    
public slots:
    
};

#endif // CANIMFRAME_H
