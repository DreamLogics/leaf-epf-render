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

CBaseObject::CBaseObject(QString id, QObject *parent) :
    QGraphicsObject(parent), m_sID(id)
{
}

CBaseObject::~CBaseObject()
{

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

QString CBaseObject::getID()
{
    return m_sID;
}
const char* CBaseObject::type() const
{


}

void CBaseObject::layout(CBaseObject* relative)
{


}
CBaseObject* CBaseObject::getObjectAnchor()
{


}
QString CBaseObject::getObjectAnchorID()
{


}

QString CBaseObject::property(QString key)
{


}
void CBaseObject::setProperty(QString key, QString value)
{


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

CSS::Stylesheet* CBaseObject::style()
{


}

void CBaseObject::setCSSOverride(QString css)
{


}
QString CBaseObject::cssOverrides()
{


}

bool CBaseObject::visible()
{


}
void CBaseObject::setVisibility(bool)
{


}

QString CBaseObject::css()
{


}

int CBaseObject::marginTop() const
{


}
int CBaseObject::marginBottom() const
{


}
int CBaseObject::marginLeft() const
{


}
int CBaseObject::marginRight() const
{


}

void CBaseObject::setMargin(int top, int left, int bottom, int right)
{


}
void CBaseObject::setMarginTop(int top)
{


}
void CBaseObject::setMarginLeft(int left)
{


}
void CBaseObject::setMarginBottom(int bottom)
{


}
void CBaseObject::setMarginRight(int right)
{


}

int CBaseObject::outerHeight() const
{


}

int CBaseObject::outerWidth() const
{


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


}
void CBaseObject::addStyleClass(QString classname)
{


}
void CBaseObject::removeStyleClass(QString classname)
{


}
