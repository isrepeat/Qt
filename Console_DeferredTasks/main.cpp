#include <QtCore/QCoreApplication>
#include "DeferredTasks.h"
#include "QtClasses.h"
#include <Windows.h>
#include <QTimer>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext& context, const QString& msg) {
        OutputDebugStringA((msg.toStdString() + "\n").c_str());
        });

    Prototype* prototype = new Prototype(&a);
    
    //QTimer::singleShot(6'000, [] {
    //    int xxx = 9;
    //    qApp->quit();
    //    });

    return a.exec();
}
