#ifndef COVERLAY_H
#define COVERLAY_H

#include "csection.h"
#include <QList>
#include <QMap>

class COverlay : public CSection
{
public:
    COverlay(QString id,CDocument* doc,bool visible);
    virtual ~COverlay();
    virtual int getObjectCount();

    void addLayer(IOEPFLayer*,bool active);

    //virtual QList<int> getLayers();
    virtual int getLayerCount();
    virtual IOEPFLayer* getLayer(int index);
    virtual void setActiveLayer(int layer);
    virtual IOEPFLayer* activeLayer();

    virtual IOEPFObject* objectByID(QString id);

    virtual bool isHidden();
    virtual QString getID();

    virtual void onEPFEvent(IEPFEvent* e);

    virtual void addConnection(QString event, QString target, IEPFEvent* ev);

    virtual IOEPFDocument* document();


public slots:

    QObjectList layers();
    void setActiveLayer(QObject*);
    QObject* getActiveLayer();

    QObject* getObjectByID(QString id);

    virtual bool isVisible();
    virtual void setVisibility(bool b);

private:

    QList<IOEPFLayer*> m_Layers;
    QMap<QString,IOEPFObject*> m_ObjectsCatalog;
    CLayer* m_pActiveLayer;

    QString m_sID;
    bool m_bHidden;
    bool m_bVisible;

    IOEPFDocument* m_pDoc;
};

#endif // COVERLAY_H
