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

#include "csection.h"
#include "clayer.h"
#include <QPainter>

CSection::CSection(QString id, CDocument* doc,bool hidden) : /*QObject(doc), */m_sID(id), m_pDoc(doc), m_bHidden(hidden)
{
    setParent((QObject*)doc);

    connect(this,SIGNAL(changed(QList<QRectF>)),this,SLOT(updateRendered(QList<QRectF>)));
}

CSection::~CSection()
{

}

int CSection::objectCount()
{
    return m_ObjectsCatalog.size();
}

void CSection::addLayer(CLayer* layer,bool active)
{
    m_Layers.append(layer);
    if (active)
        m_pActiveLayer = layer;
    addItem(layer);
}

int CSection::layerCount()
{
    return m_Layers.size();
}

CLayer* CSection::layer(int index)
{
    if (index < 0 || index >= m_Layers.size())
        return 0;

    return m_Layers[index];
}

void CSection::setActiveLayer(CLayer* layer)
{
    m_pActiveLayer = layer;
}

CLayer* CSection::activeLayer()
{
    return m_pActiveLayer;
}

CBaseObject* CSection::objectByID(QString id)
{
    if (!m_ObjectsCatalog.contains(id))
        return 0;

    return m_ObjectsCatalog[id];
}

bool CSection::isHidden()
{
    return m_bHidden;
}

QString CSection::id()
{
    return m_sID;
}

CDocument* CSection::document()
{
    return m_pDoc;
}

QObjectList CSection::layers()
{
    QObjectList list;
    for (int i=0;i<m_Layers.size();i++)
        list.append((QObject*)m_Layers[i]);
    return list;
}

void CSection::setActiveLayer(QObject* obj)
{
    CLayer* layer = dynamic_cast<CLayer*>(obj);
    if (layer)
        setActiveLayer(layer);
}

QObject* CSection::getActiveLayer()
{
    return m_pActiveLayer;
}

QObject* CSection::getObjectByID(QString id)
{
    return getObjectByID(id);
}

QImage& CSection::rendered()
{
    return m_imgRendered;
}

void CSection::updateRendered(const QList<QRectF> &region)
{
    if (m_imgRendered.isNull() || m_imgRendered.height() != height() || m_imgRendered.width() != width())
    {
        m_imgRendered = QImage(width(),height(),QImage::Format_RGB32);
        m_imgRendered.fill(Qt::black);

        QPainter p;
        p.begin(&m_imgRendered);
        render(&p);
        p.end();
        return;
    }

    QPainter p;
    p.begin(&m_imgRendered);
    for (int i=0;i<region.size();i++)
    {
        render(&p,region[i],region[i]);
    }
    p.end();

}
