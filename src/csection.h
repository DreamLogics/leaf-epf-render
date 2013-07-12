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
#include <QGraphicsObject>
#include <QMutex>

class CDocument;
class CLayer;
class CBaseObject;

class CViewportItem
{
public:
    CViewportItem();

    void setSize(int height,int width);
    virtual void paint(QPainter *painter);
    virtual QRectF boundingRect() const;

protected:

    //virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);

private:
    QRectF m_qrRect;
};

class CSection : public QObject
{
    Q_OBJECT
public:
    CSection(QString id, CDocument* doc,bool hidden,int x=0, int y=0);

    ~CSection();

    virtual int objectCount();

    virtual void addLayer(CLayer*/*,bool active*/);

    virtual int layerCount();
    virtual CLayer* layer(int index);
    //virtual void setActiveLayer(CLayer* layer);
    //virtual CLayer* activeLayer();

    virtual CBaseObject* objectByID(QString id);

    virtual bool isHidden();
    virtual QString id();
    int x();
    int y();

    virtual CDocument* document();

    //void rendered(QPainter* p);

    virtual void layout(int height, int width);
    CViewportItem* viewportItem();

    virtual void render(QPainter* p, QRectF region);

public slots:

    QObjectList layers();
    //void setActiveLayer(QObject*);
    //QObject* getActiveLayer();
    QObject* getObjectByID(QString id);

    //void updateRendered( const QList<QRectF> &region );
    //void updateFixedObjects(int dy);
    //void scrollSection(int dx, int dy);
    //void updateRendered(QRectF view);
    //void updateRendered();

    void mouseDoubleClickEvent ( int x, int y );
    void mousePressEvent( int x, int y );
    void mouseReleaseEvent( int x, int y );
    void mouseMoveEvent( int x, int y );

private:

    CBaseObject* objectOnPos(int x, int y);

private:
    QList<CLayer*> m_Layers;
    QMap<QString,CBaseObject*> m_ObjectsCatalog;
    //CLayer* m_pActiveLayer;

    QString m_sID;
    CDocument* m_pDoc;
    bool m_bHidden;

    //QImage m_imgRendered;
    CViewportItem* m_pViewportItem;


    QRectF m_rRect;
    int m_iX;
    int m_iY;

};

#endif // CSECTION_H
