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

#include "cdocument.h"
#include <QFile>
#include "czlib.h"
#include <QDebug>
#include <QFontDatabase>
#include "canimation.h"
#include "cepfview.h"
#include "clayout.h"
#include "csection.h"
#include "coverlay.h"
#include "clayer.h"
#include "cbaseobject.h"
#include "css/css_style.h"
#include <QTimer>
#include "idevice.h"
#include "cepfjs.h"
#include "canimator.h"

CDocument::CDocument(QStringList platforms, QString language) : m_Platforms(platforms), m_sLanguage(language)
{
    m_pCurrentLayout = 0;
    m_pRenderView = 0;
    //m_pActiveOverlay = 0;
    m_pStylesheet = 0;
    m_bShouldStopLayout = false;
    m_pJS = 0;
}

CDocument::~CDocument()
{
    for (int i=0;i<m_Sections.size();i++)
        delete m_Sections[i];

    for (int i=0;i<m_Overlays.size();i++)
        delete m_Overlays[i];

    for (int i=0;i<m_Layouts.size();i++)
        delete m_Layouts[i];

    QMap<QString,struct Resource>::Iterator it;

    for (it=m_Resources.begin();it!=m_Resources.end();it++)
    {
        it.value().device->close();
        delete it.value().device;
    }

    QMap<QString,QIODevice*>::Iterator it2;

    for (it2=m_devices.begin();it2!=m_devices.end();it2++)
    {
        it2.value()->close();
        delete it2.value();
    }

    for (int i=0;i<m_RegisteredFonts.size();i++)
        Device::currentDevice()->removeApplicationFont(m_RegisteredFonts[i]);


    if (m_pJS)
        delete m_pJS;

    //QFontDatabase::removeAllApplicationFonts();

}

int CDocument::sectionCount()
{
    return m_Sections.size();
}

CSection* CDocument::section(int index)
{
    return m_Sections[index];
}

CSection* CDocument::sectionByID(QString id)
{
    for (int i = 0;i<sectionCount();i++)
    {
        if (section(i)->id() == id)
            return section(i);
    }
    return 0;
}

CSection* CDocument::sectionByPosition(int x, int y)
{
    for (int i = 0;i<sectionCount();i++)
    {
        if (section(i)->x() == x && section(i)->y() == y)
            return section(i);
    }
    return 0;
}

void CDocument::addSection(CSection* section)
{
    m_Sections.append(section);
}


int CDocument::overlayCount()
{
    return m_Overlays.size();
}

COverlay* CDocument::overlay(int index)
{
    return m_Overlays[index];
}

COverlay* CDocument::overlayByID(QString id)
{
    for (int i = 0;i<sectionCount();i++)
    {
        if (overlay(i)->id() == id)
            return overlay(i);
    }
    return 0;
}

void CDocument::addOverlay(COverlay* overlay)
{
    m_Overlays.append(overlay);
}

QString CDocument::property(QString key)
{
    if (!m_Props.contains(key))
        return "";
    return m_Props[key];
}

void CDocument::addProperty(QString key, QString value)
{
    m_Props.insert(key,value);
}

CLayout* CDocument::layoutByID(QString id,bool b)
{
    for (int i=0;i<m_Layouts.size();i++)
    {
        if (m_Layouts[i]->id() == id)
        {
            if (b)
            {
                m_pCurrentLayout = m_Layouts[i];
                if (m_pStylesheet)
                    delete m_pStylesheet;
                m_pStylesheet = new CSS::Stylesheet(m_pCurrentLayout,m_pRenderView->height(),m_pRenderView->width(),this);
            }
            return m_Layouts[i];
        }
    }
    return 0;
}

void CDocument::layout(int height, int width, int sectionid, bool bCurrentSectionOnly)
{
    double target,closest,dc,r;
    int i,match=-1;

    stopLayout(false);

    target = height / width;
    closest = 0;

    for (i=0;i<m_Layouts.size();i++)
    {
        if (!m_Platforms.contains(m_Layouts[i]->platform()))
            continue;

        if (m_sLanguage != m_Layouts[i]->language())
            continue;

        r = m_Layouts[i]->height() / m_Layouts[i]->width();
        dc = r - target;

        if (dc == 0 || (dc < closest && closest != 0))
        {
            match = i;
            closest = dc;
        }

    }

    if (match == -1)
    {
        //zonder language preference
        for (i=0;i<m_Layouts.size();i++)
        {
            if (!m_Platforms.contains(m_Layouts[i]->platform()))
                continue;
            r = m_Layouts[i]->height() / m_Layouts[i]->width();
            dc = r - target;

            if (dc == 0 || (dc < closest && closest != 0))
            {
                match = i;
                closest = dc;
            }
        }
    }

    if (match == -1)
    {
        //geen layout gevonden, dan maar zonder platform preference kijken
        for (i=0;i<m_Layouts.size();i++)
        {
            r = m_Layouts[i]->height() / m_Layouts[i]->width();
            dc = r - target;

            if (dc == 0 || (dc < closest && closest != 0))
            {
                match = i;
                closest = dc;
            }
        }
    }

    if (match == -1)
        return;

    //we hebben er 1 gevonden
    m_mCSSMutex.lock();
    if (m_pCurrentLayout != m_Layouts[match])
    {
        m_pCurrentLayout = m_Layouts[match];
        if (m_pStylesheet)
            delete m_pStylesheet;
        m_pStylesheet = new CSS::Stylesheet(m_pCurrentLayout,height,width,this);
    }
    else
    {
        double hf,wf;
        hf = height;
        hf/=m_pCurrentLayout->height();
        wf = width;
        wf/=m_pCurrentLayout->width();
        m_pStylesheet->setScale(hf,wf);
    }
    m_mCSSMutex.unlock();

    section(sectionid)->layout(height,width);

    updateRenderView();

    if (bCurrentSectionOnly)
        return;

    CSection* s;
    for (i=0;i<sectionCount();i++)
    {
        if (shouldStopLayout())
            return;
        if (i == sectionid)
            continue;
        s = section(i);
        s->layout(height,width);
    }

    COverlay* o;
    for (i=0;i<overlayCount();i++)
    {
        if (shouldStopLayout())
            return;
        o = overlay(i);
        o->layout(height,width);
    }
}

void CDocument::addLayout(CLayout *layout)
{
    //m_Layouts.insert(id,size);
    m_Layouts.push_back(layout);
}


QByteArray CDocument::resource(QString resource)
{
    if (!m_Resources.contains(resource))
    {
        qDebug() << "Requested non existing resource:" << resource;
        return QByteArray();
    }

    struct Resource res = m_Resources[resource];
    QByteArray data;

    if (res.type == 0 || res.type == 1 || res.type == 3)
    {

        QFile f(res.container);

        int s = res.size_compressed;

        if (res.size_compressed == 0)
            s = res.size;

        if (f.open(QIODevice::ReadOnly))
        {
            if (res.type == 3)
            {
                data = f.readAll();

                if (res.size_compressed != 0)
                    CZLib::decompress(&data,res.size,res.checksum);
            }
            else
            {
                if (f.seek(res.offset))
                {
                    data = f.read(s);

                    if (res.size_compressed != 0)
                        CZLib::decompress(&data,res.size,res.checksum);
                }
            }

            f.close();
        }
    }
    else if (res.type == 4)
    {
        qDebug() << "remote res not implemented";
    }
    else
        qDebug() << "unsupported resource type";

    return data;
}

int CDocument::resource(QString resource, char *buffer, int len, int offset)
{
    if (!m_Resources.contains(resource))
    {
        qDebug() << "Requested non existing resource:" << resource;
        return 0;
    }

    int br=0;
    struct Resource res = m_Resources[resource];

    if (res.type == 0 || res.type == 1 || res.type == 3)
    {

        QFile f(res.container);

        int s = res.size_compressed;

        if (res.size_compressed == 0)
            s = res.size;
        else
        {
            qDebug() << "compressed stream impl";
            return 0;
        }

        if (f.open(QIODevice::ReadOnly))
        {
            if (res.type == 3)
            {
                if (f.seek(offset))
                    br = f.read(buffer,len);
            }
            else
            {
                if (f.seek(res.offset + offset))
                {
                    br = f.read(buffer,len);
                }
            }

            f.close();
        }
    }
    else if (res.type == 4)
    {
        qDebug() << "remote res not implemented";
    }
    else
        qDebug() << "unsupported resource type";

    return br;
}

CDocument::Resource CDocument::resourceInfo(QString resource)
{
    struct Resource res;
    res.checksum = -1;
    res.container = "";
    res.extra = "";
    res.offset = -1;
    res.size = -1;
    res.size_compressed = -1;
    res.type = -1;

    if (!m_Resources.contains(resource))
    {
        qDebug() << "Requested non existing resource:" << resource;
        return res;
    }

    res = m_Resources[resource];
    return res;
}

void CDocument::addResource(QString re, QString container_file, QString extra, qint32 checksum, qint32 offset, qint32 size, qint32 size_compressed, qint16 type)
{
    struct Resource res;
    if (type == 1 || type == 3)
        res.container = extra;
    else
        res.container = container_file;
    res.extra = extra;
    res.checksum = checksum;
    res.type = type;
    res.offset = offset;
    res.size = size;
    res.size_compressed = size_compressed;
    res.device = new ResourceIO(re,this);

    res.device->open(QIODevice::ReadOnly);

    if (res.type == 0 || res.type == 1 || res.type == 3)
    {
        if (m_devices.contains(res.container))
            res.external_device = m_devices[res.container];
        else
        {
            res.external_device = new QFile(res.container);
            if (!res.external_device->open(QIODevice::ReadOnly))
                qDebug() << "unable to open file resource container";
            m_devices.insert(res.container,res.external_device);
        }
    }

    m_Resources.insert(re,res);

    if (re == "main.js")
    {
        m_pJS = new CEPFJS(QString::fromUtf8(resource(re)),this);
    }
}

CLayout* CDocument::currentLayout()
{
    return m_pCurrentLayout;
}

void CDocument::addAnimation(QString id, int frames, int fps, QString src)
{
    CAnimation* ani = new CAnimation(frames,fps,src,this);
    m_Animations.insert(id,ani);
}

CAnimation* CDocument::animation(QString id)
{
    if (!m_Animations.contains(id))
        return 0;
    return m_Animations[id];
}

void CDocument::setRenderview(CEPFView *r)
{
    m_pRenderView = r;
}

CEPFView* CDocument::renderview()
{
    return m_pRenderView;
}
/*
void CDocument::setActiveOverlay(COverlay *overlay)
{
    m_pActiveOverlay = overlay;
    emit _setActiveOverlay(overlay->id());
}

COverlay* CDocument::activeOverlay()
{
    return m_pActiveOverlay;
}
*/
QObjectList CDocument::sections()
{
    QObjectList list;
    for (int i=0;i<m_Sections.size();i++)
        list.append(m_Sections[i]);
    return list;
}

QObjectList CDocument::overlays()
{
    QObjectList list;
    for (int i=0;i<m_Overlays.size();i++)
        list.append(m_Overlays[i]);
    return list;
}

QObject* CDocument::getCurrentSection()
{
    return (QObject*)(m_Sections[m_pRenderView->currentSection()]);
}

void CDocument::setCurrentSection(QString section_id)
{
    m_pRenderView->setSection(section_id);
}

void CDocument::playAnimation(QString strAnimation, bool bLoop)
{
    CAnimation* ani = animation(strAnimation);
    if (ani)
    {
        ani->play(bLoop);
    }
}

QObject* CDocument::getSectionByID(QString id)
{
    return (QObject*)(sectionByID(id));
}
/*
void CDocument::setActiveOverlay(QString overlay_id)
{
    emit _setActiveOverlay(overlay_id);
}
*/
void CDocument::load(int height, int width, int sectionid)
{
    CSection* s;
    COverlay* o;
    CLayer* layer;
    CBaseObject* obj;
    int i;

    qDebug() << "load" << height << width;

    for (i=0;i<sectionCount();i++)
    {
        s = section(i);

        //preload
        for (int n=0;n<s->layerCount();n++)
        {
            layer = s->layer(n);
            for (int l=0;l<layer->objectCount();l++)
            {
                obj = layer->object(l);
                qDebug() << "preload" << obj->id();
                obj->preload();
            }
        }
    }

    for (i=0;i<overlayCount();i++)
    {
        o = overlay(i);

        //preload
        for (int n=0;n<o->layerCount();n++)
        {
            layer = o->layer(n);
            for (int l=0;l<layer->objectCount();l++)
            {
                obj = layer->object(l);
                qDebug() << "preload" << obj->id();
                obj->preload();
            }
        }
    }

    layout(height,width,sectionid,false);

    if (m_pJS)
        m_pJS->run();

    //QTimer::singleShot(1000,this,SIGNAL(finishedLoading()));
    sendEvent("finishedLoading");

    emit finishedLoading();
}

CSS::Stylesheet* CDocument::stylesheet(bool hold_control)
{
    if (hold_control)
        m_mCSSMutex.lock();
    return m_pStylesheet;
}

void CDocument::releaseStylesheet()
{
    m_mCSSMutex.unlock();
}

void CDocument::onEPFEvent(EPFEvent *ev)
{
    qDebug() << ev->event();
    if (ev->event() == "playAnimation")
    {
        if (ev->parameter(0) != "")
        {
            CAnimation* ani = animation(ev->parameter(0));
            if (ani)
            {
                bool b=false;
                if (ev->parameter(1) == "true" || ev->parameter(1) == "1")
                    b=true;
                ani->play(b);
            }
        }
    }
    else if (ev->event() == "setSection")
    {
        if (ev->parameter(0) != "")
        {
            CSection* section = sectionByID(ev->parameter(0));
            if (section)
                emit setSection(indexForSection(section));
        }
    }
}

int CDocument::indexForSection(CSection *s)
{
    for (int i=0;i<sectionCount();i++)
    {
        if (section(i) == s)
            return i;
    }
    return -1;
}

/*
void CDocument::makeConnection(EPFComponent *src, QString event, EPFComponent *target, QString function)
{
    src->addConnection(target,event,function);
}
*/
void CDocument::updateRenderView()
{
    emit _updateRenderView();
}

void CDocument::saveBuffers()
{
    CSection* s;
    CLayer* l;
    CBaseObject* obj;
    for (int i=0;i<sectionCount();i++)
    {
        s = section(i);
        for (int n=0;n<s->layerCount();n++)
        {
            l=s->layer(n);
            for (int c=0;c<l->objectCount();c++)
            {
                obj = l->object(c);
                obj->saveBuffer();
            }
        }
    }
}

bool CDocument::shouldStopLayout()
{
    m_mShouldStopLayoutMutex.lock();
    bool b = m_bShouldStopLayout;
    m_mShouldStopLayoutMutex.unlock();
    return b;
}

void CDocument::stopLayout(bool b)
{
    m_mShouldStopLayoutMutex.lock();
    m_bShouldStopLayout = b;
    m_mShouldStopLayoutMutex.unlock();
}

void CDocument::clearBuffers()
{
    for (int i=0;i<sectionCount();i++)
    {
        section(i)->clearBuffers();
    }
    m_pCurrentLayout = 0;
    if (m_pStylesheet)
        delete m_pStylesheet;
    m_pStylesheet = 0;
}

void CDocument::registerFont(int id)
{
    qDebug() << "font registered" << id;
    m_RegisteredFonts.append(id);
}

ResourceIO* CDocument::resourceIO(QString resource)
{
    if (m_Resources.contains(resource))
        return m_Resources[resource].device;
    return 0;
}

ResourceIO::ResourceIO(QString resource, CDocument *doc) : m_sResource(resource), m_pDoc(doc)
{
    //m_iPos = 0;
}

qint64 ResourceIO::readData(char *data, qint64 maxlen)
{
    CDocument::Resource r = m_pDoc->resourceInfo(m_sResource);
    int len=maxlen;
    if (pos()+maxlen > r.size)
    {
        len -= (pos() + maxlen) - r.size;
    }

    if (len == 0)
        return 0;
    else if (len < 0)
        return -1;

    if (!r.external_device->seek(r.offset+pos()))
        return -1;
    /*int read = r.external_device->read(data,len);
    if (read == -1)
        return -1;*/
    //m_iPos += read;
    //qDebug() << "read data" << r.size << pos();
    return r.external_device->read(data,len);
    //return m_pDoc->resource(m_sResource,data,maxlen,pos());
}

qint64 ResourceIO::writeData(const char *data, qint64 len)
{
    qDebug() << "write data";
    return 0;
}

qint64 ResourceIO::bytesAvailable() const
{
    CDocument::Resource r = m_pDoc->resourceInfo(m_sResource);
    return r.size - pos()/* + QIODevice::bytesAvailable()*/;
}

qint64 ResourceIO::size() const
{
    CDocument::Resource r = m_pDoc->resourceInfo(m_sResource);
    return r.size/* + QIODevice::bytesAvailable()*/;
}

bool ResourceIO::isSequential() const
{
    return false;
}

bool ResourceIO::seek(qint64 p)
{
    //return true;
    qDebug() << "seek b" << p << pos();
    return QIODevice::seek(p);
}
/*
qint64 ResourceIO::pos() const
{
    return m_iPos;
}*/
void CDocument::sectionChange(QString id)
{
    CSection* s=0;
    if (id != "")
        s = sectionByID(id);
    CAnimator::get(thread())->setSection(s);
}
