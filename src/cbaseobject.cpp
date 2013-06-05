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

#include "cbaseobject.h"
#include "css/css_style.h"
#include "clayer.h"
#include "csection.h"
#include "cdocument.h"
#include "cepfview.h"

CBaseObject::CBaseObject(QString id, CLayer* layer) :
    m_sID(id), m_pLayer(layer)
{
}

CBaseObject::~CBaseObject()
{

}

void CBaseObject::setParents(CBaseObject* obj)
{
    /*CSS::Property* relative = document()->stylesheet()->property(this,"relative-to");
    if (!relative->isNull())
    {
        CBaseObject* obj = section()->objectByID(relative->toString());
        if (obj)
        {
            setParent((QObject*)obj);
            setParentItem((QGraphicsItem*)obj);
        }
    }
    else
    {
        setParent((QObject*)m_pLayer);
        setParentItem((QGraphicsItem*)m_pLayer);
    }*/
    setParent((QObject*)obj);
    //setParentItem((QGraphicsItem*)obj);
}

CLayer* CBaseObject::layer()
{
    return m_pLayer;
}

void CBaseObject::setBoundingRect(const QRectF &r)
{
    if (r != m_rRect)
    {
        prepareGeometryChange();
        m_rRect = r;
    }
}

QRectF CBaseObject::boundingRect() const
{
    return m_rRect;
}

void CBaseObject::preload()
{


}

QString CBaseObject::id()
{
    return m_sID;
}

const char* CBaseObject::objectType() const
{
    return "base";
}

void CBaseObject::layout()
{
    CBaseObject* r = relative();

    if (!r)
        return;

    prepareGeometryChange();

    //QRectF oldrect = m_rRect;
    QRectF relrect = r->boundingRect();

    CSS::Stylesheet* css = document()->stylesheet();

    setMargin(css->property(this,"margin-top")->toInt(),css->property(this,"margin-left")->toInt(),css->property(this,"margin-bottom")->toInt(),css->property(this,"margin-right")->toInt());
    //setPadding(css->property(this,"padding-top")->toInt(),css->property(this,"padding-left")->toInt(),css->property(this,"padding-bottom")->toInt(),css->property(this,"padding-right")->toInt());

    m_rRect.setHeight(css->property(this,"height")->toInt());
    m_rRect.setWidth(css->property(this,"width")->toInt());

    /*if (css->property(this,"relative-to")->toString() == "")
    {
        m_rRect.setTop(relrect.top() + r->outerHeight() + marginTop());
        m_rRect.setLeft(css->property(this,"left")->toInt() + marginLeft());

        //top/left/bottom/right negeren
    }
    else*/
    if (css->property(this,"position")->toString() != "relative")
    {
        m_rRect.setTop(relrect.top() + css->property(this,"top")->toInt());
        m_rRect.setLeft(relrect.left() + css->property(this,"left")->toInt());

        m_rRect.setHeight(css->property(this,"height")->toInt());
        m_rRect.setWidth(css->property(this,"width")->toInt());

        if (!css->property(this,"bottom")->isNull() && !css->property(this,"top")->isNull())
        {
            m_rRect.setBottom(relrect.bottom() - css->property(this,"bottom")->toInt());
            m_rRect.setTop(m_rRect.bottom() - css->property(this,"height")->toInt());
        }
        else if (!css->property(this,"bottom")->isNull())
            m_rRect.setBottom(relrect.bottom() - css->property(this,"bottom")->toInt());

        if (!css->property(this,"right")->isNull() && css->property(this,"left")->isNull())
        {
            m_rRect.setRight(relrect.right() - css->property(this,"right")->toInt());
            m_rRect.setLeft(m_rRect.right() - css->property(this,"width")->toInt());
        }
        else if (!css->property(this,"right")->isNull())
            m_rRect.setRight(relrect.right() - css->property(this,"right")->toInt());
    }


    //min/max height/width
    if (!css->property(this,"min-height")->isNull() && css->property(this,"height")->toInt() < css->property(this,"min-height")->toInt())
        m_rRect.setHeight(css->property(this,"min-height")->toInt());

    if (!css->property(this,"min-width")->isNull() && css->property(this,"width")->toInt() < css->property(this,"min-width")->toInt())
        m_rRect.setHeight(css->property(this,"min-width")->toInt());

    if (!css->property(this,"max-height")->isNull() && css->property(this,"height")->toInt() > css->property(this,"max-height")->toInt())
        m_rRect.setHeight(css->property(this,"max-height")->toInt());

    if (!css->property(this,"max-width")->isNull() && css->property(this,"width")->toInt() > css->property(this,"max-width")->toInt())
        m_rRect.setHeight(css->property(this,"max-width")->toInt());



    /*if (oldrect.size() != m_rRect.size())
    {
        //qDebug() << "size change";

        m_bNeedsRedraw = true;
    }*/

}

CBaseObject* CBaseObject::relative()
{
    return (CBaseObject*)parent();
}
QString CBaseObject::relativeID()
{
    return relative()->id();
}

QString CBaseObject::property(QString key)
{
    if (!m_Props.contains(key))
        return QString();
    return m_Props[key];
}
void CBaseObject::setProperty(QString key, QString value)
{
    if (m_Props.contains(key))
    {
        m_Props[key] = value;
        return;
    }

    m_Props.insert(key,value);
}

void CBaseObject::mouseDoubleClickEvent ( QMouseEvent * e, QPoint contentpos )
{


}
void CBaseObject::mousePressEvent(QMouseEvent *, QPoint contentpos)
{


}
void CBaseObject::mouseReleaseEvent(QMouseEvent *, QPoint contentpos)
{


}
void CBaseObject::mouseMoveEvent(QMouseEvent *, QPoint contentpos)
{


}

void CBaseObject::setCSSOverride(QString css)
{
    m_sCSSOverrides = css;
}

QString CBaseObject::cssOverrides()
{
    return m_sCSSOverrides;
}

int CBaseObject::marginTop() const
{
    return m_iMarginTop;
}
int CBaseObject::marginBottom() const
{
    return m_iMarginBottom;
}
int CBaseObject::marginLeft() const
{
    return m_iMarginLeft;
}
int CBaseObject::marginRight() const
{
    return m_iMarginRight;
}

void CBaseObject::setMargin(int top, int left, int bottom, int right)
{
    m_iMarginTop = top;
    m_iMarginBottom = bottom;
    m_iMarginLeft = left;
    m_iMarginRight = right;
}
void CBaseObject::setMarginTop(int top)
{
    m_iMarginTop = top;
}
void CBaseObject::setMarginLeft(int left)
{
    m_iMarginLeft = left;
}
void CBaseObject::setMarginBottom(int bottom)
{
    m_iMarginBottom = bottom;
}
void CBaseObject::setMarginRight(int right)
{
    m_iMarginRight = right;
}

int CBaseObject::outerHeight() const
{
    return m_rRect.height() + m_iMarginTop + m_iMarginBottom;
}

int CBaseObject::outerWidth() const
{
    return m_rRect.width() + m_iMarginLeft + m_iMarginRight;
}
/*
int CBaseObject::paddingTop() const
{


}
int CBaseObject::paddingBottom() const
{


}
int CBaseObject::paddingLeft() const
{


}
int CBaseObject::paddingRight() const
{


}

void CBaseObject::setPadding(int top, int left, int bottom, int right)
{


}
void CBaseObject::setPaddingTop(int top)
{


}
void CBaseObject::setPaddingLeft(int left)
{


}
void CBaseObject::setPaddingBottom(int bottom)
{


}
void CBaseObject::setPaddingRight(int right)
{


}

int CBaseObject::innerHeight() const
{


}
int CBaseObject::innerWidth() const
{


}*/

QStringList CBaseObject::styleClasses() const
{
    return m_StyleClasses;
}
void CBaseObject::addStyleClass(QString classname)
{
    if (m_StyleClasses.contains(classname))
        return;
    m_StyleClasses.append(classname);
}
void CBaseObject::removeStyleClass(QString classname)
{
    m_StyleClasses.removeAll(classname);
}

CSection* CBaseObject::section()
{
    return m_pLayer->section();
}

CDocument* CBaseObject::document()
{
    return section()->document();
}

QVariant CBaseObject::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemParentHasChanged)
        layout();
}
