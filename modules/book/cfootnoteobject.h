#ifndef CFOOTNOTEOBJECT_H
#define CFOOTNOTEOBJECT_H

#include <cbaseobject.h>
#include <iepfobjectfactory.h>

class QPainter;

class CFootnoteObjectactory : public IEPFObjectFactory
{
    virtual CBaseObject* create(QString id, CLayer *layer);
};

class CFootnoteObject : public CBaseObject
{
    Q_OBJECT
public:
    CFootnoteObject(QString id, CLayer* layer);

    virtual void preload();
    virtual void paint(QPainter *painter);
    virtual void paintBuffered(QPainter *p);
    virtual void onEPFEvent(EPFEvent *ev);

signals:

public slots:

};

#endif // CFOOTNOTEOBJECT_H
