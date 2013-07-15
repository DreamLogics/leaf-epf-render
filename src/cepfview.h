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
//#include <QGraphicsView>
#include <QGLWidget>
#include "csection.h"

class CDocument;
//class QGraphicsScene;
//class CSectionView;

class LEAFEPFRENDERSHARED_EXPORT CEPFView : public QGLWidget
{
    Q_OBJECT
public:
    CEPFView();


    void setDocument(CDocument* doc);
    int currentSection();

public slots:

    void setSection(QString id);
    void setSection(int index);
    void nextSection();
    void previousSection();
    void tocSection();

    void ready();

    void updateDot();
    void transitionAnim();

signals:

    void loadDocument(int height, int width);
    void viewChanged(int dy);
    //void scrollSection(int dx, int dy);

    //void updateRendered(QRectF);
    void setViewOffset(int dx, int dy);

    void layout(int height, int width);

    void mouseDoubleClickEvent ( int x, int y );
    void mousePressEvent( int x, int y );
    void mouseReleaseEvent( int x, int y );
    void mouseMoveEvent( int x, int y );

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


    //QGraphicsScene* m_pDocScene;
};

#endif // CEPFVIEW_H
