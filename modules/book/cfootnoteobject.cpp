#include "cfootnoteobject.h"

CBaseObject* CFootnoteObjectFactory::create(QString id, CLayer *layer)
{
    return new CFootnoteObject(id,layer);
}

CFootnoteObject::CFootnoteObject(QString id, CLayer *layer) : CBaseObject(id,layer)
{
}

void CFootnoteObject::preload()
{
    //register for events
}

void CFootnoteObject::paint(QPainter *painter)
{

}

void CFootnoteObject::paintBuffered(QPainter *painter)
{

}


void CFootnoteObject::onEPFEvent(EPFEvent *ev)
{
    CBaseObject::onEPFEvent(ev);
    if (ev->event() == "onEnterView")
    {

    }
}
