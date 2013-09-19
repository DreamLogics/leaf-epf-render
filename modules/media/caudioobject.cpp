#include "caudioobject.h"

CBaseObject* CAudioObjectFactory::create(QString id, CLayer *layer)
{
    return new CAudioObject(id,layer);
}

CAudioObject::CAudioObject(QString id, CLayer *layer) : CBaseObject(id,layer)
{
}

void CAudioObject::preload()
{

}

void CAudioObject::paint(QPainter *painter)
{

}
