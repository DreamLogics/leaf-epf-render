#ifndef COEPFDOCUMENTWRITER_H
#define COEPFDOCUMENTWRITER_H

#include <QString>
#include <QList>

class QFile;

class COEPFDocumentWriter
{
public:
    COEPFDocumentWriter();

    struct resource
    {
        qint16 type;
        qint16 resnamesize;
        qint16 resextrasize;
        qint32 filesize;
        qint32 filesizecompressed;
        qint32 dataoffset;
        //qint32 crc32;
        QString resname;
        QString extra;
    };

    bool writeDocument(QString dir, QString* err);
    void parseContentDir(QFile* ds, QString dir, QList<struct resource> *reslist);

};

#endif // COEPFDOCUMENTWRITER_H
