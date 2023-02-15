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
    
    //QtConnection::Connect(&temp, &Temp::ErrorPublic, &temp, [](int value) {
    //    return value / 100.0f;
    //    });


    QtConnection::Connect(&temp, &Temp::ErrorPublic, &second, &Second::ErrorSlot);

    QTimer::singleShot(600, [&temp] {
        temp.EmitSignal();
        });

    return a.exec();
}
