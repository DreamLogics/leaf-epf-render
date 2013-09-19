#ifndef CAUDIOOBJECT_H
#define CAUDIOOBJECT_H

#include "cbaseobject.h"
#include "iepfobjectfactory.h"

class CAudioObjectFactory : public IEPFObjectFactory
{
    virtual CBaseObject* create(QString id, CLayer *layer);
};

class CAudioObject : public CBaseObject
{
    Q_OBJECT
public:
    CAudioObject(QString id, CLayer* layer);

    virtual void preload();
    virtual void paint(QPainter *painter);
};


#endif // CAUDIOOBJECT_H
