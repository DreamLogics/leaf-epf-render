#ifndef CSCROLLAREA_H
#define CSCROLLAREA_H

#include <QMutex>

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

private:
    int m_iScrollX;
    int m_iScrollY;
    int m_iScrollXMax;
    int m_iScrollYMax;
    QMutex m_mScrollMutex;
};

#endif // CSCROLLAREA_H
