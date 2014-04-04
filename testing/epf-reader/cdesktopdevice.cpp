#include "cdesktopdevice.h"

int CDesktopDevice::addApplicationFont(const QByteArray &data)
{
    return -1;
}

void CDesktopDevice::removeApplicationFont(int)
{

}


int CDesktopDevice::addAudioSource(const QByteArray &data)
{
    return 0;
}

void CDesktopDevice::playAudio(int source)
{

}

void CDesktopDevice::stopAudio(int source)
{

}

const char* CDesktopDevice::deviceIdentifier() const
{
    return "desktop";
}

int CDesktopDevice::deviceFlags() const
{
    return 0;
}


double CDesktopDevice::getAccelerationX()
{
    return 0;
}

double CDesktopDevice::getAccelerationY()
{
    return 0;
}

double CDesktopDevice::getAccelerationZ()
{
    return 0;
}


double CDesktopDevice::getMagneticX()
{
    return 0;
}

double CDesktopDevice::getMagneticY()
{
    return 0;
}

double CDesktopDevice::getMagneticZ()
{
    return 0;
}


int CDesktopDevice::getPitch()
{
    return 0;
}

int CDesktopDevice::getYaw()
{
    return 0;
}

int CDesktopDevice::getRoll()
{
    return 0;
}


void CDesktopDevice::getLine(const char* label)
{

}

void CDesktopDevice::alert(const char* title, const char* msg)
{

}
