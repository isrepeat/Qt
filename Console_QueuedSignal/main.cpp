#include <QtCore/QCoreApplication>
#include <Windows.h>
#include <string>
#include <QTimer>
#include <QDebug>
#include "Foo.h"
#include "Bar.h"


template <int defaultType>
class TestClass {
public:
    
    int Func(int type = defaultType) {
        return type;
    }
};

template <int defaultType>
int TestFunc(int type = defaultType) {
    return type;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    //auto res = TestFunc<12>();
    
    TestClass<12> testClass;
    auto res = testClass.Func();

    auto foo = new Foo();
    auto bar = new Bar();


    foo->Connect(&FooPublicSignals::Hello, [](QString msg) {
        qDebug() << "GOT !!!";
        }, Qt::ConnectionType::UniqueConnection);

    QTimer::singleShot(1'000, [foo] {
        foo->DoWork();
        Sleep(200);
        Sleep(200);
        });

    return a.exec();
}
