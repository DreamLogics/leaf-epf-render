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

    virtual void preload();
    virtual void paint(QPainter *painter);

    virtual void mousePressEvent( QPoint pos );
    virtual void mouseReleaseEvent( QPoint pos );
    virtual void mouseMoveEvent( QPoint pos );

signals:

public slots:

};

#endif // CSCROLLAREAOBJECT_H
