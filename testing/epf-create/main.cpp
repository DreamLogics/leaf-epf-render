#include <QtCore/QCoreApplication>
#include "coepfdocumentwriter.h"
#include <iostream>
#include <QString>

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        COEPFDocumentWriter dw;
        QString err;
        if (!dw.writeDocument(argv[1],&err))
            std::cout << err.toStdString() << std::endl;
        else
            std::cout << "EPF written." << std::endl;
    }
    else
        std::cout << "Usage: epf-create <directory>" << std::endl;

    return 0;
}
