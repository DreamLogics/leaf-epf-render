#include "epfedit.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    epfedit w;
    w.show();
    
    return a.exec();
}
