#ifndef CSCROLLAREAOBJECT_H
#define CSCROLLAREAOBJECT_H

#include "../cbaseobject.h"
#include "../iepfobjectfactory.h"

class QPainter;

class CScrollAreaObjectFactory : public IEPFObjectFactory
{
    virtual CBaseObject* create(QString id, CLayer *layer);
};

class CScrollAreaObject : public CBaseObject
{
    Q_OBJECT
public:
    CScrollAreaObject(QString id, CLayer* layer);
    ~CScrollAreaObject();

    virtual void preload();
    virtual void paint(QPainter *painter);
    virtual void paintBuffered(QPainter* p);

    virtual void mousePressEvent( QPoint pos );
    virtual void mouseReleaseEvent( QPoint pos );
    virtual void mouseMoveEvent( QPoint pos );

    void setScrollXMax(int max);
    void setScrollYMax(int max);
    void setScrollX(int val);
    void setScrollY(int val);

    int scrollX();
    int scrollY();
    int scrollXMax();
    int scrollYMax();

private slots:

    void momentum();

signals:

public slots:

private:

    int m_iScrollXStart;
    int m_iScrollYStart;

    int m_iScrollX;
    int m_iScrollY;
    int m_iScrollXMax;
    int m_iScrollYMax;

    int m_iMomentumDistance;
    int m_iMomentumStart;
    int m_iMomentumPos;

    QElapsedTimer m_SwipeTimer;
    QTimer* m_MomentumTimer;

    QMutex m_mScrollMutex;

    QPoint m_ClickStartPoint;

};

#endif // CSCROLLAREAOBJECT_H
