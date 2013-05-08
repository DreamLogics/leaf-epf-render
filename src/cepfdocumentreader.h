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

#ifndef CEPFDOCUMENTREADER_H
#define CEPFDOCUMENTREADER_H

#include "cdocument.h"
#include "iepfobjectfactory.h"
#include <QString>
#include <QMap>
#include "leaf-epf-render_global.h"
#include <QStringList>

class LEAFEPFRENDERSHARED_EXPORT CEPFDocumentReader
{
public:
    CEPFDocumentReader();
    ~CEPFDocumentReader();

    CDocument* loadFromFile(QString filename, QString *error);
    void registerObjectType(QString type, IEPFObjectFactory* factory);
    void addPlatform(QString platform);
    void setLanguage(QString language);

private:

    CBaseObject* createObject(QString type,QString id, CLayer* layer, QString props, QString styleclasses);

private:
    QMap<QString,IEPFObjectFactory*> m_ObjectTypes;
    QStringList m_Platforms;
    QString m_sLanguage;
};

#endif // CEPFDOCUMENTREADER_H
