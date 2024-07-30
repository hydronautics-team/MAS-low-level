#include <QApplication>
#include "controlbeacon.h"
#include "widget.h"
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ControlBeacon cb;

//    Widget w;

//    QObject::connect(&w, &Widget::startDirect, &cb, &ControlBeacon::slotStartDirect);
//    QObject::connect(&w, &Widget::startRoundR, &cb, &ControlBeacon::slotStartRound);
//    QObject::connect(&w, &Widget::stop, &cb, &ControlBeacon::slotStop);
//    QObject::connect(&cb, &ControlBeacon::updateUpdate, &w, &Widget::update);
//    QObject::connect(&w, &Widget::signalTimeUpdate, &cb, &ControlBeacon::updateTime);

//    w.show();


    return a.exec();

}
