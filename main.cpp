#include "cs_rov.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CS_ROV cs_rov;
    return a.exec();
}
