#include "cwidgethandler.h"
#include <QLineEdit>
#include "../../src/cepfview.h"
#include <QPainter>
#include <QApplication>
#include <QDebug>

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
    QObject* s = sender();
    QWidget* w = new QLineEdit();
    w->installEventFilter(this);
    m_widgets.insert(s,w);
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

        for (it = m_widgets.begin();it != m_widgets.end();it++)
        {
            /*if (it.value()->event(ev))
                return true;*/
            /*if (QApplication::sendEvent(it.value(),ev))
                return true;*/
        }
        return false;
    }
    else if (m_widgets.contains(p))
    {
        qDebug() << "event intercepted";
        return false;
    }
}

void CWidgetHandler::renderWidget(QObject *obj, QPainter *p)
{
    if (!m_widgets.contains(obj))
        return;
    QWidget* w = m_widgets[obj];
    w->render(p);
}
