#ifndef CUNSUPPORTEDOBJECT_H
#define CUNSUPPORTEDOBJECT_H

#include "cbaseobject.h"

class CUnsupportedObject : public CBaseObject
{
public:
    CUnsupportedObject();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // CUNSUPPORTEDOBJECT_H
