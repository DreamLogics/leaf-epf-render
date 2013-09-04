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

#include "cepfview.h"
#include "cdocument.h"
//#include "csectionview.h"
#include "csection.h"
#include "coverlay.h"
#include "clayer.h"
#include "cbaseobject.h"
#include <QGraphicsScene>
#include <QDebug>
#include <QTimer>
#include <QEvent>
#include <QScrollBar>
#include <QMouseEvent>
#include <QRectF>
#include <QWheelEvent>
#include <QTouchEvent>

CEPFView::CEPFView()
{
    //m_pDocScene = new QGraphicsScene();
    //setScene(m_pDocScene);
    m_bIsLoading = true;
    m_iRenderDot = 0;
    m_iPreviousSection = -1;
    m_dTransition = 0.0;
    m_pResizeTimer = new QTimer();
    m_pActiveOverlay = 0;

    m_pResizeTimer->setInterval(1000);

    m_bInResize = false;

    m_pDocument = 0;

    connect(m_pResizeTimer,SIGNAL(timeout()),this,SLOT(resizeDone()));
    //setAlignment(Qt::AlignLeft | Qt::AlignTop);

    //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void CEPFView::setDocument(CDocument *doc)
{
    /*if (m_pDocument)
    {
        //m_pDocument->clearBuffers();
        emit clearBuffers();
        disconnect(this,SIGNAL(updateRendered(QRectF)),0,0);
    }*/

    m_pDocument = doc;
    int i,first=-1;

    qDebug() << "setting document";

    /*for (i=0;i<m_SectionViews.size();i++)
        delete m_SectionViews[i];*/

    //m_SectionViews.clear();
    m_SectionIndex.clear();

    m_iCurrentSection = -1;

    //CSectionView* view;
    CSection* section;

    doc->setRenderview(this);
    //doc->layout(height(),width());

    for (i=0;i<doc->sectionCount();i++)
    {
        section = doc->section(i);

        //view = new CSectionView(section);
        //m_SectionViews.append(view);
        m_SectionIndex.append(section->id());

        //m_pDocScene->addItem(view);

        //connect(this,SIGNAL(scrollSection(int,int)),section,SLOT(scrollSection(int,int)));
        connect(this,SIGNAL(updateRendered(QRectF)),section,SLOT(updateRendered(QRectF)));


        //view->setGeometry(width(),0,width(),height());
        //view->setX(width());

        if (first == -1 && !section->isHidden())
            first = i;
    }

    m_iCurrentSection = first;
    m_bIsLoading = true;
    QTimer::singleShot(300,this,SLOT(updateDot()));

    disconnect(this,SIGNAL(loadDocument()),0,0);
    disconnect(this,SIGNAL(layout(int,int)),0,0);
    connect(this,SIGNAL(loadDocument(int,int,int)),doc,SLOT(load(int,int,int)));
    connect(this,SIGNAL(layout(int,int,int,bool)),doc,SLOT(layout(int,int,int,bool)));
    connect(doc,SIGNAL(finishedLoading()),this,SLOT(ready()));
    connect(doc,SIGNAL(_updateRenderView()),this,SLOT(update()));
    connect(doc,SIGNAL(setSection(int)),this,SLOT(setSection(int)));
    connect(this,SIGNAL(clearBuffers()),doc,SLOT(clearBuffers()));

    emit loadDocument(height(),width(),first);
}

void CEPFView::setSection(int index)
{
    if (index < 0 || index >= m_pDocument->sectionCount())
        return;

    /*CSectionView* view = m_SectionViews[index];
    view->setX(0);

    if (m_iCurrentSection > -1)
    {
        CSectionView* curview = m_SectionViews[m_iCurrentSection];
        curview->setX(width());
    }*/
    CSection* s = m_pDocument->section(index);

    disconnect(this,SIGNAL(mouseDoubleClickEvent(int,int)),0,0);
    disconnect(this,SIGNAL(mouseMoveEvent(int,int)),0,0);
    disconnect(this,SIGNAL(mousePressEvent(int,int)),0,0);
    disconnect(this,SIGNAL(mouseReleaseEvent(int,int)),0,0);

    connect(this,SIGNAL(mouseDoubleClickEvent(int,int)),s,SLOT(mouseDoubleClickEvent(int,int)));
    connect(this,SIGNAL(mouseMoveEvent(int,int)),s,SLOT(mouseMoveEvent(int,int)));
    connect(this,SIGNAL(mousePressEvent(int,int)),s,SLOT(mousePressEvent(int,int)));
    connect(this,SIGNAL(mouseReleaseEvent(int,int)),s,SLOT(mouseReleaseEvent(int,int)));

    m_iPreviousSection = m_iCurrentSection;
    m_iCurrentSection = index;
    if (m_iPreviousSection == m_iCurrentSection)
        m_iPreviousSection = -1;

    m_TransFx = s->transitionType();

    if (m_TransFx != CSection::NoneFx && m_iPreviousSection != -1)
    {
        m_dTransition = 1.0;
        transitionAnim();
    }

    update();
}

void CEPFView::setSection(QString id)
{
    if (!m_SectionIndex.contains(id))
        return;

    setSection(m_SectionIndex.indexOf(id));
}

void CEPFView::nextSection()
{
    int i;

    if (m_iCurrentSection >= m_pDocument->sectionCount())
        return;

    for (i=m_iCurrentSection+1;i<m_pDocument->sectionCount();i++)
    {
        if (!m_pDocument->section(i)->isHidden())
        {
            setSection(i);
            break;
        }
    }

}

void CEPFView::previousSection()
{
    int i;

    if (m_iCurrentSection <= 0)
        return;

    for (i=m_iCurrentSection-1;i<m_pDocument->sectionCount();i--)
    {
        if (!m_pDocument->section(i)->isHidden())
        {
            setSection(i);
            break;
        }
    }
}

int CEPFView::currentSection()
{
    return m_iCurrentSection;
}

void CEPFView::ready()
{
    m_bIsLoading = false;
    setSection(m_iCurrentSection);
    //setScene(m_pDocument->section(0));
    //ensureVisible(0,0,1,1);
}

void CEPFView::tocSection()
{
    QString tocs = m_pDocument->property("toc");
    int toci = indexForSection(tocs);
    if (toci == m_iCurrentSection)
    {
        setSection(m_iPreviousSection);
        return;
    }
    if (toci >= 0 < toci < m_pDocument->sectionCount())
        setSection(toci);
}

COverlay* CEPFView::activeOverlay()
{
    COverlay* overlay;
    COverlay* overlayActive=0;

    for (int i=0;i<m_pDocument->overlayCount();i++)
    {
        overlay = m_pDocument->overlay(i);
        if (overlay->active())
            overlayActive = overlay;
    }

    if (overlayActive)
    {
        if (m_pActiveOverlay != overlayActive)
        {
            if (m_pActiveOverlay)
            {
                disconnect(this,SIGNAL(mouseDoubleClickEventOverlay(int,int)),0,0);
                disconnect(this,SIGNAL(mouseMoveEventOverlay(int,int)),0,0);
                disconnect(this,SIGNAL(mousePressEventOverlay(int,int)),0,0);
                disconnect(this,SIGNAL(mouseReleaseEventOverlay(int,int)),0,0);
            }

            connect(this,SIGNAL(mouseDoubleClickEventOverlay(int,int)),overlayActive,SLOT(mouseDoubleClickEvent(int,int)));
            connect(this,SIGNAL(mouseMoveEventOverlay(int,int)),overlayActive,SLOT(mouseMoveEvent(int,int)));
            connect(this,SIGNAL(mousePressEventOverlay(int,int)),overlayActive,SLOT(mousePressEvent(int,int)));
            connect(this,SIGNAL(mouseReleaseEventOverlay(int,int)),overlayActive,SLOT(mouseReleaseEvent(int,int)));
        }

        m_pActiveOverlay = overlayActive;

        return overlayActive;
    }

    if (m_pActiveOverlay)
    {
        disconnect(this,SIGNAL(mouseDoubleClickEventOverlay(int,int)),0,0);
        disconnect(this,SIGNAL(mouseMoveEventOverlay(int,int)),0,0);
        disconnect(this,SIGNAL(mousePressEventOverlay(int,int)),0,0);
        disconnect(this,SIGNAL(mouseReleaseEventOverlay(int,int)),0,0);
    }

    m_pActiveOverlay = 0;

    return 0;
}

/*
void CEPFView::drawForeground(QPainter *p, const QRectF &rect)
{
    if (m_bIsLoading)
    {
        p->resetTransform();
        p->setRenderHint(QPainter::Antialiasing);

        p->setOpacity(1.0);


        p->fillRect(0,0,width(),height(),QColor(30,30,30));

        for (int dot=0;dot<3;dot++)
        {
            if (dot == m_iRenderDot)
                p->setBrush(QColor(200,0,0));
            else
                p->setBrush(QColor("grey"));
            p->drawEllipse((width()/2)-18+(dot*12),(height()/2)+100,8,8);
        }

        p->setBrush(Qt::NoBrush);

        return;
    }
    QGraphicsView::drawForeground(p,rect);
}
*/
void CEPFView::updateDot()
{
    if (m_iRenderDot == 2) m_iRenderDot = 0; else m_iRenderDot++;
    //viewport()->update();
    update();
    if (m_bIsLoading)
        QTimer::singleShot(300,this,SLOT(updateDot()));
}
/*
bool CEPFView::viewportEvent(QEvent *event)
{
    if (!event)
        return false;

    if (event->type() == QEvent::Scroll || event->type() == QEvent::Wheel)
    {
        qDebug() << verticalScrollBar()->value();
        //return QAbstractScrollArea::viewportEvent(event);
        //return true;
    }

    return QGraphicsView::viewportEvent(event);
}

void CEPFView::scrollContentsBy(int dx, int dy)
{
    emit scrollSection(dx,dy);
}
*/
void CEPFView::resizeEvent(QResizeEvent *event)
{
    if (!m_bIsLoading && m_pDocument && m_pDocument->sectionCount() > 0)
    {
        //m_pDocument->layout(height(),width());
        //m_pDocument->stopLayout(true);
        emit layout(height(),width(),m_iCurrentSection,true);
        m_pResizeTimer->start();
        //m_bInResize = true;
    }
}

void CEPFView::resizeDone()
{
    m_pResizeTimer->stop();
    //m_bInResize = false;
    emit layout(height(),width(),m_iCurrentSection,false);
}

void CEPFView::paintEvent(QPaintEvent *ev)
{
    QPainter p;
    p.begin(this);

    if (m_bIsLoading)
    {
        p.resetTransform();
        p.setRenderHint(QPainter::Antialiasing);

        p.setOpacity(1.0);


        p.fillRect(0,0,width(),height(),QColor(30,30,30));

        for (int dot=0;dot<3;dot++)
        {
            if (dot == m_iRenderDot)
                p.setBrush(QColor(200,0,0));
            else
                p.setBrush(QColor("grey"));
            p.drawEllipse((width()/2)-18+(dot*12),(height()/2)+100,8,8);
        }

        p.setBrush(Qt::NoBrush);

        return;
    }
    else
    {
        //p.setRenderHint(QPainter::HighQualityAntialiasing);
        p.setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);

        CSection* s = m_pDocument->section(m_iCurrentSection);
        int nx,ny,psx,psy,sx,sy;

        sx = s->x() * width();
        sy = s->y() * height();

        if (m_iPreviousSection != -1 && m_dTransition > 0)
        {
            CSection* ps = m_pDocument->section(m_iPreviousSection);

            psx = ps->x() * width();
            psy = ps->y() * height();

            if (m_TransFx == CSection::SlideFx)
            {
                nx = (m_dTransition * psx) + ((1-m_dTransition) * sx);
                ny = (m_dTransition * psy) + ((1-m_dTransition) * sy);
                //p.translate(psx-nx,psy-ny);
            }


            //ps->render(&p,QRectF(nx,ny,width(),height()));
            //p.resetTransform();
        }
        else
        {
            nx = (1-m_dTransition) * sx;
            ny = (1-m_dTransition) * sy;
        }

        CSection* cs;


        //for (int i=0;i<m_pDocument->sectionCount();i++)
        int i;

        if (m_iCurrentSection < m_iPreviousSection)
            i=m_pDocument->sectionCount()-1;
        else
            i=0;

        while (true)
        {
            if (m_TransFx == CSection::SlideFx || (m_TransFx == CSection::FadeFx && (i == m_iCurrentSection || i == m_iPreviousSection) ) || i == m_iCurrentSection )
            {

                cs = m_pDocument->section(i);
                if (m_TransFx == CSection::SlideFx)
                {
                    p.resetTransform();
                    p.translate(cs->x()*width()-nx,cs->y()*height()-ny);
                }
                else
                {
                    if (m_TransFx == CSection::FadeFx && i == m_iCurrentSection)
                    {
                        //qDebug() << "transition" << 1-m_dTransition << cs->id();
                        p.setOpacity(1-m_dTransition);
                    }
                    else
                        p.setOpacity(1.0);

                    nx = cs->x() * width();
                    ny = cs->y() * height();

                }
                cs->render(&p,QRectF(nx,ny,width(),height()));
            }

            if (m_iCurrentSection < m_iPreviousSection)
                i--;
            else
                i++;

            if (i<0 || i >= m_pDocument->sectionCount())
                break;
        }


        /*if (m_TransFx == CSection::SlideFx)
        {
            p.translate(sx-nx,sy-ny);
            //qDebug() << "nxy" << nx << ny;
        }
        else
        {
            if (m_TransFx == CSection::FadeFx)
                p.setOpacity(1-m_dTransition);
            nx = sx;
            ny = sy;
        }*/

        //overlays
        p.resetTransform();
        COverlay* overlay;
        for (int oi=0;oi<m_pDocument->overlayCount();oi++)
        {
            overlay = m_pDocument->overlay(oi);
            if (overlay->isVisible())
                overlay->render(&p,QRectF(0,0,width(),height()));
        }


        //s->render(&p,QRectF(nx,ny,width(),height()));
        //p.translate(-s->x()*width(),-s->y()*height());
        //for (int i=0;i<m_pDocument->sectionCount();i++)
        //    m_pDocument->section(i)->render(&p,QRectF(s->x()*width(),s->y()*height(),width(),height()));

    }

    p.end();
}

void CEPFView::mousePressEvent(QMouseEvent *ev)
{
    if (m_bIsLoading)
        return;
    int x,y;
    x = ev->x();
    y = ev->y();
    if (activeOverlay())
        emit mousePressEventOverlay(x,y);
    else
        emit mousePressEvent(x,y);
}

void CEPFView::mouseReleaseEvent(QMouseEvent *ev)
{
    if (m_bIsLoading)
        return;
    int x,y;
    x = ev->x();
    y = ev->y();
    if (activeOverlay())
        emit mouseReleaseEventOverlay(x,y);
    else
        emit mouseReleaseEvent(x,y);
}

void CEPFView::mouseMoveEvent(QMouseEvent *ev)
{
    if (m_bIsLoading)
        return;
    int x,y;
    x = ev->x();
    y = ev->y();
    if (activeOverlay())
        emit mouseMoveEventOverlay(x,y);
    else
        emit mouseMoveEvent(x,y);
}

void CEPFView::mouseDoubleClickEvent(QMouseEvent *ev)
{
    if (m_bIsLoading)
        return;
    int x,y;
    x = ev->x();
    y = ev->y();
    if (activeOverlay())
        emit mouseDoubleClickEventOverlay(x,y);
    else
        emit mouseDoubleClickEvent(x,y);
}

void CEPFView::transitionAnim()
{
    //400 ms anim
    m_dTransition -= 0.05;
    update();
    if (m_dTransition > 0)
        QTimer::singleShot(20,this,SLOT(transitionAnim()));
}

void CEPFView::wheelEvent(QWheelEvent *ev)
{
    if (m_bIsLoading)
        return;
    CSection *s = m_pDocument->section(m_iCurrentSection);
    if (s)
    {
        //s->setScrollY(ev->delta());
        //qDebug() << ev->delta();
        if (ev->orientation() == Qt::Vertical)
            s->setScrollY(s->scrollY()-ev->delta());
        else
            s->setScrollX(s->scrollX()-ev->delta());
        update();
    }
}

int CEPFView::indexForSection(QString id)
{
    return m_pDocument->indexForSection(m_pDocument->sectionByID(id));
}

bool CEPFView::event(QEvent *ev)
{
    bool b = QGLWidget::event(ev);
    if (ev->type() == QEvent::TouchBegin)
    {
        QTouchEvent* tev = dynamic_cast<QTouchEvent*>(ev);
        if (tev)
        {

        }
    }
    return b;
}

void CEPFView::clearDocBuffers()
{
    emit clearBuffers();
}
