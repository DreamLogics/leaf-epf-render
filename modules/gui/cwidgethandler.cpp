#include "cwidgethandler.h"
#include <QLineEdit>
#include "../../src/cepfview.h"
#include <QPainter>
#include <QApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QKeyEvent>
#include "../../src/cbaseobject.h"
#include "../../src/cdocument.h"
//#include "clineedit.h"

QMap<CEPFView*,CWidgetHandler*> g_handlers;

CWidgetHandler::CWidgetHandler(CEPFView* p, QObject *parent) :
    QObject(parent), m_pView(p)
{
}

CWidgetHandler* CWidgetHandler::get(CEPFView* p)
{
    if (g_handlers.contains(p))
        return g_handlers[p];
    CWidgetHandler* h = new CWidgetHandler(p);
    p->installEventFilter(h);
    g_handlers.insert(p,h);
    return h;
}

void CWidgetHandler::destroy()
{
    QMap<QObject*,QWidget*>::iterator it;

    for (it = m_widgets.begin();it != m_widgets.end();it++)
    {
        it.value()->deleteLater();
    }
    g_handlers.remove(m_pView);
    deleteLater();
}

void CWidgetHandler::createTextField()
{
    /*QObject* s = sender();
    CLineEdit* w = new CLineEdit(m_pView);
    w->installEventFilter(this);
    connect(w,SIGNAL(redrawNeeded()),m_pView,SLOT(updateView()));
    w->show();
    m_widgets.insert(s,w);*/
}

void CWidgetHandler::updateWidgetGeometry(QRect r)
{
    QObject* s = sender();
    if (m_widgets.contains(s))
    {
        QWidget* w = m_widgets[s];
        w->setGeometry(r);
    }
}

void CWidgetHandler::showWidget()
{
    QObject* s = sender();
    if (m_widgets.contains(s))
    {
        QWidget* w = m_widgets[s];
        w->show();
    }
}

void CWidgetHandler::hideWidget()
{
    QObject* s = sender();
    if (m_widgets.contains(s))
    {
        QWidget* w = m_widgets[s];
        w->hide();
    }
}

void CWidgetHandler::destroyWidget()
{
    QObject* s = sender();
    if (m_widgets.contains(s))
    {
        QWidget* w = m_widgets[s];
        w->deleteLater();
        m_widgets.remove(s);
    }
}

bool CWidgetHandler::eventFilter(QObject *p, QEvent *ev)
{
    if (p == m_pView)
    {
        QMap<QObject*,QWidget*>::iterator it;
        QRect r;

        for (it = m_widgets.begin();it != m_widgets.end();it++)
        {
            if (!isActiveObject(it.key()))
                continue;
            CBaseObject* obj = dynamic_cast<CBaseObject*>(it.key());
            r = obj->boundingRect().toRect();
            /*if (it.value()->event(ev))
                return true;*/
            /*if (QApplication::sendEvent(it.value(),ev))
                return true;*/
            if (ev->type() == QEvent::MouseButtonPress || ev->type() == QEvent::MouseButtonRelease
                    || ev->type() == QEvent::MouseMove || ev->type() == QEvent::MouseButtonDblClick)
            {
                QMouseEvent* mev = dynamic_cast<QMouseEvent*>(ev);
                //qDebug() << "mouse ev" << mev->pos() << r;
                if (mev && r.contains(mev->pos()))
                {
                    qDebug() << "mouse ev pos";
                    QPoint lpos(mev->pos().x() - r.x(),mev->pos().y() - r.y());
                    QMouseEvent* nme = new QMouseEvent(ev->type(),lpos,mev->button(),mev->buttons(),mev->modifiers());
                    QApplication::sendEvent(it.value(),nme);
                    delete nme;
                    //it.value()->setFocus();
                }
            }
            else if ((ev->type() == QEvent::KeyPress || QEvent::KeyRelease) && it.value()->hasFocus())
            {
                QKeyEvent* kev = dynamic_cast<QKeyEvent*>(ev);
                if (kev)
                {
                    QKeyEvent* nke = new QKeyEvent(ev->type(),kev->key(),kev->modifiers(),kev->text(),kev->isAutoRepeat(),kev->count());
                    QApplication::sendEvent(it.value(),nke);
                    delete nke;
                }
            }
        }
        return false;
    }
    else if (dynamic_cast<QWidget*>(p))
    {
        qDebug() << "event intercepted";
        return false;
    }
    return false;
}

bool CWidgetHandler::isActiveObject(QObject *p)
{
    CBaseObject* obj = dynamic_cast<CBaseObject*>(p);
    if (!obj)
        return false;

    if (obj->document()->section(m_pView->currentSection()) == obj->section())
        return true;

    return false;
}

void CWidgetHandler::renderWidget(QObject *obj, QPainter *p)
{
    if (!m_widgets.contains(obj))
        return;
    QWidget* w = m_widgets[obj];
    w->render(p);
}
