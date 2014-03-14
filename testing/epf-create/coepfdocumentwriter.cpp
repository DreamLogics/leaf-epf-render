#include "coepfdocumentwriter.h"
#include <QFile>
#include <QDir>
#include <QDataStream>
//#include "czlib.h"
#include <QtLZMA/qlzma.h>
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
    QByteArray data,outdata;
    qint32 size,sizec;
    //qint32 crc32=0;
    qint32 offset;
    unsigned char lzma2;
    //CZLib zipper;

    int offoff;

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

        ds << (qint32)(0); //cert size

        ds << (qint16)(0); //signed hash size

        ds << (qint32)(0); //temp offset

        offoff = outp.pos()-4;

        //content.xml
        data = container.readAll();
        container.close();
        size = data.size();
        if (QLZMA::encode2(data,outdata,&lzma2) != 0)
        //if (!zipper.compress(&data,&crc32))
        {
            (*err) = "Compression error.";
            return false;
        }
        sizec = data.size();
        ds << size << sizec << lzma2;
        outp.write(outdata);
        //ds << data;

        //if (QLZMA::decode2(outdata,data,size,lzma2) != 0)
        //if (!zipper.decompress(&data,size,crc32))
        /*{
            (*err) = "de-compression error.";
            return false;
        }*/

        if (QFile(dir + "/content").exists())
        {
            QList<resource> reslist;
            //parse content files
            parseContentDir(&outp,dir + "/content",&reslist);
            offset = outp.pos();
            for (int i=0;i<reslist.size();i++)
            {
                //ds << (qint32)(0x44454945);
                ds << reslist.at(i).type;
                ds << reslist.at(i).resnamesize;
                ds << reslist.at(i).resextrasize;
                ds << reslist.at(i).filesize;
                ds << reslist.at(i).filesizecompressed;
                ds << reslist.at(i).lzma2;
                ds << reslist.at(i).dataoffset;
                //ds << reslist.at(i).crc32;
                //ds << reslist.at(i).resname.toStdString().c_str();
                outp.write(reslist.at(i).resname.toStdString().c_str(),reslist.at(i).resnamesize);
                //outp.write(reslist.at(i).extra.toStdString().c_str(),reslist.at(i).resextrasize);
            }
            outp.seek(offoff);
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
    QByteArray data,dataout;
    double ratio;
    //CZLib zipper;
    for (int i=0;i<info.size();i++)
    {
        if (info.at(i).isFile())
        {
            QFile f(info.at(i).absoluteFilePath());
            if (f.open(QIODevice::ReadOnly))
            {
                qDebug() << "Writing" << info.at(i).fileName() << "to container...";
                data = f.readAll();
                res.filesize = data.size();
                res.dataoffset = ds->pos();
                if (excludecompress.contains(info.at(i).fileName().section(".",-1)))
                {
                    res.filesizecompressed = 0;
                    res.lzma2 = 0;
                    ds->write(data);
                }
                else
                {
                    //zipper.compress(&data,&res.crc32);
                    qDebug() << "LZMA2 encode...";
                    dataout = QByteArray();
                    QLZMA::encode2(data,dataout,&res.lzma2);
                    res.filesizecompressed = dataout.size();
                    ratio = (double)res.filesizecompressed / (double)res.filesize;
                    qDebug() << "Compressed" << res.filesize << "bytes to" << res.filesizecompressed << "ratio" << ratio;
                    if (ratio >= 1.0)
                    {
                        qDebug() << "Ratio >= 1.0, storing uncompressed...";
                        res.filesizecompressed = 0;
                        res.lzma2 = 0;
                        ds->write(data);
                    }
                    else
                        ds->write(dataout);
                }
                res.resname = info.at(i).fileName();
                res.resnamesize = res.resname.size();
                res.resextrasize = 0;
                res.type = 0;
                reslist->append(res);
            }
        }
    }
}
