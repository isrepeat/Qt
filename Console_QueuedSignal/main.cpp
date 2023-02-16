#include <QtCore/QCoreApplication>
#include <Windows.h>
#include <string>
#include <any>
#include <variant>
#include <QTimer>
#include <QDebug>
#include "Foo.h"
#include "Bar.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    Second second;
    Temp temp;
    
    //temp.Connect(&TempPrivateSignals::ErrorPrivate, &temp, [](int value) {
    //    return value / 100.0f;
    //    });


    //QtConnection::Connect(&temp, &Temp::ErrorPublic, &second, &Second::ErrorSlot);


    //QObject::connect(&temp, &TempPrivateSignals::ErrorPrivate, &temp, [=](int) {
    //    }, Qt::QueuedConnection);


    QTimer::singleShot(600, [&temp] {
        temp.EmitSignal();
        });

    return a.exec();
}
