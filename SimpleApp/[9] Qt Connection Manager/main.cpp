#include "QtConnection.h"
#include "TestClasses.h"

#include <QtCore/QCoreApplication>
#include <QObject>
#include <QTimer>
#include <QDebug>
#include <string>


void foo(int, float) {
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    Sender sender;
    SenderTemplate<int> senderTemplate;
    Receiver receiver;


    //FunctionTraits<decltype(&SenderTemplate<int>::VoidSignal)>::Class;
    //FunctionTraits<decltype(&SenderTemplate<int>::VoidSignal)>::Function;

    //Hqt::ConnectInternal<SenderTemplateNotifier>(&senderTemplate, &SenderTemplate<int>::VoidSignal, &receiver, [&receiver] {
    //    }, Qt::QueuedConnection, Hqt::ConnectFlags::DisconnectAfterInvoke);

    //Hqt::ConnectInternal<SenderTemplateNotifier>(&senderTemplate, &SenderTemplate<int>::VoidSignal, &receiver, [&receiver] {
    //Hqt::ConnectInternal(&senderTemplate, &SenderTemplate<int>::VoidSignal, &receiver, [&receiver] {
    Hqt::Connect(&senderTemplate, &SenderTemplate<int>::VoidSignal, &receiver, [&receiver] {
        }, Qt::QueuedConnection, Hqt::ConnectFlags::DisconnectAfterInvoke);


    //Hqt::ConnectInternal(&senderTemplate, &SenderTemplate<int>::VoidSignal_own, &receiver, [&receiver] {
    //    }, Qt::QueuedConnection, Hqt::ConnectFlags::DisconnectAfterInvoke);



    QTimer::singleShot(600, [&sender] {
        emit sender.VoidSignal();
        });

    return a.exec();
}
