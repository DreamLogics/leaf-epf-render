#ifndef CLAYER_H
#define CLAYER_H

#include <QObject>
#include <QString>
#include <QList>

class CBaseObject;
class CSection;

class CLayer : public QObject
{
    Q_OBJECT
public:
    CLayer(QString id, QObject *parent = 0);

    QString id();

    CBaseObject* object(int index);
    int objectCount();

    CSection* section();

    
signals:
    
public slots:
    

private:

    QList<CBaseObject*> m_Objects;
};

#endif // CLAYER_H
