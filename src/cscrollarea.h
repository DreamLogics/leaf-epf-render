#ifndef CSCROLLAREA_H
#define CSCROLLAREA_H

#include <QMutex>
#include <QRectF>

class QPainter;

class CScrollArea
{
public:
    CScrollArea();

    void setScrollXMax(int max);
    void setScrollYMax(int max);
    void setScrollX(int val);
    void setScrollY(int val);

    int scrollX();
    int scrollY();
    int scrollXMax();
    int scrollYMax();

    QRectF verticalScroller();
    QRectF horizontalScroller();

    QRectF verticalScrollerBar();
    QRectF horizontalScrollerBar();

    void drawScrollbar(QPainter *p, int width, int height);

private:
    int m_iScrollX;
    int m_iScrollY;
    int m_iScrollXMax;
    int m_iScrollYMax;
    QMutex m_mScrollMutex;
    QRectF m_rVertScroller;
    QRectF m_rVertScrollerBar;
    QRectF m_rHoriScroller;
    QRectF m_rHoriScrollerBar;
};

#endif // CSCROLLAREA_H
