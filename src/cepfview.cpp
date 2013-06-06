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
#include "csectionview.h"
#include "csection.h"
#include "clayer.h"
#include "cbaseobject.h"
#include <QGraphicsScene>
#include <QDebug>
#include <QTimer>

CEPFView::CEPFView()
{
    m_pDocScene = new QGraphicsScene();
    setScene(m_pDocScene);
    m_bIsLoading = false;
    m_iRenderDot = 0;
}

void CEPFView::setDocument(CDocument *doc)
{
    m_pDocument = doc;
    int i,first=-1;

    for (i=0;i<m_SectionViews.size();i++)
        delete m_SectionViews[i];

    m_SectionViews.clear();
    m_SectionIndex.clear();

    CSectionView* view;
    CSection* section;

    doc->setRenderview(this);
    doc->layout(height(),width());


    for (i=0;i<doc->sectionCount();i++)
    {
        section = doc->section(i);

        view = new CSectionView(section);
        m_SectionViews.append(view);
        m_SectionIndex.append(section->id());

        m_pDocScene->addItem(view);

        //view->setGeometry(width(),0,width(),height());
        view->setX(width());

        if (first == -1 && !section->isHidden())
            first = i;
    }

    setSection(first);
    m_bIsLoading = true;
    QTimer::singleShot(300,this,SLOT(updateDot()));

    disconnect(this,SIGNAL(loadDocument()),0,0);
    connect(this,SIGNAL(loadDocument(int,int)),doc,SLOT(load(int,int)));
    connect(doc,SIGNAL(finishedLoading()),this,SLOT(ready()));

    emit loadDocument(height(),width());
}

void CEPFView::setSection(int index)
{
    if (index < 0 || index >= m_SectionViews.size())
        return;

    CSectionView* view = m_SectionViews[index];
    CSectionView* curview = m_SectionViews[m_iCurrentSection];

    //view->move(0,0);
    view->setX(0);
    //curview->move(width(),0);
    curview->setX(width());

    m_iCurrentSection = index;
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
    for (i=m_iCurrentSection+1;i<m_SectionViews.size();i++)
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
    for (i=m_iCurrentSection-1;i<m_SectionViews.size();i--)
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
}

void CEPFView::tocSection()
{

}

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

void CEPFView::updateDot()
{
    if (m_iRenderDot == 2) m_iRenderDot = 0; else m_iRenderDot++;
    viewport()->update();
    if (m_bIsLoading)
        QTimer::singleShot(300,this,SLOT(updateDot()));
}
