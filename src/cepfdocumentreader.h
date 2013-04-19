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
