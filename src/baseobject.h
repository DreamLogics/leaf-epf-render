#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <QObject>

class BaseObject : public QObject
{
    Q_OBJECT
public:
    explicit BaseObject(QObject *parent = 0);

signals:

public slots:

};

#endif // BASEOBJECT_H
