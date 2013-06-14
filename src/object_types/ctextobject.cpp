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
#include "cdocument.h"
#include "css/css_style.h"
#include "css/css_painters.h"
#include <QUrl>
#include <QDebug>
#include <QTextCursor>
#include <QAbstractTextDocumentLayout>
#include <QTextDocumentFragment>

CBaseObject* CTextObjectFactory::create(QString id, CLayer *layer)
{
    return new CTextObject(id,layer);
}

CTextObject::CTextObject(QString id, CLayer* layer) :
    CBaseObject(id,layer)
{
    m_pTextDoc = 0;
}

void CTextObject::preload()
{
    m_pTextDoc = new QTextDocument();
}

void CTextObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    CSS::Stylesheet* css = document()->stylesheet();

    if (!css->property(this,"background-color")->isNull())
    {
        CSS::paintBackgroundColor(painter,boundingRect(),css->property(this,"background-color")->toString());
    }

    if (!css->property(this,"background-image")->isNull())
    {
        QString size;

        if (!css->property(this,"background-size")->isNull())
            size = css->property(this,"background-size")->toString();
        else if (!css->property(this,"background-image-size")->isNull())
            size = css->property(this,"background-image-size")->toString();

        CSS::paintBackgroundImage(painter,boundingRect(),size,css->property(this,"background-image")->toString(),document());
    }

    if (!css->property(this,"color-overlay")->isNull())
    {
        QRegExp cov("(#[0-9a-fA-F]{3,6}) +([a-zA-Z]+) +([0-9\\.]+)");
        if (cov.indexIn(css->property(this,"color-overlay")->toString()) != -1)
        {
            CSS::paintColorOverlay(painter,boundingRect(),cov.cap(1),CSS::renderModeFromString(cov.cap(2)),cov.cap(3).toDouble());
        }
    }

    if (!m_pTextDoc)
        return;

    painter->translate(boundingRect().x(),boundingRect().y()+m_iRenderOffset);
    m_pTextDoc->drawContents(painter);
    painter->translate(-boundingRect().x(),-boundingRect().y()+m_iRenderOffset);

    /*painter->setFont(QFont("sans-serif",10));
    painter->setPen(QColor("red"));
    painter->drawText(boundingRect().left()+16,boundingRect().bottom()-16,id());

    painter->drawRect(boundingRect());*/
}

void CTextObject::layout(QRectF relrect)
{
    QRectF oldrect = boundingRect();
    CBaseObject::layout(relrect);

    if (!m_pTextDoc)
    {
        qDebug() << "QTextDocument not initialized.";
        return;
    }

    if (oldrect.size() == boundingRect().size())
        return;

    //QFont font("Sans",14);
    //m_pTextDoc->setDefaultFont(font);

    QString html;

    //html maken
    QString src = document()->stylesheet()->property(this,"text-source")->toString();//property("src");
    src = src.replace(QRegExp("[\"']+"),"");

    if (src != "") //als we geen src hebben nemen we de overflow
    {
        QByteArray htmldata = document()->resource(src);

        html = "<html><head><link rel='stylesheet' type='text/css' href='format.css'>";
        html += "</head><body id=\""+id()+"\">";
        html += QString::fromUtf8(htmldata.constData(),htmldata.size());
        html += "</body></html>";

        //qDebug() << html;
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
                //qDebug() << html;
            }
            else
                return;
        }
        else
            return;

    }

    QRectF r = boundingRect();
    m_pTextDoc->addResource(QTextDocument::StyleSheetResource, QUrl( "format.css" ), css());
    //qDebug() << html << css;

    //hebben we geen hoogte? Dan gewoon vullen
    int height = r.height();
    int width = r.width();

    m_iRenderOffset = 0;

    //qDebug() << "text object width" << width << container_width;

    if (width == 0)
    {
        qDebug() << "text object with no width";
        return;
    }

    if (height == 0)
    {
        if (html.size() > 0)
        {
            m_pTextDoc->setTextWidth(width);
            m_pTextDoc->setHtml(html);
            //qDebug() << "hooooogg text" << m_pTextDoc->size().height() << r.height();
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
            //qDebug() << pos;
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
            QString valign = document()->stylesheet()->property(this,"v-align")->toString();
            if (valign != "top")
            {
                m_pTextDoc->setTextWidth(width);
                int dh = height - m_pTextDoc->size().height();
                if (valign == "center")
                    m_iRenderOffset = dh/2;
                else
                    m_iRenderOffset = dh;
            }

            m_sOverflow = "";
        }

    }

    int f=-1;
    QRegExp imgfinder("< *img[^>]*>");
    QRegExp srcfinder("src *= *\"([^\"]+)\"");
    QByteArray data;
    QString fn;

    while (1)
    {
        f = html.indexOf(imgfinder,f+1);
        if (f == -1)
            break;
        srcfinder.indexIn(imgfinder.cap());
        fn = srcfinder.cap(1);
        fn = QUrl::fromPercentEncoding(fn.toUtf8());
        data = document()->resource(fn);
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
    //qDebug() << m_pTextDoc->toHtml();
}


QString CTextObject::overflow()
{
    //TODO: overflow wissen om geheugen te besparen
    return m_sOverflow;
}

QString CTextObject::css()
{
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

    CSS::Selector* csel;

    QStringList proplist;

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

                    //qDebug() << proplist;

                    for (int n=0;n<proplist.size();n++)
                    {
                        newcss += proplist[n] + ": " + csel->property(proplist[n])->toString() + ";";
                    }

                    newcss += "}";
                }
            }
            else
                sel += s[i];

        }
    }

    qDebug() << newcss;

    return newcss;

}
