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

#ifndef CEPFVIEW_H
#define CEPFVIEW_H

#include "leaf-epf-render_global.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGLWidget>
#include "csection.h"
#include <QLayout>

class CDocument;
//class QGraphicsScene;
//class CSectionView;
class QTimer;
class COverlay;

class CEPFLayout : public QLayout
{
public:
    CEPFLayout(QWidget* parent);

    virtual void addItem(QLayoutItem *);
    virtual int count() const;
    virtual QLayoutItem* itemAt(int index) const;
    virtual QLayoutItem* takeAt(int index);

    //virtual Qt::Orientations expandingDirections () const;
    virtual QRect geometry() const;
    /*virtual bool isEmpty() const;
    virtual QSize maximumSize() const;
    virtual QSize minimumSize() const;*/
    virtual QSize sizeHint() const;

private:
    QList<QLayoutItem*> m_items;
};

/*class CEPFView;

class CEPFScene : public QGraphicsScene
{
public:
    CEPFScene(CEPFView* view);

protected:

    virtual void drawBackground(QPainter *painter, const QRectF &rect);

private:
    CEPFView* m_pView;
};*/


class LEAFEPFRENDERSHARED_EXPORT CEPFView : public QGLWidget
{
    Q_OBJECT
public:
    CEPFView(QWidget* parent=0);
    ~CEPFView();

    void setDocument(CDocument* doc);
    void unload();
    int currentSection();

    void clearDocBuffers();

    COverlay* activeOverlay();

public slots:

    void setSection(QString id);
    void setSection(int index);
    void nextSection();
    void previousSection();
    void tocSection();

    void ready();

    void updateDot();
    void transitionAnim();

    void resizeDone();

    void updateView();

signals:

    void loadDocument(int height, int width, int currentsection);
    void viewChanged(int dy);
    void sectionChange(QString id);
    //void scrollSection(int dx, int dy);

    //void updateRendered(QRectF);
    void setViewOffset(int dx, int dy);

    void layout(int height, int width, int sectionid, bool bCurrentSectionOnly);

    void mouseDoubleClickEvent ( int x, int y );
    void mousePressEvent( int x, int y );
    void mouseReleaseEvent( int x, int y );
    void mouseMoveEvent( int x, int y );

    void mouseDoubleClickEventOverlay ( int x, int y );
    void mousePressEventOverlay( int x, int y );
    void mouseReleaseEventOverlay( int x, int y );
    void mouseMoveEventOverlay( int x, int y );

    void keyPressEvent(int key, QString val);
    void keyReleaseEvent(int key, QString val);


    void clearBuffers();

protected:

    //virtual void drawForeground(QPainter *painter, const QRectF &rect);
    //virtual bool viewportEvent(QEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    //virtual void scrollContentsBy( int dx, int dy );
    virtual void paintEvent(QPaintEvent *);
    virtual void mouseDoubleClickEvent ( QMouseEvent *ev );
    virtual void mousePressEvent( QMouseEvent *ev );
    virtual void mouseReleaseEvent( QMouseEvent *ev );
    virtual void mouseMoveEvent( QMouseEvent *ev );
    virtual void wheelEvent(QWheelEvent *ev);
    virtual void keyPressEvent(QKeyEvent *ev);
    virtual void keyReleaseEvent(QKeyEvent* ev);

    virtual bool event(QEvent *ev);

private:

    int indexForSection(QString id);

private:
    bool m_bIsLoading;

    QList<QString> m_SectionIndex;
    CDocument* m_pDocument;

    //QList<CSectionView*> m_SectionViews;

    int m_iPreviousSection;
    int m_iCurrentSection;
    int m_iTOCSection;

    int m_iRenderDot;

    CSection::TransitionFx m_TransFx;
    double m_dTransition;

    QTimer* m_pResizeTimer;

    bool m_bInResize;

    COverlay* m_pActiveOverlay;


    //QGraphicsScene* m_pDocScene;
    friend class CEPFGL;
};

#endif // CEPFVIEW_H
