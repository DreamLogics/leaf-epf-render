#include "epfide.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EpfIDE w;
    w.show();

    return a.exec();
}
