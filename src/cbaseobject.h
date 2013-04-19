#ifndef CBASEOBJECT_H
#define CBASEOBJECT_H

#include <QGraphicsObject>

class CBaseObject : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit CBaseObject(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // CBASEOBJECT_H
