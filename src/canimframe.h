#ifndef CANIMFRAME_H
#define CANIMFRAME_H

#include <QObject>

class CSection;
class CBaseObject;

class CAnimFrame : public QObject
{
    Q_OBJECT
public:    
    CAnimFrame(CSection* s);

    void setPropertiesForObject(CBaseObject* object, QMap<QString,QString>);
    void apply();
    QMap<CBaseObject*,QMap<QString,QString> > objectPropMap();
    bool hasObjectProps(CBaseObject* obj);
    CSection* section();

private:
    QMap<CBaseObject*,QMap<QString,QString> > m_Objects;
    CSection* m_pSection;

signals:
    
public slots:
    
};

#endif // CANIMFRAME_H
