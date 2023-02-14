#include <QtCore/QCoreApplication>
#include <Windows.h>
#include <string>
#include <QTimer>
#include <QDebug>
#include "Foo.h"
#include "Bar.h"


void invoke(Second* _this, void (Second::* ptr)(int)) {
    (_this->*ptr)(12);
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);



    Temp temp;
    
    //Second second;

    //QObject::connect(&tempSignals.bar, &std::function<void(int)>::operator(), [] (int value) {
    //    int xxx = 9;
    //    });

    //QObject::connect(&tempSignals.temp, &Temp::Bar, [](int value) {
    //    int xxx = 9;
    //    });

    //QObject::connect(&tempSignals, &TempSignals::Foo<void>, [](int value) {
    //    int xxx = 9;
    //    });

    //QtSignal<void(int,float)>::_Mybase::Signal

    //temp.Connect(&TempSignals::Hello, [](int value) {
    //    int xxx = 9;
    //    });


    //QtConnection::Connect(&temp, &Temp::Hello, &second, &Second::MySlot);

    //QtConnection::Connect(&temp, &Temp::Hello, nullptr, [](int value) -> float {
    //    int xxx = 9;
    //    return 3.11f;
    //    });

    QTimer::singleShot(600, [&temp] {
        temp.EmitSignal();
        });





    //foo->Connect(&FooPublicSignals::Hello, [](QString msg) {
    //    qDebug() << "GOT !!!";
    //    }, Qt::ConnectionType::UniqueConnection);

    //QTimer::singleShot(1'000, [foo] {
    //    foo->DoWork();
    //    Sleep(200);
    //    Sleep(200);
    //    });

    return a.exec();
}
