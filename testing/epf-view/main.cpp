#include <QApplication>
#include <cepfdocumentreader.h>
#include <cepfview.h>
#include <QThread>
#include <iostream>
#include <QDebug>
#include <idevice.h>

class CDesktopDevice : public IDevice
{
public:
    CDesktopDevice()
    {

    }

    virtual int addApplicationFont(const QByteArray &data)
    {
        return -1;
    }

    virtual void removeApplicationFont(int)
    {

    }

    virtual int addAudioSource(const QByteArray &data)
    {
        return -1;
    }

    virtual void playAudio(int source)
    {

    }

    virtual void stopAudio(int source)
    {

    }

    virtual const char* deviceIdentifier() const
    {
        return "desktop";
    }
    virtual int deviceFlags() const
    {
        return 0/*IDevice::dfTouchScreen*/;
    }

    virtual double getAccelerationX()
    {
        return 0;
    }

    virtual double getAccelerationY()
    {
        return 0;
    }
    virtual double getAccelerationZ()
    {
        return 0;
    }

    virtual double getMagneticX()
    {
        return 0;
    }
    virtual double getMagneticY()
    {
        return 0;
    }
    virtual double getMagneticZ()
    {
        return 0;
    }

    virtual int getPitch()
    {
        return 0;
    }
    virtual int getYaw()
    {
        return 0;
    }
    virtual int getRoll()
    {
        return 0;
    }

    virtual void getLine(const char* label)
    {

    }

    virtual void alert(const char* title, const char* msg)
    {

    }

};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (argc < 2 || argc > 4 || argc == 3)
    {
        std::cout << "Usage: epf-view <epf file> [<width> <height>]\n";
        return 0;
    }

    int height=600,width=800;

    if (argc == 4)
    {
        height = QString(argv[3]).toInt();
        width = QString(argv[2]).toInt();
    }

    Device::setCurrentDevice(new CDesktopDevice());

    CEPFDocumentReader dr;
    CDocument *doc;

    dr.setLanguage("en");
    dr.addPlatform("*");

    QString error;

    QThread* th = new QThread();
    th->start();
    doc = dr.loadFromFile(argv[1],&error,th);
    //CDocument* doc = dr.loadFromFile("dump.epf",&error,th);

    if (!doc)
    {
        qDebug() << error;
        return -1;
    }

    CEPFView epfview;
    epfview.setGeometry(0,0,width,height);
    epfview.show();
    epfview.setDocument(doc);

    return a.exec();
}

