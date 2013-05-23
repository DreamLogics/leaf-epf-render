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

#ifndef CLAYER_H
#define CLAYER_H

#include <QObject>
#include <QString>
#include <QList>

class CBaseObject;
class CSection;

class CLayer : public QObject
{
    Q_OBJECT
public:
    CLayer(QString id, CSection *parent = 0);

    QString id();

    CBaseObject* object(int index);
    int objectCount();
    void addObject(CBaseObject* obj);

    CSection* section();

    
signals:
    
public slots:
    

private:
    QString m_sID;
    CSection* m_pSection;

    QList<CBaseObject*> m_Objects;
};

#endif // CLAYER_H
