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
#include <QLineEdit>
#include "ctextfieldobject.h"

QMap<QWidget*,CWidgetHandler*> g_handlers;

CWidgetHandler::CWidgetHandler(QWidget *parent) :
    QWidget(parent)
{
    setGeometry(parent->geometry());
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
    g_handlers.remove(parentWidget());
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
    QObject* s = sender();
    CTextFieldObject* tfo = dynamic_cast<CTextFieldObject*>(s);
    if (!tfo)
        return;
    QLineEdit* w = new QLineEdit(this);
    connect(w,SIGNAL(textChanged(QString)),tfo,SLOT(setValue(QString)));
    connect(tfo,SIGNAL(textChanged(QString)),w,SLOT(setText(QString)));
    w->installEventFilter(s);
    m_widgets.insert(s,w);
    w->show();
}

void CWidgetHandler::updateWidgetGeometry(QRect r)
{
    qDebug() << "setza" << r;
    QObject* s = sender();
    if (m_widgets.contains(s))
    {
        QWidget* w = m_widgets[s];
        qDebug() << "setza" << r;
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
    QWidget* widget = dynamic_cast<QWidget*>(p);
    if (widget && ev->type() == QEvent::Resize)
    {
        setGeometry(widget->geometry());
    }

    return false;
}
/*
bool CWidgetHandler::isActiveObject(QObject *p)
{
    CBaseObject* obj = dynamic_cast<CBaseObject*>(p);
    if (!obj)
        return false;

    if (obj->document()->section(m_pView->currentSection()) == obj->section())
        return true;

    return false;
}
*/
void CWidgetHandler::renderWidget(QObject *obj, QPainter *p)
{
    if (!m_widgets.contains(obj))
        return;
    QWidget* w = m_widgets[obj];
    w->render(p);
}

bool CWidgetHandler::event(QEvent *ev)
{
    return false;
}
