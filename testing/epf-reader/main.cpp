#include "epfreader.h"
#include <QApplication>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString file;

    if (argc == 2)
        file = argv[1];

    EPFReader w(file);
    w.show();

    return a.exec();
}
