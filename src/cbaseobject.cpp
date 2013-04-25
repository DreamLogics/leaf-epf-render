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
