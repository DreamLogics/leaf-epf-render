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
//#include <QGraphicsScene>
#include <QString>
#include <QMap>
#include <QImage>
//#include <QGraphicsObject>
#include <QMutex>
#include "epfevent.h"
#include <QColor>

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

class CSection : public QObject, public EPFComponent
{
    Q_OBJECT
public:
    CSection(QString id, CDocument* doc,bool hidden,int x=0, int y=0);

    virtual ~CSection();

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

    virtual void layout(int height, int width, QList<CBaseObject*> updatelist = QList<CBaseObject*>());
    virtual void layout(QList<CBaseObject*> updatelist = QList<CBaseObject*>());
    CViewportItem* viewportItem();

    virtual void render(QPainter* p, QRectF region);

    enum TransitionFx
    {
        NoneFx = 0,
        SlideFx,
        FadeFx
    };

    TransitionFx transitionType();

    void setFocus(CBaseObject* obj);
    CBaseObject* focus();

    void setScrollXMax(int max);
    void setScrollYMax(int max);
    void setScrollX(int val);
    void setScrollY(int val);

    int scrollX();
    int scrollY();
    int scrollXMax();
    int scrollYMax();

    void clearBuffers();

    void takeControl(CBaseObject* obj);
    void releaseControl(CBaseObject* obj);

    virtual void onEPFEvent(EPFEvent *ev);

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

    void keyEvent(int key, QString val);

private:

    CBaseObject* objectOnPos(int x, int y);
    void drawScrollbar(QPainter* p);

private:
    QList<CLayer*> m_Layers;
    QMap<QString,CBaseObject*> m_ObjectsCatalog;
    //CLayer* m_pActiveLayer;

    QString m_sID;
    CDocument* m_pDoc;
    bool m_bHidden;

    //QImage m_imgRendered;
    CViewportItem* m_pViewportItem;

    CBaseObject* m_pFocusObj;

    CBaseObject* m_pControlObj;


    QRectF m_rRect;
    int m_iX;
    int m_iY;

    int m_iScrollXStart;
    int m_iScrollYStart;

    int m_iScrollX;
    int m_iScrollY;
    int m_iScrollXMax;
    int m_iScrollYMax;

    int m_iLayoutHeight;
    int m_iLayoutWidth;

    QMutex m_mRectMutex;
    QMutex m_mScrollMutex;
    QMutex m_mTransitionFxMutex;
    QMutex m_mScrollStyleMutex;

    QColor m_qcScrollbarColor;
    QColor m_qcScrollerColor;

    QRectF m_rVertScroller;
    QRectF m_rHorScroller;

    QRectF m_rVertScrollerBar;
    QRectF m_rHorScrollerBar;

    QPoint m_ClickStartPoint;

    TransitionFx m_iTransitionFx;

};

#endif // CSECTION_H
