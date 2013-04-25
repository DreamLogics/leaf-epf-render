#include "cunsupportedobject.h"

CUnsupportedObject::CUnsupportedObject()
{
}

void CUnsupportedObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF r = boundingRect();
}
