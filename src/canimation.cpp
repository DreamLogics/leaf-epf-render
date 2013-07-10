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

#include "canimation.h"
#include "css/css_style.h"
#include <QColor>
#include <QDebug>
#include <QTime>
#include "cdocument.h"
#include "csection.h"
#include "cbaseobject.h"
#include "clayout.h"
#include "canimframe.h"
#include "cepfview.h"

CAnimation::CAnimation(int frames, int fps, QString src, CDocument* doc)
{
    //10:ipad:section-sorting-new:menu-sorting-new-active{top: 120px;}
    m_pDoc = doc;

    QByteArray data = doc->resource(src);
    QString animdata = QString::fromUtf8(data.constData(),data.size());
    animdata = animdata.replace(QRegExp("[\t\n\r]+"),"");
    QRegExp anireg("([0-9]+):([^:]+):([^\\{]+)\\{([^\\}]+)\\}");
    QStringList props,prop,selectorl;
    QString selector,propsdata,key,val,layoutid;
    CAnimFrame* aniframe;
    QMap<QString,QString> propmap;//,lastframeprops,tempmap;

    int i;
    //int curframe = 0;
    int frame;
    int offset=0;

    CSection* sec=0;
    CBaseObject* obj;
    CLayout* layout;

    QMap<int,CAnimFrame*> framemap;
    QList<QString> layouts;

    while (animdata.indexOf(anireg,offset) != -1)
    {

        frame = anireg.cap(1).toInt();
        layoutid = anireg.cap(2);
        selector = anireg.cap(3);
        propsdata = anireg.cap(4);


        layout = doc->layoutByID(layoutid);
        if (!layout)
            continue;


        //layouts.append(layoutid);

        selectorl = selector.split(":");
        if (selectorl.size() != 2)
            break;


        sec = doc->sectionByID(selectorl[0]);
        if (!sec)
            break;

        obj = sec->objectByID(selectorl[1]);
        if (!obj)
            break;

        if (m_Frames.contains(layoutid))
            framemap = m_Frames[layoutid];
        else
            framemap.clear();

        /*if (framemap.size() == 0 && frame != 1)
        {
            CStyleParser sp(layout,obj->getID(),sec->getID(),layout->height(),layout->width(),obj->styleClasses());
            QStringList sl = sp.keys();
            propmap.clear();
            for (i=0;i<sl.size();i++)
            {
                propmap.insert(sl[i],sp.property(sl[i]));
            }
            aniframe = new CAnimFrame(sec);
            aniframe->setPropertiesForObject(obj,propmap);
            framemap.insert(1,aniframe);
        }*/

        props = propsdata.split(";");
        propmap.clear();

        for (i=0;i<props.size();i++)
        {
            prop = props[i].split(":");
            if (prop.size() == 2)
            {
                key = prop[0].replace(QRegExp("(^ +| +$)"),"");
                val = prop[1].replace(QRegExp("(^ +| +$)"),"");
                propmap.insert(key,val);
            }
        }

        if (framemap.contains(frame))
            aniframe = framemap[frame];
        else
            aniframe = new CAnimFrame(sec);//sec->renderer());

        aniframe->setPropertiesForObject(obj,propmap);

        framemap.insert(frame,aniframe);

        if (m_Frames.contains(layoutid))
            m_Frames[layoutid] = framemap;
        else
            m_Frames.insert(layoutid,framemap);

        offset += anireg.cap(0).size();
    }

    m_iFrameCount = frames;
    layouts = m_Frames.keys();
/*
    if (sec)
    {
        for (int n=0;n<layouts.size();n++)
            generateFrames(layouts[n]);
    }
*/
    m_pGeneratedFrame = 0;
    m_iCurFrame = 1;
    m_pTimer = new QTimer();
    m_pTimer->setInterval(1000/fps);
    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(playNextFrame()));
}

CAnimation::~CAnimation()
{
    delete m_pTimer;
}

QString CAnimation::mixColor(QString color1, QString color2, double pos)
{
    QColor c1(color1);
    QColor c2(color2);
    QString nc,rs,gs,bs;

    int r,g,b;

    r = c1.red() - ((c1.red() - c2.red())*pos);
    g = c1.green() - ((c1.green() - c2.green())*pos);
    b = c1.blue() - ((c1.blue() - c2.blue())*pos);

    rs = QString::number(r,16);
    if (rs.size() == 1)
        rs = "0"+rs;

    gs = QString::number(g,16);
    if (gs.size() == 1)
        gs = "0"+gs;

    bs = QString::number(b,16);
    if (bs.size() == 1)
        bs = "0"+bs;

    nc = "#" + rs + gs + bs;

    //qDebug() << color1 << color2 << pos << nc;

    return nc;
}

QString CAnimation::mixInt(QString int1, QString int2, double pos)
{
    QRegExp intreg("([0-9]+)");
    int i1,i2,in;
    int offi1b,offi1s;
    QString sn;

    //qDebug() << "mix int:"<<int1<<int2<<pos;

    offi1b = intreg.indexIn(int1);
    if (offi1b != -1)
    {
        i1 = intreg.cap(1).toInt();
        offi1s = intreg.cap(1).size();
    }

    if (intreg.indexIn(int2) != -1)
    {
        i2 = intreg.cap(1).toInt();
    }

    in = i1 - ((i1 - i2)*pos);
    sn = int1.left(offi1b) + QString::number(in) + int1.mid(offi1b+offi1s);

    //qDebug() << sn;

    return sn;
}

QString CAnimation::mixDouble(QString dbl1, QString dbl2, double pos)
{
    QRegExp intreg("([0-9\\.]+)");
    double i1,i2,in;
    int offi1b,offi1s;
    QString sn;

    //qDebug() << "mix double:"<<dbl1<<dbl2<<pos;

    offi1b = intreg.indexIn(dbl1);
    if (offi1b != -1)
    {
        i1 = intreg.cap(1).toDouble();
        offi1s = intreg.cap(1).size();
    }

    if (intreg.indexIn(dbl2) != -1)
    {
        i2 = intreg.cap(1).toDouble();
    }

    in = i1 - ((i1 - i2)*pos);
    sn = dbl1.left(offi1b) + QString::number(in) + dbl1.mid(offi1b+offi1s);

    //qDebug() << sn;

    return sn;
}

void CAnimation::play(bool loop)
{
    m_bLoop = loop;
    m_pTimer->start();
}

void CAnimation::stop()
{
    m_pTimer->stop();
}

void CAnimation::reset()
{
    m_iCurFrame = 0;
}

void CAnimation::playNextFrame()
{
    /*if (!m_pDoc->currentLayout())
    {
        qDebug() << "currentlayout null";
        return;
    }*/

    if (!m_Frames.contains(m_pDoc->currentLayout()->id()))
    {
        qDebug() << "No animation found for layout "<<m_pDoc->currentLayout()->id();
        m_pTimer->stop();
        return;
    }
    getFrame(m_pDoc->currentLayout()->id(),m_iCurFrame)->apply();
    //m_Frames[m_pDoc->currentLayout()->getID()][m_iCurFrame]->apply();
    m_iCurFrame++;
    if (m_iCurFrame > m_iFrameCount)
    {
        m_iCurFrame = 1;
        if (!m_bLoop)
        {
            m_pTimer->stop();
            emit finished();
        }
    }
}


CAnimFrame* CAnimation::getFrame(QString layout, int frame)
{
    if (!m_Frames.contains(layout))
        return 0;
    if (m_Frames[layout].contains(frame))
        return m_Frames[layout][frame];
    else
    {
        //genereer frame
        return generateFrame(layout,frame);
    }
}

CAnimFrame* CAnimation::generateFrame(QString layout, int frame)
{
    if (m_pGeneratedFrame)
        delete m_pGeneratedFrame;

    QMap<int,CAnimFrame*> frames = m_Frames[layout];
    QMap<CBaseObject*,QMap<QString,QString> > objpropmapend,objpropmapstart;
    QList<int> frameind = frames.keys();
    QList<CBaseObject*> objs;
    int endframe,startframe,i;
    CAnimFrame* endframep ,*animframe;
    CBaseObject* obj;
    QMap<QString,QString> endpropmap,startpropmap,propmapnew;
    QStringList props;
    QString newpropval,propval1,propval2;
    double position;

    //endframe vinden
    endframe = 0;
    for (i = 0;i<frameind.size();i++)
    {
        endframe = frameind[i];
        if (endframe > frame)
            break;
    }
    //startframe vinden
    startframe=0;
    for (i = frameind.size()-1;i>=0;i--)
    {
        startframe = frameind[i];
        if (startframe < frame)
            break;
    }

    if (startframe >= endframe && frame < endframe)
    {
        //startframe wordt 1, props baseren op huidige object props
        startframe = 1;
    }

    if (startframe == 0)
        startframe = 1;

    endframep = frames[endframe];
    objpropmapend = endframep->objectPropMap();

    objs = objpropmapend.keys();
    //qDebug() << objs.size();

    if (frames.contains(startframe))
        objpropmapstart = frames[startframe]->objectPropMap();
    else if (startframe == 1 && frames.contains(0) && frame != 1)
        objpropmapstart = frames[0]->objectPropMap();
    else if (startframe == 1 && !frames.contains(1))
    {
        if (frames.contains(0))
            delete frames[0];

        //qDebug() << "build start frames";

        animframe = new CAnimFrame(endframep->section());
        CSS::Stylesheet* sp = m_pDoc->stylesheet();

        for (i = 0;i<objs.size();i++)
        {
            obj = objs[i];
            startpropmap.clear();

            QStringList sl = sp->selector(obj)->properties();

            for (int ii=0;ii<sl.size();ii++)
            {
                startpropmap.insert(sl[ii],sp->property(obj,sl[ii])->value());
                //qDebug() << sl[ii] << sp.property(sl[ii]);
            }
            objpropmapstart.insert(obj,startpropmap);
            animframe->setPropertiesForObject(obj,startpropmap);
        }
        /*if (frames.contains(0))
            frames[0] = animframe;
        else
            frames.insert(0,animframe);
        m_Frames[layout] = frames;*/

        //we generate the frame after the current one
        startframe = frame - 1;
    }

    animframe = new CAnimFrame(endframep->section());

    //alle objecten afgaan

    for (i = 0;i<objs.size();i++)
    {
        obj = objs[i];
        propmapnew.clear();

        endpropmap = objpropmapend[obj];
        if (objpropmapstart.contains(obj))
            startpropmap = objpropmapstart[obj];
        else
            startpropmap = endpropmap;

        props = endpropmap.keys();

        //alle delta waardes mixen
        for (int pi=0;pi<props.size();pi++)
        {
            propval1 = startpropmap[props[pi]];
            propval2 = endpropmap[props[pi]];
            position = (double)(frame-startframe)/(double)(endframe-startframe-1);

            //qDebug() << "Mix prop: "<<props[pi] << propval1 << startpropmap[props[pi]];

            if (CSS::color_props.contains(props[pi]))
            {
                //newpropval = mixColor(propval1,startpropmap[props[pi]],(double)(frame-startframe)/(double)(endframe-startframe-1));
                newpropval = mixColor(propval1,propval2,position);
                propmapnew.insert(props[pi],newpropval);
            }
            else if (CSS::int_props.contains(props[pi]))
            {
                //newpropval = mixInt(propval1,startpropmap[props[pi]],(double)(frame-startframe)/(double)(endframe-startframe-1));
                newpropval = mixInt(propval1,propval2,position);
                propmapnew.insert(props[pi],newpropval);
            }
            else if (CSS::double_props.contains(props[pi]))
            {
                newpropval = mixDouble(propval1,propval2,position);
                propmapnew.insert(props[pi],newpropval);
            }
            else if (props[pi] == "color-overlay")
            {
                QStringList costart = propval1.split(QRegExp(" +"));
                QStringList coend = propval2.split(QRegExp(" +"));

                if (costart.size() == 3 && coend.size() == 3)
                {
                    //ignore the rendermode of the animation
                    newpropval = mixColor(costart[0],coend[0],position) + " " + costart[1] + " " + mixDouble(costart[2],coend[2],position);
                    propmapnew.insert(props[pi],newpropval);
                }
                else
                {
                    qDebug() << "color-overlay" << propval1 << propval2;
                }
            }
            else
            {
                propmapnew.insert(props[pi],propval2);
            }
        }
        //checken of we props van de last frame missen, zo ja dan kopieren
        props = endpropmap.keys();
        for (int pi=0;pi<props.size();pi++)
        {
            //qDebug() << "Copy prop: "<<props[pi];
            if (!propmapnew.contains(props[pi]))
                propmapnew.insert(props[pi],endpropmap[props[pi]]);
        }
        //assign propmap
        animframe->setPropertiesForObject(obj,propmapnew);

    }

    m_pGeneratedFrame = animframe;

    return animframe;
}
