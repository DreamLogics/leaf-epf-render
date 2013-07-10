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

#include "canimframe.h"
#include "cbaseobject.h"
#include "csection.h"
#include "cdocument.h"
#include "cepfview.h"
#include <QDebug>
//#include "css/css_style.h"

CAnimFrame::CAnimFrame(CSection* section) : m_pSection(section)//CSectionRender *sr) : m_pSR(sr)
{

}

void CAnimFrame::setPropertiesForObject(CBaseObject *object, QMap<QString, QString> map)
{
    if (m_Objects.contains(object))
        m_Objects[object] = map;
    else
        m_Objects.insert(object,map);
}

void CAnimFrame::apply()
{
    QString css;
    QMap<QString,QString> props;
    QList<QString> proplist;
    QList<CBaseObject*> objects = m_Objects.keys();
    CBaseObject* obj;
    for (int i=0;i<objects.size();i++)
    {
        obj = objects[i];
        props = m_Objects[obj];
        proplist = props.keys();
        css = "";
        for (int n=0;n<proplist.size();n++)
        {
            css += proplist[n] + ":" + props[proplist[n]] + ";";
        }
        obj->setCSSOverride(css);
        //qDebug() << obj->id() << css;
    }



    //update view
    //m_pSR->updateView();
    //m_pSection->renderer()->updateView();
    //m_pSection->document()->renderview()->update();
    m_pSection->layout(m_pSection->document()->renderview()->height(),m_pSection->document()->renderview()->width());
    m_pSection->document()->renderview()->update();
}

bool CAnimFrame::hasObjectProps(CBaseObject *obj)
{
    return m_Objects.contains(obj);
}

CSection* CAnimFrame::section()
{
    return m_pSection;
}

QMap<CBaseObject*,QMap<QString,QString> > CAnimFrame::objectPropMap()
{
    return m_Objects;
}
