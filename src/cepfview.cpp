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

CEPFView::CEPFView()
{
}

void CEPFView::setDocument(CDocument *doc)
{
    m_pDocument = doc;
    int i,n,l,first=-1;

    for (i=0;i<m_SectionViews.size();i++)
        delete m_SectionViews[i];

    m_SectionViews.clear();
    m_SectionIndex.clear();

    CSectionView* view;
    CSection* section;
    CLayer* layer;

    for (i=0;i<doc->sectionCount();i++)
    {
        section = doc->section(i);

        //preload
        for (n=0;n<section->layerCount();n++)
        {
            layer = section->layer(n);
            for (l=0;l<layer->objectCount();l++)
                layer->object(l)->preload();
        }

        view = new CSectionView(section);
        m_SectionViews.append(view);
        m_SectionIndex.append(section->id());

        view->setGeometry(width(),0,width(),height());

        if (first == -1 && !section->isHidden())
            first = i;
    }

    setSection(first);
}

void CEPFView::setSection(int index)
{
    if (index < 0 || index >= m_SectionViews.size())
        return;

    CSectionView* view = m_SectionViews[index];
    CSectionView* curview = m_SectionViews[m_iCurrentSection];

    view->move(0,0);
    curview->move(width(),0);

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
