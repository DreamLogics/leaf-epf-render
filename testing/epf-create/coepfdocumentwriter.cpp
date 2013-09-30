#include "coepfdocumentwriter.h"
#include <QFile>
#include <QDir>
#include <QDataStream>
#include "czlib.h"
#include <QByteArray>
#include <QDebug>

COEPFDocumentWriter::COEPFDocumentWriter()
{
}

bool COEPFDocumentWriter::writeDocument(QString dir, QString *err)
{
    if (dir.right(1) == "/")
        dir = dir.left(dir.size()-1);

    QFile container(dir + "/content.xml");
    QFile outp(dir + ".epf");
    QByteArray data;
    qint32 size,sizec;
    qint32 crc32=0;
    qint32 offset;
    CZLib zipper;

    if (!container.exists())
    {
        (*err) = "content.xml not found!";
        return false;
    }

    if (!container.open(QIODevice::ReadOnly))
    {
        (*err) = "Could not open content.xml for reading!";
        return false;
    }

    if (outp.open(QIODevice::WriteOnly))
    {
        QDataStream ds(&outp);

        ds << (qint32)(0x44455046); //magic
        ds << (qint16)(1); //version
        ds << (qint32)(0); //temp offset

        //content.xml
        data = container.readAll();
        container.close();
        size = data.size();
        if (!zipper.compress(&data,&crc32))
        {
            (*err) = "Compression error.";
            return false;
        }
        sizec = data.size();
        ds << size << sizec << crc32;
        outp.write(data);
        //ds << data;

        if (!zipper.decompress(&data,size,crc32))
        {
            (*err) = "de-compression error.";
            return false;
        }

        if (QFile(dir + "/content").exists())
        {
            QList<resource> reslist;
            //parse content files
            parseContentDir(&outp,dir + "/content",&reslist);
            offset = outp.pos();
            for (int i=0;i<reslist.size();i++)
            {
                ds << (qint32)(0x44454945);
                ds << reslist.at(i).type;
                ds << reslist.at(i).resnamesize;
                ds << reslist.at(i).resextrasize;
                ds << reslist.at(i).filesize;
                ds << reslist.at(i).filesizecompressed;
                ds << reslist.at(i).dataoffset;
                ds << reslist.at(i).crc32;
                //ds << reslist.at(i).resname.toStdString().c_str();
                outp.write(reslist.at(i).resname.toStdString().c_str(),reslist.at(i).resnamesize);
                //outp.write(reslist.at(i).extra.toStdString().c_str(),reslist.at(i).resextrasize);
            }
            outp.seek(6);
            ds << offset;
        }
    }
    else
    {
        (*err) = "Could not open "+outp.fileName()+" for writing!";
        outp.close();
        return false;
    }

    outp.close();
    return true;
}

void COEPFDocumentWriter::parseContentDir(QFile *ds, QString dir, QList<resource> *reslist)
{
    struct resource res;
    QDir d(dir);
    QStringList excludecompress = QStringList() << "ogv" << "ogg" << "webm";
    QFileInfoList info = d.entryInfoList();
    QByteArray data;
    CZLib zipper;
    for (int i=0;i<info.size();i++)
    {
        if (info.at(i).isFile())
        {
            QFile f(info.at(i).absoluteFilePath());
            if (f.open(QIODevice::ReadOnly))
            {
                data = f.readAll();
                res.filesize = data.size();
                if (excludecompress.contains(info.at(i).fileName().section(".",-1)))
                {
                    res.filesizecompressed = 0;
                    res.crc32 = 0;
                }
                else
                {
                    zipper.compress(&data,&res.crc32);
                    res.filesizecompressed = data.size();
                }
                res.dataoffset = ds->pos();
                res.resname = info.at(i).fileName();
                res.resnamesize = res.resname.size();
                res.resextrasize = 0;
                res.type = 0;
                reslist->append(res);
                ds->write(data.constData(),data.size());
            }
        }
    }
}
