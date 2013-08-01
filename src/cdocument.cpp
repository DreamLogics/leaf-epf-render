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

CDocument::CDocument(QStringList platforms, QString language) : m_Platforms(platforms), m_sLanguage(language)
{
    m_pCurrentLayout = 0;
    m_pRenderView = 0;
    m_pActiveOverlay = 0;
    m_pStylesheet = 0;
    m_bShouldStopLayout = false;
}

CDocument::~CDocument()
{
    for (int i=0;i<m_Sections.size();i++)
        delete m_Sections[i];

    for (int i=0;i<m_Overlays.size();i++)
        delete m_Overlays[i];

    for (int i=0;i<m_Layouts.size();i++)
        delete m_Layouts[i];

    QFontDatabase::removeAllApplicationFonts();

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
    int mm = height * width;
    int d,dc,r,match,i;

    stopLayout(false);

    dc = -1;

    for (i=0;i<m_Layouts.size();i++)
    {
        if (!m_Platforms.contains(m_Layouts[i]->platform()))
            continue;

        if (m_sLanguage != m_Layouts[i]->language())
            continue;

        r = m_Layouts[i]->height() * m_Layouts[i]->width();
        d = r - mm;
        if (d<0)
            d *= -1;

        if (d <= dc || dc == -1)
        {
            match = i;
            dc = d;
        }

    }

    if (dc == -1)
    {
        //zonder language preference
        for (i=0;i<m_Layouts.size();i++)
        {
            if (!m_Platforms.contains(m_Layouts[i]->platform()))
                continue;
            r = m_Layouts[i]->height() * m_Layouts[i]->width();
            d = r - mm;
            if (d<0)
                d *= -1;

            if (d <= dc || dc == -1)
            {
                match = i;
                dc = d;
            }

        }
    }

    if (dc == -1)
    {
        //geen layout gevonden, dan maar zonder platform preference kijken
        for (i=0;i<m_Layouts.size();i++)
        {
            r = m_Layouts[i]->height() * m_Layouts[i]->width();
            d = r - mm;
            if (d<0)
                d *= -1;

            if (d <= dc || dc == -1)
            {
                match = i;
                dc = d;
            }

        }
    }

    if (dc == -1)
        return;

    //we hebben er 1 gevonden
    //lijst maken met alle layouts die een gelijke oppervlakte hebben
    QList<int> ml;

    for (i=0;i<m_Layouts.size();i++)
    {
        r = m_Layouts[i]->height() * m_Layouts[i]->width();
        if (dc == r && i != match)
            ml.push_back(i);
    }

    if (ml.size() > 0)
    {
        dc = -1;
        mm = m_Layouts[match]->height();
        //gelijke oppervlakte gevonden, hoogtes vergelijken
        for (i = 0;i<ml.size();i++)
        {
            r = m_Layouts[ml[i]]->height();
            d = r - mm;
            if (d<0)
                d *= -1;

            if (d <= dc || dc == -1)
            {
                match = ml[i];
                dc = d;
            }
        }
    }

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

void CDocument::addResource(QString resource, QString container_file, QString extra, qint32 checksum, qint32 offset, qint32 size, qint32 size_compressed, qint16 type)
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

    m_Resources.insert(resource,res);
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

void CDocument::setActiveOverlay(COverlay *overlay)
{
    m_pActiveOverlay = overlay;
}

COverlay* CDocument::activeOverlay()
{
    return m_pActiveOverlay;
}

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

void CDocument::setActiveOverlay(QString overlay_id)
{
    setActiveOverlay(overlayByID(overlay_id));
}

void CDocument::load(int height, int width, int sectionid)
{
    CSection* s;
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

    layout(height,width,sectionid,false);

    //QTimer::singleShot(1000,this,SIGNAL(finishedLoading()));
    sendEvent("finishedLoading");

    emit finishedLoading();
}

CSS::Stylesheet* CDocument::stylesheet()
{
    return m_pStylesheet;
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
}
