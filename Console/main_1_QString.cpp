#include <QtCore/QCoreApplication>
#include <QString>
#include <QDebug>


int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);

    
    {// remove double quotes
        QString stringWithDoubleQuotes = "\"Hello my world !!!\"";
        auto list = stringWithDoubleQuotes.split('"');
        auto res = stringWithDoubleQuotes.split('"').join("");
        qDebug() << "res =" << list[1];
    }

    return a.exec();
}
