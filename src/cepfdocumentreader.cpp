#include "cepfdocumentreader.h"
#include <QFile>
#include <QDataStream>
#include "czlib.h"
#include "pugixml/pugixml.hpp"
#include "csection.h"
#include "clayer.h"
#include <QStringList>
#include <QDebug>
#include "clayout.h"
#include "coverlay.h"
#include <QFontDatabase>
#include "cbaseobject.h"
#include "cunsupportedobject.h"

#include "object_types/cblockobject.h"
#include "object_types/ctextobject.h"
#include "object_types/cmediaobject.h"
#include "object_types/cimageobject.h"
#include "object_types/cscriptobject.h"

CEPFDocumentReader::CEPFDocumentReader()
{
    registerObjectType("text",new CTextObjectFactory());
    registerObjectType("image",new CImageObjectFactory());
    registerObjectType("block",new CBlockObjectFactory());
    registerObjectType("media",new CMediaObjectFactory());
    registerObjectType("scripted",new CScriptObjectFactory());
}

CEPFDocumentReader::~CEPFDocumentReader()
{
    QMap<QString,IOEPFObjectFactory*>::iterator it;
    for (it=m_ObjectTypes.begin();it != m_ObjectTypes.end();it++)
        delete it.value();
}

CDocument* CEPFDocumentReader::loadFromFile(QString filename, QString* error)
{
    QFile f(filename);
    QByteArray data,container;
    QDataStream ds;
    qint32 magic;
    qint16 version;
    qint32 offsetind;

    bool compressed=true;
    qint32 containersize;
    qint32 containercrc32;

    qint16 resnamesize;
    qint32 filesize;
    qint32 filesizecompressed;
    qint32 dataoffset;
    qint32 crc32;
    QString resname;

    if (f.open(QIODevice::ReadOnly))
    {
        ds.setDevice(&f);
        //header
        ds >> magic;
        if (magic != 0x4F455046 ) //OEPF
        {
            //error
            (*error) = "Filetype not supported.";
            return 0;
        }

        ds >> version;

        if (version > 1)
        {
            (*error) = "The version is too new.";
            return 0;
        }

        ds >> offsetind;

        //container.xml
        ds >> containersize >> filesizecompressed >> containercrc32;
        dataoffset = f.pos();

        if (!(containersize > 0 && filesizecompressed >= 0 && ((filesizecompressed > 0 && dataoffset + filesizecompressed < f.size()) || (filesizecompressed == 0 && dataoffset + containersize < f.size()))))
        {
            (*error) = "File corruption detected (container xml).";
            return 0;
        }

        if (filesizecompressed == 0)
        {
            compressed = false;
            filesizecompressed = containersize;
        }

        container = f.read(filesizecompressed);

        if (!f.seek(offsetind))
        {
            (*error) = "This file is damaged.";
            return 0;
        }

        CDocument* document = new CDocument(m_Platforms,m_sLanguage);

        while (!f.atEnd())
        {
            ds >> magic;

            if (magic != 0x4F454945 ) //OEIE
            {
                (*error) = "File corruption detected (file entry header).";
                delete document;
                return 0;
            }

            ds >> resnamesize >> filesize >> filesizecompressed >> dataoffset >> crc32;

            //TODO empty files
            if (!(resnamesize > 0 && filesize > 0 && filesizecompressed >= 0 && dataoffset > 0 && dataoffset < offsetind
                  && ((filesizecompressed > 0 && dataoffset + filesizecompressed <= offsetind) || (filesizecompressed == 0 && dataoffset + filesize <= offsetind))))
            {
                (*error) = "File corruption detected (entry offset: "+QString::number(f.pos()-18)+" data offset: "+QString::number(dataoffset)+").";
                delete document;
                return 0;
            }
            data = f.read(resnamesize);
            resname = QString::fromUtf8(data.constData(),data.size());

            //maak resource entry
            document->addResource(resname,filename,crc32,dataoffset,filesize,filesizecompressed);


        }

        f.close();

        //lees de container.xml
        if (compressed)
        {
            if(!CZLib::decompress(&container,containersize,containercrc32))
            {
                (*error) = "Compression error in container.xml.";
                delete document;
                return 0;
            }
        }

        pugi::xml_document containerxml;
        pugi::xml_parse_result r = containerxml.load_buffer(container.constData(),container.size());

        if (!r)
        {
            (*error) = "File corruption detected. Could not load XML.";
            delete document;
            return 0;
        }

        pugi::xml_node props = containerxml.child("OEPF").child("info");

        for (pugi::xml_node prop = props.child("prop"); prop; prop = prop.next_sibling("prop"))
        {
            document->addProperty(prop.attribute("key").value(),prop.child_value());
            qDebug() << "property added: "<<prop.attribute("key").value() << prop.child_value();
        }

        pugi::xml_node fonts = containerxml.child("OEPF").child("fonts");
        for (pugi::xml_node font = fonts.child("font"); font; font = font.next_sibling("font"))
        {
            if (QFontDatabase::addApplicationFontFromData(document->getResource(font.attribute("src").value())) == -1)
                qDebug() << "Could not load font:" << font.attribute("src").value();
        }

        pugi::xml_node layouts = containerxml.child("OEPF").child("layouts");
        //QRect lrect;
        //int sz;
        QString style;
        CLayout *lo;
        QByteArray sba;

        for (pugi::xml_node layout = layouts.child("layout"); layout; layout = layout.next_sibling("layout"))
        {
            //sz = layout.attribute("width").as_int() * layout.attribute("height").as_int();


            //load resource
            sba = document->getResource(layout.attribute("src").value());
            style = QString::fromUtf8(sba.constData(),sba.size());
            //document->setStyle(layout.attribute("id").value(),style);

            lo = new COEPFLayout(layout.attribute("id").value(),layout.attribute("height").as_int(),layout.attribute("width").as_int(),style,layout.attribute("platform").value(), layout.attribute("lang").value());
            document->addLayout(lo);

            //qDebug() << "layout added: "<<layout.attribute("id").value() << "dimension: "<<sz;
        }

        QMap<QString,IOEPFObject*> objectmap;
        QMap<QString,IOEPFSection*> sectionmap;

        pugi::xml_node sections = containerxml.child("OEPF").child("sections");
        pugi::xml_node overlays = containerxml.child("OEPF").child("overlays");
        pugi::xml_node object;
        pugi::xml_node layer;

        COEPFSection* s;
        COEPFOverlay* o;
        COEPFLayer* l;
        IOEPFObject* obj;

        //int renderstyle;
        bool bActive;

        //overlays

        for (pugi::xml_node overlay = overlays.child("overlay"); overlay; overlay = overlay.next_sibling("overlay"))
        {
            //section maken
            o = new COEPFOverlay(overlay.attribute("id").value(),document,false);
            sectionmap.insert(overlay.attribute("id").value(),o);

            o->setVisibility(overlay.attribute("visible").as_bool());

            for (layer = overlay.child("layer"); layer; layer = layer.next_sibling("layer"))
            {

                l = new COEPFLayer(document);

                //objects maken
                for (object = layer.child("object"); object; object = object.next_sibling("object"))
                {
                    obj = createObject(object.attribute("type").value(),object.attribute("id").value(),object.attribute("properties").value(),object.value(),object.attribute("styles").value());
                    obj->setSection(o);
                    obj->setDocument(document);
                    objectmap.insert(QString(overlay.attribute("id").value())+":"+QString(object.attribute("id").value()),obj);

                    qDebug() << "object made of type("<< object.attribute("type").value()<<"): "<<object.attribute("id").value();

                    //s->addObject(obj,object.attribute("id").value(),object.attribute("layer").as_int());
                    l->addObject(obj);
                }

                if (layer.attribute("active").as_int() == 1)
                    bActive = true;
                else
                    bActive = false;

                o->addLayer(l,bActive);

            }

            document->addOverlay(o);
        }

        //sections verwerken

        for (pugi::xml_node section = sections.child("section"); section; section = section.next_sibling("section"))
        {
            //section maken
            s = new COEPFSection(section.attribute("id").value(),document,section.attribute("hidden").as_bool());
            sectionmap.insert(section.attribute("id").value(),o);

            for (layer = section.child("layer"); layer; layer = layer.next_sibling("layer"))
            {

                l = new COEPFLayer(document);

                //objects maken
                for (object = layer.child("object"); object; object = object.next_sibling("object"))
                {
                    obj = createObject(object.attribute("type").value(),object.attribute("id").value(),object.attribute("properties").value(),object.value(),object.attribute("styles").value());
                    obj->setSection(s);
                    obj->setDocument(document);
                    objectmap.insert(QString(section.attribute("id").value())+":"+QString(object.attribute("id").value()),obj);

                    qDebug() << "object made of type("<< obj->type() <<"): "<<object.attribute("id").value();

                    //s->addObject(obj,object.attribute("id").value(),object.attribute("layer").as_int());
                    l->addObject(obj);
                }

                if (layer.attribute("active").as_int() == 1)
                    bActive = true;
                else
                    bActive = false;

                s->addLayer(l,bActive);

            }

            document->addSection(s);
        }

        QStringList anims;

        //animations
        pugi::xml_node animations = containerxml.child("OEPF").child("animations");

        for (pugi::xml_node animation = animations.child("animation"); animation; animation = animation.next_sibling("animation"))
        {
            anims.append(animation.attribute("id").value());
            document->addAnimation(animation.attribute("id").value(),animation.attribute("frames").as_int(),animation.attribute("fps").as_int(),animation.attribute("src").value());
        }

        //connections
        pugi::xml_node connections = containerxml.child("OEPF").child("connections");
        QString func,param;
        //QStringList parameters;
        QRegExp funcreg("([^\\(]+)\\(([^\\)]*)\\)");

        for (pugi::xml_node connection = connections.child("connection"); connection; connection = connection.next_sibling("connection"))
        {
            func = connection.attribute("function").value();
            if (funcreg.indexIn(func) != -1)
            {
                func = funcreg.cap(1);
                param = funcreg.cap(2);
                if (objectmap.contains(connection.attribute("source").value()))
                {
                    ev = new CEPFEvent(func,param);
                    objectmap[connection.attribute("source").value()]->addConnection(connection.attribute("event").value(),connection.attribute("target").value(),ev);
                }
                else if (sectionmap.contains(connection.attribute("source").value()))
                {
                    ev = new CEPFEvent(func,param);
                    sectionmap[connection.attribute("source").value()]->addConnection(connection.attribute("event").value(),connection.attribute("target").value(),ev);
                }
                else if (anims.contains(connection.attribute("source").value()))
                {
                    IOEPFAnimation* ani = document->getAnimation(connection.attribute("source").value());
                    ev = new CEPFEvent(func,param);
                    ani->addConnection(connection.attribute("event").value(),connection.attribute("target").value(),ev);
                }
            }
        }

        return document;

    }

    (*error) = "Could not open file.";
    return 0;

}

void CEPFDocumentReader::registerObjectType(QString type, IOEPFObjectFactory *factory)
{
    m_ObjectTypes.insert(type,factory);
}

CBaseObject* CEPFDocumentReader::createObject(QString type, QString id, CLayer* layer, QString props, QString styleclasses)
{
    IEPFObjectFactory* factory;
    CBaseObject* obj;
    QStringList propslist,prop,styles;

    if (m_ObjectTypes.contains(type))
    {
        factory = m_ObjectTypes[type];
        obj = factory->create(id,layer);

        propslist = props.split(";");
        for (int i=0;i<propslist.size();i++)
        {
            prop = propslist.at(i).split(":");
            if (prop.size() == 2)
                obj->setProperty(prop[0],prop[1]);
        }

        styles = styleclasses.split(";");
        for (int i=0;i<styles.size();i++)
            obj->addStyleClass(styles[i]);

        return obj;
    }
    return new CUnsupportedObject(id);
}

void CEPFDocumentReader::addPlatform(QString platform)
{
    m_Platforms << platform;
}

void CEPFDocumentReader::setLanguage(QString language)
{
    m_sLanguage = language;
}
