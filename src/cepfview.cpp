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

CEPFView::CEPFView()
{
    m_pDocScene = new QGraphicsScene();
    setScene(m_pDocScene);
    m_bIsLoading = false;
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
    if (!doc->layout(height(),width(),true))
    {
        qDebug() << "EPFView::setDocument(doc) > No layout!";
        return;
    }

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

    disconnect(this,SIGNAL(loadDocument()),0,0);
    connect(this,SIGNAL(loadDocument()),doc,SLOT(load()));

    emit loadDocument();
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

}

void CEPFView::tocSection()
{

}

void CEPFView::drawForeground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawForeground(painter,rect);
}
