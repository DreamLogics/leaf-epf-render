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

#include "ctextobject.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QTextDocument>
#include "../cdocument.h"
#include "../css/css_style.h"
#include "../css/css_painters.h"
#include <QUrl>
#include <QDebug>
#include <QTextCursor>
#include <QAbstractTextDocumentLayout>
#include <QTextDocumentFragment>
#include <QApplication>
#include <QDesktopWidget>
//#include <QSvgGenerator>
//#include <QSvgRenderer>
#include <QDataStream>
#include "../csection.h"
#include "../clayout.h"
#include "../clayer.h"

CBaseObject* CTextObjectFactory::create(QString id, CLayer *layer)
{
    return new CTextObject(id,layer);
}

CTextObject::CTextObject(QString id, CLayer* layer) :
    CBaseObject(id,layer)
{
    m_pTextDoc = 0;
    m_iRenderOffset = 0;
    m_bTextChanged = true;
}

void CTextObject::preload()
{
    m_pTextDoc = new QTextDocument();
}

void CTextObject::paint(QPainter *painter)
{
    CSS::Stylesheet* css = document()->stylesheet();
    QRectF r = boundingRect();
    r.moveTop(0);
    r.moveLeft(0);

    //painter->fillRect(r,QColor("white"));
    //qDebug() << "text obj on pos" << boundingRect();

    if (!css->property(this,"background-color").isNull())
    {
        CSS::paintBackgroundColor(painter,r,css->property(this,"background-color").toString());
    }
    /*else
        CSS::paintBackgroundColor(painter,r,"#ffffff");*/

    if (!css->property(this,"background-image").isNull())
    {
        QString size;

        if (!css->property(this,"background-size").isNull())
            size = css->property(this,"background-size").toString();
        else if (!css->property(this,"background-image-size").isNull())
            size = css->property(this,"background-image-size").toString();

        CSS::paintBackgroundImage(painter,r,size,css->property(this,"background-image").toString(),document());
    }

    if (!m_pTextDoc)
        return;



    painter->translate(0,m_iRenderOffset);
    m_pTextDoc->drawContents(painter);
    painter->translate(0,-m_iRenderOffset);

    /*painter->setFont(QFont("sans-serif",10));
    painter->setPen(QColor("red"));
    painter->drawText(boundingRect().left()+16,boundingRect().bottom()-16,id());

    painter->drawRect(boundingRect());*/


    CSS::paintColorOverlay(painter,this);

}

void CTextObject::layout(QRectF relrect, QList<CBaseObject*> updatelist)
{

    QRectF oldrect = boundingRect();
    CBaseObject::layout(relrect,updatelist);

    if ((updatelist.size() > 0 && updatelist.contains(this)) || updatelist.size() == 0)
    {

        if (!m_pTextDoc)
        {
            //qDebug() << "QTextDocument not initialized.";
            return;
        }

        if (oldrect.size() == boundingRect().size() && !m_bTextChanged)
            return;

        m_bTextChanged = false;
        //QFont font("Sans",14);
        //m_pTextDoc->setDefaultFont(font);

        QString html;

        if (!m_sOverrideHTML.isNull())
        {
            html = "<html><head><link rel='stylesheet' type='text/css' href='format.css'>";
            html += "</head><body id=\""+id()+"\">";
            html += m_sOverrideHTML;
            html += "</body></html>";
        }
        else
        {
            //html maken
            QString src = document()->stylesheet()->property(this,"text-source").toString();//property("src");
            src = src.replace(QRegExp("[\"']+"),"");

            if (src != "") //als we geen src hebben nemen we de overflow
            {
                QByteArray htmldata = document()->resource(src);
                QString body = QString::fromUtf8(htmldata.constData(),htmldata.size());;
                QRegExp bodyfinder("< *(body|BODY)[^>]*>");
                QRegExp bodyendfinder("< */ *(body|BODY)[^>]*>");

                int bodystart = body.indexOf(bodyfinder);
                if (bodystart != -1)
                    body = body.mid(bodystart + bodyfinder.cap(0).size());

                int bodyend = body.indexOf(bodyendfinder);
                if (bodyend != -1)
                    body = body.left(bodyend);


                html = "<html><head><link rel='stylesheet' type='text/css' href='format.css'>";
                html += "</head><body id=\""+id()+"\">";
                html += body;
                html += "</body></html>";

                //qDebug()() << html;
            }
            else
            {
                //overflow van de parent nemen
                QString parent = property("parent");
                if (parent != "")
                {
                    CTextObject* tpar = dynamic_cast<CTextObject*>(section()->objectByID(parent));
                    if (tpar)
                    {
                        html = "<html><head><link rel='stylesheet' type='text/css' href='format.css'>";
                        html += "</head><body id=\""+id()+"\">";
                        html += tpar->overflow();
                        html += "</body></html>";
                        //qDebug()() << html;
                    }
                    else
                        return;
                }
                else
                    return;

            }
        }

        QRectF r = boundingRect();
        m_pTextDoc->addResource(QTextDocument::StyleSheetResource, QUrl( "format.css" ), css());
        //qDebug()() << html << css;

        //hebben we geen hoogte? Dan gewoon vullen
        int height = r.height();
        int width = r.width();

        m_iRenderOffset = 0;

        //qDebug()() << "text object width" << width << container_width;

        if (width == 0)
        {
            //qDebug() << "text object with no width";
            return;
        }

        if (height == 0)
        {
            if (html.size() > 0)
            {
                m_pTextDoc->setTextWidth(width);
                m_pTextDoc->setHtml(html);
                //qDebug()() << "hooooogg text" << m_pTextDoc->size().height() << r.height();
                r.setHeight(m_pTextDoc->size().height());
                setBoundingRect(r);
            }
            /*else
            {
                r.setHeight(0);
                setRect(r);
            }*/
        }
        else
        {
            QSize s(width,height);
            m_pTextDoc->setPageSize(s);
            m_pTextDoc->setHtml(html);

            if (m_pTextDoc->pageCount() > 1)
            {
                //we hebben meer content, zet dit in de overflow zodat het gelinkte text object deze kan inladen
                QTextCursor tc(m_pTextDoc);
                QPoint pp(0,height);
                int pos = m_pTextDoc->documentLayout()->hitTest(pp,Qt::FuzzyHit);
                //qDebug()() << pos;
                tc.setPosition(pos);
                tc.setPosition(m_pTextDoc->characterCount()-1,QTextCursor::KeepAnchor);
                QTextDocumentFragment tdf = tc.selection();
                m_sOverflow = tdf.toHtml();

                //strip de troep
                int fb,fe;
                fb = m_sOverflow.indexOf("<body>");
                if (fb != -1)
                {
                    fb += 6;
                    fe = m_sOverflow.indexOf("</body>");
                    m_sOverflow = m_sOverflow.mid(fb,fe-fb);
                    m_sOverflow = m_sOverflow.replace(QRegExp("style *= *\"[^\"]*\""),"");
                    m_sOverflow = m_sOverflow.replace(QRegExp("<[ \\/]*span *>"),"");
                    m_sOverflow = m_sOverflow.replace("<!--StartFragment-->","");
                    m_sOverflow = m_sOverflow.replace("<!--EndFragment-->","");
                }
            }
            else
            {
                //check v-align style
                CSS::Property vap = document()->stylesheet()->property(this,"v-align");
                if (!vap.isNull())
                {
                    QString valign = vap.toString();
                    if (valign != "top")
                    {
                        m_pTextDoc->setTextWidth(width);
                        int dh = height - m_pTextDoc->size().height();
                        if (valign == "center")
                            m_iRenderOffset = dh/2;
                        else
                            m_iRenderOffset = dh;
                    }
                }

                m_sOverflow = "";
            }

        }

        int f=-1;
        QRegExp imgfinder("< *img[^>]*>");
        QRegExp srcfinder("src *= *\"([^\"]+)\"");
        QByteArray data;
        QString fn,rn;
        QStringList sl;

        while (1)
        {
            f = html.indexOf(imgfinder,f+1);
            if (f == -1)
                break;
            srcfinder.indexIn(imgfinder.cap());
            fn = srcfinder.cap(1);
            fn = QUrl::fromPercentEncoding(fn.toUtf8());

            sl = fn.split("/");
            if (sl.size() > 1)
                rn = sl[sl.size()-1];
            else
                rn = fn;

            data = document()->resource(rn);
            if (data.size() > 0)
            {
                m_pTextDoc->addResource(QTextDocument::ImageResource, QUrl( fn ), data);
            }
        }

        //TODO: dit moet beter kunnen toch?

        /*m_AnchorIndex.clear();

        QTextCursor c(m_pTextDoc);
        QStringList an;
        //QRegExp idfind("id *= *\"([^\"]+)\"");
        for (int i=0;i<m_pTextDoc->characterCount();i++)
        {
            c.setPosition(i);
            c.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor);

            an = c.charFormat().anchorNames();

            if (an.size() > 0 && !m_AnchorIndex.contains(an[an.size()-1]))
            {
                m_AnchorIndex.insert(an[an.size()-1],i);
            }
        }*/

        //buffer

        //if (m_pmRendered)
        //    delete m_pmRendered;

        //m_pmRendered = new QPixmap(m_pTextDoc->size());
        /*m_pmRendered = QPixmap(m_pTextDoc->size().toSize());

        m_pmRendered.fill(Qt::transparent);

        QPainter p;
        p.begin(&m_pmRendered);
        m_pTextDoc->drawContents(&p);
        p.end();*/
        //qDebug()() << m_pTextDoc->toHtml();

        buffer();
    }
}


QString CTextObject::overflow()
{
    //TODO: overflow wissen om geheugen te besparen
    return m_sOverflow;
}

QString CTextObject::css()
{
    CSS::Stylesheet* stylesheet = document()->stylesheet();
    CSS::Property csssrc = stylesheet->property(this,"css-source");
    if (!csssrc.isNull())
    {
        QByteArray data = document()->resource(csssrc.toString());
        if (data.size() > 0)
            return QString::fromUtf8(data.constData(),data.size());
    }


    QString s = document()->currentLayout()->css();
    QString newcss;
    QString sel;
    bool bInSel=false;
    QRegExp outerspaces("(^ +| +$)");

    //remove all css not relevant to the text object
    QRegExp newlines("[\n\r]+");
    QRegExp tabs("[\t]+");
    s.replace(newlines,"");
    s.replace(tabs," ");

    s = CSS::Stylesheet::parseGroup(s);

    CSS::Selector* csel;

    QStringList proplist;

    QString propval;

    for (int i=0;i<s.size();i++)
    {
        if (bInSel)
        {
            if (s[i] == '}')
            {
                sel = "";
                bInSel = false;
            }
        }
        else
        {
            if (s[i] == '{')
            {
                bInSel = true;
                sel.replace(outerspaces,"");
                qDebug() << sel << "#"+section()->id()+"::"+id();
                if ((sel.startsWith("#"+section()->id()+"::"+id()) || sel.startsWith("#"+section()->id()+":"+layer()->id()+":"+id()))
                        && !(sel == "#"+section()->id()+"::"+id() || sel == "#"+section()->id()+":"+layer()->id()+":"+id()))
                {
                    csel = document()->stylesheet()->selector(sel);
                    proplist = csel->properties();

                    sel.replace("#"+section()->id()+"::"+id(),"");
                    sel.replace("#"+section()->id()+":"+layer()->id()+":"+id(),"");
                    newcss += sel + "{";

                    //qDebug()() << proplist;

                    for (int n=0;n<proplist.size();n++)
                    {
                        propval = csel->property(proplist[n]).toString();
                        if (proplist[n] == "font-size" && propval.endsWith("px"))
                            propval = QString::number(pointFromPixel(propval.left(propval.size()-2).toInt()))+"pt";
                        newcss += proplist[n] + ": " + propval + ";";
                        //qDebug()() << proplist[n] << propval;
                    }

                    newcss += "}";
                }
            }
            else
                sel += s[i];

        }
    }

    //qDebug() << "newcss" << newcss;

    return newcss;

}

int CTextObject::pointFromPixel(int px)
{
    int dpi=QApplication::desktop()->logicalDpiY();
    return px * 72 / dpi;
}

void CTextObject::onEPFEvent(EPFEvent *ev)
{
    CBaseObject::onEPFEvent(ev);
    if (ev->event() == "setText")
    {
        //qDebug()() << "set text shizzle" << ev->parameter(0);
        setText(ev->parameter(0));
        //document()->updateRenderView();
    }
}

void CTextObject::setText(QString html)
{
    m_sOverrideHTML = html;
    m_bTextChanged = true;
    section()->layout();
}

/*
void CTextObject::buffer()
{
    QDataStream ds(&m_baSVG,QIODevice::WriteOnly);
    QSvgGenerator svg;
    svg.setOutputDevice(ds.device());
    QPainter p;
    p.begin(&svg);
    paint(&p);
    p.end();
}

void CTextObject::paintBuffered(QPainter *p)
{
    QSvgRenderer svg(m_baSVG);
    svg.render(p);
}
*/
bool CTextObject::useDevicePixels() const
{
    return true;
}

JSTextObjectProxy::JSTextObjectProxy(CTextObject *obj) : JSBaseObjectProxy(obj), m_pTextObject(obj)
{

}

void JSTextObjectProxy::setText(QString html)
{
    m_pTextObject->setText(html);
}
