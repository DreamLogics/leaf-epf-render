/****************************************************************************
**
** LEAF EPF Render engine
** http://leaf.dreamlogics.com/
**
** Copyright (C) 2013 DreamLogics
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published
** by the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

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
