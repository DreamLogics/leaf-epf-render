#ifndef CSECTION_H
#define CSECTION_H

#include <QObject>
#include <QGraphicsScene>

class CDocument;
class CLayer;
class CBaseObject;

class CSection : public QObject, public QGraphicsScene
{
    Q_OBJECT
public:
    CSection(QString id, CDocument* doc,bool hidden);

    ~CSection();

    virtual int objectCount();

    void addLayer(CLayer*,bool active);

    virtual int layerCount();
    virtual CLayer* layer(int index);
    virtual void setActiveLayer(int layer);
    virtual CLayer* activeLayer();

    virtual CBaseObject* objectByID(QString id);

    virtual bool isHidden();
    virtual QString getID();

    virtual CDocument* document();

public slots:

    QObjectList layers();
    void setActiveLayer(QObject*);
    QObject* getActiveLayer();
    QObject* getObjectByID(QString id);

private:
    QList<CLayer*> m_Layers;
    QMap<QString,CBaseObject*> m_ObjectsCatalog;
    CLayer* m_pActiveLayer;

    QString m_sID;
    bool m_bHidden;

    CDocument* m_pDoc;
    
};

#endif // CSECTION_H
