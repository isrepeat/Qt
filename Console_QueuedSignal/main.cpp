#include <QtCore/QCoreApplication>
#include "QueuedSignal.h"
#include <Windows.h>
#include <string>

//enum Type {
//    Auto = 11,
//    Direct = 22,
//    Queued = 33,
//    Blocking = 44
//};
//
//
//template <Type defaultConnectionType = Type::Auto>
//class Temp {
//public:
//    Temp() = default;
//
//    template <typename Fn>
//    void Connect(Fn func) {
//        this->Connect(func, defaultConnectionType);
//    }
//
//
//protected:
//    template <typename Fn>
//    void Connect(Fn func, Type connectionType) {
//        func();
//        OutputDebugStringA(("type = " + std::to_string(connectionType) + "\n").c_str());
//    }
//};


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //Temp temp;

    //temp.Connect([] {});
    //temp.Connect([] {}, Type::Blocking);

    auto foo = new Foo();
    auto bar = new Bar();

    ////QObject::connect(foo, &FooPrivateSignals::Error, [] {
    ////    qDebug() << "GOT !!!";
    ////    });

    //foo->Connect(&FooPrivateSignals::Error, [] {
    //    qDebug() << "GOT !!!";
    //    });

    //foo->Connect(&FooPrivateSignals::Error, bar, &Bar::Handler);



    QTimer::singleShot(1'000, [foo] {
        foo->DoWork();
        Sleep(200);
        Sleep(200);
        });

    return a.exec();
}
