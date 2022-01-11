#include "mainwindow.h"
#include <QDebug>
#include <QString>
#include <QList>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusDeviceInfo>
#include <QCanBusFrame>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


//    qDebug() << "closed";
    MainWindow w;
    w.show();
    return a.exec();
}
