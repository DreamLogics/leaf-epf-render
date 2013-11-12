#ifndef CSCROLLAREAOBJECT_H
#define CSCROLLAREAOBJECT_H

#include "../cbaseobject.h"
#include "../iepfobjectfactory.h"
#include "../cscrollarea.h"

class QPainter;

class CScrollAreaObjectFactory : public IEPFObjectFactory
{
    virtual CBaseObject* create(QString id, CLayer *layer);
};

class CScrollAreaObject : public CBaseObject, public CScrollArea
{
    Q_OBJECT
public:
    CScrollAreaObject(QString id, CLayer* layer);
    ~CScrollAreaObject();

    virtual void preload();
    virtual void paint(QPainter *painter);
    virtual void paintBuffered(QPainter* p);

    virtual void layout(QRectF relativeTo, QList<CBaseObject*> updatelist = QList<CBaseObject*>());

    virtual void mousePressEvent( QPoint pos );
    virtual void mouseReleaseEvent( QPoint pos );
    virtual void mouseMoveEvent( QPoint pos );


private slots:

    void momentum();

signals:

public slots:

private:

    int m_iScrollXStart;
    int m_iScrollYStart;

    int m_iMomentumDistance;
    int m_iMomentumStart;
    int m_iMomentumPos;

    QElapsedTimer m_SwipeTimer;
    QTimer* m_MomentumTimer;

    QMutex m_mScrollMutex;

    QPoint m_ClickStartPoint;

};

#endif // CSCROLLAREAOBJECT_H
