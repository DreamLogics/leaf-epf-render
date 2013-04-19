#ifndef IEPFOBJECTFACTORY_H
#define IEPFOBJECTFACTORY_H

class CBaseObject;
class QString;
class CLayer;

class IEPFObjectFactory
{
    public:
    ~IEPFObjectFactory() {}

    CBaseObject* create(QString id, CLayer* layer) = 0;

};

#endif // IEPFOBJECTFACTORY_H
