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

#ifndef CSECTION_H
#define CSECTION_H

#include <QObject>
#include <QGraphicsScene>
#include <QString>
#include <QMap>
#include <QImage>
#include <QGraphicsItem>

class CDocument;
class CLayer;
class CBaseObject;

class CDocumentItem : public QGraphicsItem
{
public:
    CDocumentItem();

    void setSize(int height,int width);
    void setHeight(int height);
    void increaseHeight(int delta);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QRectF boundingRect() const;

protected:

    //virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);

private:
    QRectF m_qrRect;
};

class CSection : /*public QObject, */public QGraphicsScene
{
    Q_OBJECT
public:
    CSection(QString id, CDocument* doc,bool hidden);

    ~CSection();

    virtual int objectCount();

    void addLayer(CLayer*,bool active);

    virtual int layerCount();
    virtual CLayer* layer(int index);
    virtual void setActiveLayer(CLayer* layer);
    virtual CLayer* activeLayer();

    virtual CBaseObject* objectByID(QString id);

    virtual bool isHidden();
    virtual QString id();

    virtual CDocument* document();

    QImage& rendered();

    void layout(int height, int width);
    CDocumentItem* documentItem();

public slots:

    QObjectList layers();
    void setActiveLayer(QObject*);
    QObject* getActiveLayer();
    QObject* getObjectByID(QString id);

    void updateRendered( const QList<QRectF> &region );

private:
    QList<CLayer*> m_Layers;
    QMap<QString,CBaseObject*> m_ObjectsCatalog;
    CLayer* m_pActiveLayer;

    QString m_sID;
    bool m_bHidden;

    CDocument* m_pDoc;
    QImage m_imgRendered;
    CDocumentItem* m_pDocumentItem;
    
};

#endif // CSECTION_H
