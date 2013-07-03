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

#include "cepfdocumentreader.h"
#include <QFile>
#include <QDataStream>
#include "czlib.h"
#include "pugixml/src/pugixml.hpp"
#include "csection.h"
#include "clayer.h"
#include <QStringList>
#include <QDebug>
#include "clayout.h"
#include "coverlay.h"
#include <QFontDatabase>
#include "cbaseobject.h"
#include "cunsupportedobject.h"
#include <QThread>
#include <QObject>
#include "epfevent.h"

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
    QMap<QString,IEPFObjectFactory*>::iterator it;
    for (it=m_ObjectTypes.begin();it != m_ObjectTypes.end();it++)
        delete it.value();
}

CDocument* CEPFDocumentReader::loadFromFile(QString filename, QString* error, QThread* create_in_thread)
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

    qint16 restype;
    qint16 resnamesize;
    qint16 resextrasize;
    qint32 filesize;
    qint32 filesizecompressed;
    qint32 dataoffset;
    //qint32 crc32;
    QString resname;
    QString extrafield;



    if (f.open(QIODevice::ReadOnly))
    {
        ds.setDevice(&f);
        //header
        ds >> magic;
        if (magic != 0x44455046 ) //DEPF
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

            if (magic != 0x44454945 ) //DEIE
            {
                (*error) = "File corruption detected (file entry header).";
                delete document;
                return 0;
            }

            ds >> restype >> resnamesize >> resextrasize >> filesize >> filesizecompressed >> dataoffset/* >> crc32*/;

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
            data = f.read(resextrasize);
            extrafield = QString::fromUtf8(data.constData(),data.size());

            //maak resource entry
            document->addResource(resname,filename,extrafield/*,crc32*/,dataoffset,filesize,filesizecompressed,restype);


        }

        f.close();

        //lees de container.xml
        if (compressed)
        {
            if(!CZLib::decompress(&container,containersize/*,containercrc32*/))
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

        pugi::xml_node modules = containerxml.child("EPF").child("modules");

        for (pugi::xml_node module = modules.child("module"); module; module = module.next_sibling("module"))
        {
            if (module.attribute("required").as_bool() && !m_Modules.contains(module.attribute("id").value()))
            {
                (*error) = "The module \""+QString(module.attribute("id").value())+"\" is required but not available.";
                delete document;
                return 0;
            }
        }



        pugi::xml_node props = containerxml.child("EPF").child("info");

        for (pugi::xml_node prop = props.child("prop"); prop; prop = prop.next_sibling("prop"))
        {
            document->addProperty(prop.attribute("key").value(),prop.child_value());
            //qDebug() << "property added: "<<prop.attribute("key").value() << prop.child_value();
        }

        pugi::xml_node fonts = containerxml.child("EPF").child("fonts");
        for (pugi::xml_node font = fonts.child("font"); font; font = font.next_sibling("font"))
        {
            if (QFontDatabase::addApplicationFontFromData(document->resource(font.attribute("src").value())) == -1)
                qDebug() << "Could not load font:" << font.attribute("src").value();
        }

        pugi::xml_node layouts = containerxml.child("EPF").child("layouts");
        //QRect lrect;
        //int sz;
        QString style;
        CLayout *lo;
        QByteArray sba;

        for (pugi::xml_node layout = layouts.child("layout"); layout; layout = layout.next_sibling("layout"))
        {
            //sz = layout.attribute("width").as_int() * layout.attribute("height").as_int();


            //load resource
            sba = document->resource(layout.attribute("src").value());
            style = QString::fromUtf8(sba.constData(),sba.size());
            //document->setStyle(layout.attribute("id").value(),style);

            lo = new CLayout(layout.attribute("id").value(),layout.attribute("height").as_int(),layout.attribute("width").as_int(),style,layout.attribute("platform").value(), layout.attribute("lang").value());
            document->addLayout(lo);

            //qDebug() << "layout added: "<<layout.attribute("id").value() << "dimension: "<<sz;
        }


        QMap<QString,CSection*> sectionmap;

        pugi::xml_node sections = containerxml.child("EPF").child("sections");
        pugi::xml_node overlays = containerxml.child("EPF").child("overlays");
        pugi::xml_node object;
        pugi::xml_node layer;

        CSection* s;
        COverlay* o;
        CLayer* l;
        CBaseObject* obj;

        //int renderstyle;
        bool bActive;

        //overlays

        for (pugi::xml_node overlay = overlays.child("overlay"); overlay; overlay = overlay.next_sibling("overlay"))
        {
            //section maken
            o = new COverlay(overlay.attribute("id").value(),document,false);
            sectionmap.insert(overlay.attribute("id").value(),o);

            o->setVisibility(overlay.attribute("visible").as_bool());

            for (layer = overlay.child("layer"); layer; layer = layer.next_sibling("layer"))
            {

                l = new CLayer(layer.attribute("id").value(),o);

                /*if (layer.attribute("active").as_int() == 1)
                    bActive = true;
                else
                    bActive = false;*/

                //o->addLayer(l/*,bActive*/);

                //objects maken
                for (object = layer.child("object"); object; object = object.next_sibling("object"))
                {
                    obj = createObject(object.attribute("type").value(),object.attribute("id").value(),l,object.attribute("properties").value(),object.attribute("styles").value(),object.attribute("enabled").as_bool());
                    m_objectmap.insert(QString(overlay.attribute("id").value())+":"+QString(object.attribute("id").value()),obj);

                    obj->setParent(/*(QObject*)*/l);
                    //obj->setParentItem(/*(QGraphicsItem*)*/l);
                    //obj->setParent((QObject*)o->documentItem());
                    //obj->setParentItem(o->documentItem());

                    //qDebug() << "object made of type("<< object.attribute("type").value() <<"): "<<object.attribute("id").value();

                    //s->addObject(obj,object.attribute("id").value(),object.attribute("layer").as_int());
                    l->addObject(obj);


                    parseObjectNode(&object,l,o,obj);
                }

                o->addLayer(l);

            }


            document->addOverlay(o);
        }



        //sections verwerken

        for (pugi::xml_node section = sections.child("section"); section; section = section.next_sibling("section"))
        {
            //section maken
            s = new CSection(section.attribute("id").value(),document,section.attribute("hidden").as_bool());
            sectionmap.insert(section.attribute("id").value(),o);

            for (layer = section.child("layer"); layer; layer = layer.next_sibling("layer"))
            {

                l = new CLayer(layer.attribute("id").value(),s);

                /*if (layer.attribute("active").as_int() == 1)
                    bActive = true;
                else
                    bActive = false;*/

                //s->addLayer(l/*,bActive*/);

                //objects maken
                for (object = layer.child("object"); object; object = object.next_sibling("object"))
                {
                    obj = createObject(object.attribute("type").value(),object.attribute("id").value(),l,object.attribute("properties").value(),object.attribute("styles").value(),object.attribute("enabled").as_bool());
                    m_objectmap.insert(QString(section.attribute("id").value())+":"+QString(object.attribute("id").value()),obj);

                    obj->setParent(l);
                    //obj->setParentItem(l);
                    //obj->setParent((QObject*)s->documentItem());
                    //obj->setParentItem(s->documentItem());


                    //qDebug() << "object made of type("<< object.attribute("type").value()<<"): "<<object.attribute("id").value();

                    //s->addObject(obj,object.attribute("id").value(),object.attribute("layer").as_int());
                    l->addObject(obj);

                    parseObjectNode(&object,l,o,obj);
                }

                s->addLayer(l);

            }

            document->addSection(s);
        }


        //qDebug() << "ani";

        QStringList anims;

        //animations
        pugi::xml_node animations = containerxml.child("EPF").child("animations");

        for (pugi::xml_node animation = animations.child("animation"); animation; animation = animation.next_sibling("animation"))
        {
            anims.append(animation.attribute("id").value());
            document->addAnimation(animation.attribute("id").value(),animation.attribute("frames").as_int(),animation.attribute("fps").as_int(),animation.attribute("src").value());
        }

        //qDebug() << "con";

        //connections
        pugi::xml_node connections = containerxml.child("EPF").child("connections");
        QString func,param;
        //QStringList parameters;
        //QRegExp funcreg("([^\\(]+)\\(([^\\)]*)\\)");

        for (pugi::xml_node connection = connections.child("connection"); connection; connection = connection.next_sibling("connection"))
        {
            func = connection.attribute("function").value();
            //if (funcreg.indexIn(func) != -1)
            {
                //func = funcreg.cap(1);
                //param = funcreg.cap(2);
                EPFComponent* src_obj=0;
                EPFComponent* dst_obj=0;

                //qDebug() << "add con" << connection.attribute("source").value() << connection.attribute("target").value();

                if (m_objectmap.contains(connection.attribute("source").value()))
                    src_obj = m_objectmap[connection.attribute("source").value()];
                else if (sectionmap.contains(connection.attribute("source").value()))
                    src_obj = m_objectmap[connection.attribute("source").value()];
                else if (anims.contains(connection.attribute("source").value()))
                    src_obj = m_objectmap[connection.attribute("source").value()];
                else if (!strcmp(connection.attribute("source").value(),"document"))
                    src_obj = document;

                if (m_objectmap.contains(connection.attribute("target").value()))
                    dst_obj = m_objectmap[connection.attribute("target").value()];
                else if (sectionmap.contains(connection.attribute("target").value()))
                    dst_obj = m_objectmap[connection.attribute("target").value()];
                else if (anims.contains(connection.attribute("target").value()))
                    dst_obj = m_objectmap[connection.attribute("target").value()];
                else if (!strcmp(connection.attribute("target").value(),"document"))
                    dst_obj = document;

                if (src_obj && dst_obj)
                {
                    //QObject::connect(src_obj,SIGNAL());
                    //SLOT(tset);
                    src_obj->addConnection(dst_obj,connection.attribute("event").value(),func);
                }
            }
        }

        //qDebug() << "moving to thread";
        //(*error) = "test";

        if (create_in_thread)
        {
            document->moveToThread(create_in_thread);
            qDebug() << "moved";
        }


        return document;

    }

    (*error) = "Could not open file.";
    return 0;

}

void CEPFDocumentReader::registerObjectType(QString type, IEPFObjectFactory *factory)
{
    m_ObjectTypes.insert(type,factory);
}

CBaseObject* CEPFDocumentReader::createObject(QString type, QString id, CLayer* layer, QString props, QString styleclasses,bool enabled)
{
    IEPFObjectFactory* factory;
    CBaseObject* obj;
    QStringList propslist,prop,styles;

    if (m_ObjectTypes.contains(type))
    {
        factory = m_ObjectTypes[type];
        obj = factory->create(id,layer);

        obj->setEnabled(enabled);

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
    return new CUnsupportedObject(id,layer);
}

void CEPFDocumentReader::addPlatform(QString platform)
{
    m_Platforms << platform;
}

void CEPFDocumentReader::setLanguage(QString language)
{
    m_sLanguage = language;
}

void CEPFDocumentReader::parseObjectNode(pugi::xml_node *node, CLayer *layer, CSection* section,CBaseObject* parent)
{
    pugi::xml_node object;
    CBaseObject* obj;

    if (node->empty())
        return;

    for (object = node->child("object"); object; object = object.next_sibling("object"))
    {
        obj = createObject(object.attribute("type").value(),object.attribute("id").value(),layer,object.attribute("properties").value(),object.attribute("styles").value(),object.attribute("enabled").as_bool());
        m_objectmap.insert(section->id()+":"+QString(object.attribute("id").value()),obj);

        obj->setParent(parent);

        //qDebug() << "object made of type("<< object.attribute("type").value()<<"): "<<object.attribute("id").value();

        //s->addObject(obj,object.attribute("id").value(),object.attribute("layer").as_int());
        layer->addObject(obj);

        parseObjectNode(&object,layer,section,obj);
    }
}

void CEPFDocumentReader::addModule(QString module)
{
    m_Modules.append(module);
}
