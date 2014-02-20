#include "cscrollarea.h"
#include <QPainter>

CScrollArea::CScrollArea()
{
    m_iScrollX = 0;
    m_iScrollY = 0;
    m_iScrollXMax = 0;
    m_iScrollYMax = 0;
}

void CScrollArea::setScrollX(int val)
{
    m_mScrollMutex.lock();
    if (val >= 0 && val <= m_iScrollXMax)
        m_iScrollX = val;
    else if (val < 0)
        m_iScrollX = 0;
    else if (val > m_iScrollXMax)
        m_iScrollX = m_iScrollXMax;
    m_mScrollMutex.unlock();
}

void CScrollArea::setScrollY(int val)
{
    m_mScrollMutex.lock();
    if (val >= 0 && val <= m_iScrollYMax)
        m_iScrollY = val;
    else if (val < 0)
        m_iScrollY = 0;
    else if (val > m_iScrollYMax)
        m_iScrollY = m_iScrollYMax;
    m_mScrollMutex.unlock();
}

void CScrollArea::setScrollXMax(int val)
{
    if (val < 0)
        return;
    m_mScrollMutex.lock();
    if (m_iScrollX > val)
        m_iScrollX = val;
    m_iScrollXMax = val;
    m_mScrollMutex.unlock();
}

void CScrollArea::setScrollYMax(int val)
{
    if (val < 0)
        return;
    m_mScrollMutex.lock();
    if (m_iScrollY > val)
        m_iScrollY = val;
    m_iScrollYMax = val;
    m_mScrollMutex.unlock();
}

int CScrollArea::scrollX()
{
    m_mScrollMutex.lock();
    int i = m_iScrollX;
    m_mScrollMutex.unlock();
    return i;
}

int CScrollArea::scrollY()
{
    m_mScrollMutex.lock();
    int i = m_iScrollY;
    m_mScrollMutex.unlock();
    return i;
}

int CScrollArea::scrollXMax()
{
    m_mScrollMutex.lock();
    int i = m_iScrollXMax;
    m_mScrollMutex.unlock();
    return i;
}

int CScrollArea::scrollYMax()
{
    m_mScrollMutex.lock();
    int i = m_iScrollYMax;
    m_mScrollMutex.unlock();
    return i;
}

void CScrollArea::drawScrollbar(QPainter *p, int w, int h)
{
    int scrollx = scrollX();
    int scrolly = scrollY();
    int scrollxmax = scrollXMax();
    int scrollymax = scrollYMax();

    QPen pen(QColor(128,128,128));
    p->setPen(pen);

    if (scrollxmax > 0 && scrollymax == 0) //horizontal scrollbar
    {
        int scrollerwidth = w - scrollxmax;

        if (scrollerwidth < 100)
            scrollerwidth = 100;

        int scrollareax = w - scrollerwidth;
        int scrolleroffsetx = scrollx * scrollareax / scrollxmax;

        p->fillRect(0,h-10,w,10,QColor(128,128,128,128));
    }
    else if (scrollymax > 0 && scrollxmax == 0) //vertical scrollbar
    {
        int scrollerheight = h - scrollymax;

        if (scrollerheight < 100)
            scrollerheight = 100;

        int scrollareay = h - scrollerheight;
        int scrolleroffsety = scrolly * scrollareay / scrollymax;

        m_mScrollMutex.lock();
        m_rVertScroller = QRectF(w-10,scrolleroffsety,10,scrollerheight);
        m_rVertScrollerBar = QRectF(w-10,0,10,h);
        m_mScrollMutex.unlock();

        p->fillRect(m_rVertScrollerBar,QColor(128,128,128,128));
        p->fillRect(m_rVertScroller,QColor(40,40,40,160));
        p->drawRect(w-9,scrolleroffsety,10-1,scrollerheight-1);
    }
    else if (scrollxmax > 0) //y > 0 already implied / both scrollbars
    {
        int scrollerheight = h - scrollymax;
        int scrollerwidth = w - scrollxmax;

        if (scrollerheight < 100)
            scrollerheight = 100;
        if (scrollerwidth < 100)
            scrollerwidth = 100;

        int scrollareay = h - scrollerheight;
        int scrollareax = w - scrollerwidth;
        int scrolleroffsety = scrolly * scrollareay / scrollymax;
        int scrolleroffsetx = scrollx * scrollareax / scrollxmax;

        p->fillRect(0,h-10,w,10,QColor(128,128,128,128));
        p->fillRect(w-10,0,10,h,QColor(128,128,128,128));
    }

}

QRectF CScrollArea::verticalScroller()
{
    m_mScrollMutex.lock();
    QRectF r = m_rVertScroller;
    m_mScrollMutex.unlock();
    return r;
}

QRectF CScrollArea::horizontalScroller()
{
    m_mScrollMutex.lock();
    QRectF r = m_rHoriScroller;
    m_mScrollMutex.unlock();
    return r;
}

QRectF CScrollArea::verticalScrollerBar()
{
    m_mScrollMutex.lock();
    QRectF r = m_rVertScrollerBar;
    m_mScrollMutex.unlock();
    return r;
}
QRectF CScrollArea::horizontalScrollerBar()
{
    m_mScrollMutex.lock();
    QRectF r = m_rVertScrollerBar;
    m_mScrollMutex.unlock();
    return r;
}
