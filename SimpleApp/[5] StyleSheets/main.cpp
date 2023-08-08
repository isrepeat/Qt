#include <QApplication>
#include "MainWindow.h"

int main(int argc, char **argv) {
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    
    auto window = new MainWindow();
    window->resize(800,600);
    //window->resize(2000,1200);
    window->show();

    return app.exec();
}