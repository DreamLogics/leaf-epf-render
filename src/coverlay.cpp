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

#include "coverlay.h"
#include "css/css_style.h"

COverlay::COverlay(QString id,CDocument* doc,bool visible) : CSection(id,doc,true), m_bVisible(visible)
{

}

COverlay::~COverlay()
{

}

bool COverlay::isVisible()
{
    return m_bVisible;
}

void COverlay::setVisibility(bool b)
{
    m_bVisible = b;
}

void COverlay::layout(int height, int width)
{
    documentItem()->setSize(height,width);
    CSS::Stylesheet* css = document()->stylesheet();

    int i,n;
    CLayer* l;
    CBaseObject* obj;
    for (i=0;i<layerCount();i++) //layout all objects which are relative to the document and set their position to fixed
    {
        l = layer(i);
        obj=0;
        for (n=0;n<l->objectCount();n++)
        {
            obj = l->object(n);
            if (dynamic_cast<CLayer*>(obj->parentItem()))
            {
                css->property(obj,"position")->setValue("fixed");
                obj->layout();
            }
        }
    }
}
