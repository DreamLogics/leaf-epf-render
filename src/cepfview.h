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

class CDocument;
class QGraphicsScene;
class CSectionView;

class LEAFEPFRENDERSHARED_EXPORT CEPFView : public QGraphicsView//QObject
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

signals:

    void loadDocument(int height, int width);

protected:

    virtual void drawForeground(QPainter *painter, const QRectF &rect);

private:

    int indexForSection(QString id);

private:
    bool m_bIsLoading;

    QList<QString> m_SectionIndex;
    CDocument* m_pDocument;

    QList<CSectionView*> m_SectionViews;

    int m_iCurrentSection;
    int m_iTOCSection;

    int m_iRenderDot;

    QGraphicsScene* m_pDocScene;
};

#endif // CEPFVIEW_H
