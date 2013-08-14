#include "idevice.h"

IDevice* g_pDevice = 0;

IDevice* Device::currentDevice()
{
    return g_pDevice;
}

void Device::setCurrentDevice(IDevice *dev)
{
    g_pDevice = dev;
}
