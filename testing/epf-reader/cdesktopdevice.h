#ifndef CDESKTOPDEVICE_H
#define CDESKTOPDEVICE_H

#include <idevice.h>

class CDesktopDevice : public IDevice
{
public:

    virtual int addApplicationFont(const QByteArray &data);
    virtual void removeApplicationFont(int);

    virtual int addAudioSource(const QByteArray &data);
    virtual void playAudio(int source);
    virtual void stopAudio(int source);

    //virtual void playMedia(QByteArray*);

    virtual const char* deviceIdentifier() const;
    virtual int deviceFlags() const;

    virtual double getAccelerationX();
    virtual double getAccelerationY();
    virtual double getAccelerationZ();

    virtual double getMagneticX();
    virtual double getMagneticY();
    virtual double getMagneticZ();

    virtual int getPitch();
    virtual int getYaw();
    virtual int getRoll();

    virtual void getLine(const char* label);
    virtual void alert(const char* title, const char* msg);
};

#endif // CDESKTOPDEVICE_H
