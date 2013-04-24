#ifndef IDEVICE_H
#define IDEVICE_H

class IDevice
{
public:
    ~IDevice() {}

    virtual int addAudioSource(QByteArray*) = 0;
    virtual void playAudio(int source) = 0;
    virtual void stopAudio(int source) = 0;

    //virtual void playMedia(QByteArray*) = 0;

    virtual const char* deviceIdentifier() const = 0;

    virtual double getAccelerationX() = 0;
    virtual double getAccelerationY() = 0;
    virtual double getAccelerationZ() = 0;

    virtual double getMagneticX() = 0;
    virtual double getMagneticY() = 0;
    virtual double getMagneticZ() = 0;

    virtual int getPitch() = 0;
    virtual int getYaw() = 0;
    virtual int getRoll() = 0;

};

#endif // IDEVICE_H
