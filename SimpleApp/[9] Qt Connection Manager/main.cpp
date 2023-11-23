#include "QtConnection.h"
#include "TestClasses.h"
#include "Helpers.h"

#include <QtCore/QCoreApplication>
#include <QObject>
#include <QTimer>
#include <QDebug>
#include <string>


void nop() {
}

std::mutex mx;

enum Enum {};
enum EnumInt : int {};
enum EnumUInt : unsigned int {};

enum class EnumClass {};
enum class EnumClassInt : int {};
enum class EnumClassUInt : unsigned int {};


int main(int argc, char *argv[]) {
    constexpr auto szInt = sizeof(int);
    constexpr auto szUint = sizeof(unsigned int);

    constexpr auto EnumIsInt = std::is_same_v<std::underlying_type_t<Enum>, int>; // ture
    constexpr auto EnumIntIsInt = std::is_same_v<std::underlying_type_t<EnumInt>, int>; // false
    constexpr auto EnumUIntIsInt = std::is_same_v<std::underlying_type_t<EnumUInt>, int>; // false

    QCoreApplication a(argc, argv);

    Sender sender;
    SenderTemplate<int> senderTemplate;
    Receiver receiver;

    H::movable_function<void()> xxx;
    if (xxx) {
    }

    std::unique_lock<std::mutex> lk{ mx };
    auto scopedLockMovable = H::MakeScope(H::create_move_lambda(std::move(lk), [](H::moved_value<std::unique_lock<std::mutex>> movedValueRef) {
        auto lk = std::move(movedValueRef.get());
        int xx = 9;
        }));

    //auto moveLambda = H::create_move_lambda(std::move(scopedLockMovable), [](H::moved_value<H::Scope<H::movable_function<void()>>> movedValue) {
    //    auto tmp = std::move(movedValue.get());
    //    nop();
    //    return;
    //    });

    Hqt::Connect(&senderTemplate, &SenderTemplate<int>::VoidSignal, &receiver, [&receiver] {
        int xx = 9;
        }, Qt::QueuedConnection, Hqt::ConnectFlags::AutoDisconnect, std::move(scopedLockMovable));


    //QTimer::singleShot(600, [&senderTemplate] {
    H::Timer::Once(600ms,[&senderTemplate] {
        emit senderTemplate.VoidSignal();
        //moveLambda();
        });

    return a.exec();
}
