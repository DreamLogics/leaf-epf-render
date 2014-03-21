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

#include "ctextfieldobject.h"
#include <QPainter>
#include <QDebug>
#include "../../src/cdocument.h"
#include "../../src/cepfview.h"
#include <QLayout>
#include "cwidgethandler.h"
#include <QDebug>
#include <QInputMethod>
#include <QApplication>
#include "../../src/idevice.h"
#include <QMenu>
#include "textinputview.h"
#include <QLineEdit>
#include <QKeyEvent>

CBaseObject* CTextFieldObjectFactory::create(QString id, CLayer *layer)
{
    return new CTextFieldObject(id,layer);
}

JSTextFieldObjectProxy::JSTextFieldObjectProxy(CTextFieldObject *obj) : JSBaseObjectProxy(obj), m_pObject(obj)
{

}

void JSTextFieldObjectProxy::setText(QString str)
{
    m_pObject->setText(str);
}

void JSTextFieldObjectProxy::_textChanged(QString str)
{
    emit textChanged(str);
}

CTextFieldObject::CTextFieldObject(QString id, CLayer *layer) : CBaseObject(id,layer)
{
    m_bHasFocus = false;
    //m_bEmbedded = false;
}

CTextFieldObject::~CTextFieldObject()
{
    emit destroyWidget();
}

void CTextFieldObject::preload()
{
    CWidgetHandler* h = CWidgetHandler::get(document()->renderview());
    connect(this,SIGNAL(createTextField()),h,SLOT(createTextField()));
    connect(this,SIGNAL(updateWidgetGeometry(QRect)),h,SLOT(updateWidgetGeometry(QRect)));
    connect(this,SIGNAL(showWidget()),h,SLOT(showWidget()));
    connect(this,SIGNAL(hideWidget()),h,SLOT(hideWidget()));
    connect(this,SIGNAL(destroyWidget()),h,SLOT(destroyWidget()));

    emit createTextField();
}

void CTextFieldObject::paint(QPainter *painter)
{

}

void CTextFieldObject::paintBuffered(QPainter *p)
{
    return;
    //CWidgetHandler::get(document()->renderview())->renderWidget(this,p);
    //TODO: multithreaded
    QRectF r(0,0,boundingRect().width(),boundingRect().height());
    p->fillRect(r,QColor("white"));
    if (m_bHasFocus)
    {
        QPen pen("#ff0000");
        p->setPen(pen);
        p->drawRect(0,0,r.width(),r.height()-1);
    }
    QFont font;
    QRectF textbox(4,4,r.width()-8,r.height()-8);
    font.setPixelSize(textbox.height());
    p->setFont(font);
    p->setPen(QPen("#000000"));
    p->drawText(textbox,m_sValue);
}

void CTextFieldObject::layout(QRectF relativeTo,QList<CBaseObject*> updatelist)
{
    CBaseObject::layout(relativeTo,updatelist);
    qDebug() << "setzb" << boundingRect();
    //if (updatelist.contains(this))
        emit updateWidgetGeometry(boundingRect().toRect());
}

bool CTextFieldObject::eventFilter(QObject *p, QEvent *ev)
{
    QLineEdit* le = dynamic_cast<QLineEdit*>(p);
    if (le && ev->type() == QEvent::KeyPress)
    {
        QKeyEvent* kev = dynamic_cast<QKeyEvent*>(ev);
        if (kev->key() == Qt::Key_Return)
        {
            QApplication::inputMethod()->hide();
        }
    }
    return false;
}

void CTextFieldObject::setValue(QString s)
{
    m_sValue = s;
}

QObject* CTextFieldObject::jsProxy() const
{
    return m_pJSProxy;
}

void CTextFieldObject::setText(QString str)
{
    m_pJSProxy->_textChanged(str);
}

/*
void CTextFieldObject::mouseDoubleClickEvent ( QPoint pos )
{

}

void CTextFieldObject::mousePressEvent( QPoint pos )
{
    m_HoldTimer.start();
    if (section()->hasControl(this))
    {
        if (!boundingRect().contains(pos))
        {
            section()->releaseControl(this);
            m_bHasFocus = false;
#if defined(IOS) || defined(ANDROID)
            emit getLine();
#else
            QApplication::inputMethod()->hide();
#endif
            document()->updateRenderView();
            section()->mousePressEvent(pos.x(),pos.y());
        }
    }
}

void CTextFieldObject::mouseReleaseEvent( QPoint pos )
{
    qint64 t = m_HoldTimer.elapsed();
    if (Device::currentDevice()->deviceFlags() & IDevice::dfTouchScreen)
    {
        if (t >= 1000)
        {

        }
    }
#if defined(IOS) || defined(ANDROID)
            emit getLine();
#else
            QApplication::inputMethod()->show();
#endif
    if (!section()->hasControl(this))
    {
        section()->takeControl(this);
        m_bHasFocus = true;

        document()->updateRenderView();
    }
}

void CTextFieldObject::mouseMoveEvent( QPoint pos )
{

}

void CTextFieldObject::keyPressEvent(int key, QString val)
{
    //qDebug()() << "press" << key << val;
#if defined(IOS) || defined(ANDROID)
     return;
#endif
    if (key == Qt::Key_Return)
    {
        QApplication::inputMethod()->hide();
    }
    else if (key == Qt::Key_Backspace)
    {
        if (m_sValue.size() > 0)
            m_sValue = m_sValue.left(m_sValue.size()-1);
    }
    else
    {
        if (val.size() > 0)
        {
            QChar c = val[0];
            if (c.isPrint())
                m_sValue += c;
        }
    }
    document()->updateRenderView();
}

void CTextFieldObject::keyReleaseEvent(int key, QString val)
{
    //qDebug()() << "release" << key;
}*/
/*
void CTextFieldObject::setTextLine(QString str)
{
    m_sValue = str;
    section()->releaseControl(this);
    m_bHasFocus = false;
    document()->updateRenderView();
}
*/
