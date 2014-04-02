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

#include "cmathmlobject.h"
#include <QPainter>
#include <QDebug>
#include <cdocument.h>
#include <cepfview.h>
#include <QDebug>
#include "mathml/qwt_mml_document.h"
#include <QApplication>
#include <QDesktopWidget>
#include <css/css_painters.h>

static int pointFromPixel(int px)
{
    int dpi=QApplication::desktop()->logicalDpiY();
    return px * 72 / dpi;
}

CBaseObject* CMathMLObjectFactory::create(QString id, CLayer *layer)
{
    return new CMathMLObject(id,layer);
}

JSMathMLObjectProxy::JSMathMLObjectProxy(CMathMLObject *obj) : JSBaseObjectProxy(obj), m_pObject(obj)
{

}

CMathMLObject::CMathMLObject(QString id, CLayer *layer) : CBaseObject(id,layer)
{

}

CMathMLObject::~CMathMLObject()
{

}

void CMathMLObject::preload()
{
    m_pMathMLDoc = new QwtMmlDocument();
}

void CMathMLObject::paint(QPainter *p)
{

}

void CMathMLObject::paintBuffered(QPainter *p)
{
    //p->drawText(0,0,"mathml "+QString::number(m_pMathMLDoc->size().width()) + "x" + QString::number(m_pMathMLDoc->size().height()));
    CSS::paintBackgroundColor(p,this);
    CSS::paintBackgroundGradient(p,this);
    CSS::paintBackgroundImage(p,this);

    m_pMathMLDoc->paint(p,QPointF(0,0));
    CSS::paintColorOverlay(p,this);
    CSS::paintBorder(p,this);
}

void CMathMLObject::layout(QRectF relativeTo,QList<CBaseObject*> updatelist)
{
    CBaseObject::layout(relativeTo,updatelist);

    if ((updatelist.size() > 0 && updatelist.contains(this)) || updatelist.size() == 0)
    {
        QString src = styleProperty("mathml-src").toString();//property("src");
        src = src.replace(QRegExp("[\"']+"),"");
        QByteArray data = document()->resource(src);
        QString xml = QString::fromUtf8(data);
        int psi = 12;
        CSS::Property sprop = styleProperty("font-size");
        CSS::Property fontprop = styleProperty("font-family");
        if (!sprop.isNull())
            psi = pointFromPixel(sprop.toInt());
        else
            psi = 12;
        m_pMathMLDoc->setBaseFontPointSize(psi);
        if (!fontprop.isNull())
            m_pMathMLDoc->setFontName(QwtMmlDocument::NormalFont,fontprop.toString());
        else
            m_pMathMLDoc->setFontName(QwtMmlDocument::NormalFont,"Arial");
        QString error;
        int l,c;
        qDebug() << "setting content";
        m_pMathMLDoc->setContent(xml,&error,&l,&c);
        qDebug() << "content set";
        if (!error.isNull())
            qDebug() << error;
        m_pMathMLDoc->layout();
        qDebug() << "layout";

        QRectF r = boundingRect();
        if (r.height() == 0)
            r.setHeight(m_pMathMLDoc->size().height());
        if (r.width() == 0)
            r.setWidth(m_pMathMLDoc->size().width());
        setBoundingRect(r);
    }
}


QObject* CMathMLObject::jsProxy() const
{
    return m_pJSProxy;
}

