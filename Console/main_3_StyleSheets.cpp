#include <QtCore/QCoreApplication>
#include <string>
#include <regex>

// Set new or replace esist property
QString SetStyleSheetProperty(QString stylesheet, const QString& property) {
    if (QRegExp{ "[^:]+:[^;]+;" }.indexIn(property) == -1)  // If not valid property (TODO: make better validation)
        return stylesheet;

    auto name = property.split(":")[0].remove(' ');
    auto regExp = QRegExp("(^|[\\s]+|;)(" + name + "[\\s]*:[^;]+;)"); // find 'start' or 'some space' or ';' before name

    if (regExp.indexIn(stylesheet) == -1)
        return stylesheet + property;
    else {
        auto captureText = regExp.cap(2); // get second capture group in reg exp (....) --> (name: xxxx)
        return stylesheet.replace(captureText, property);
    }
}

QString SetStyleSheetProperties(QString stylesheet, QString selector, const QString& properties) {
    selector.replace("*", "\\*");
    auto regExp = QRegExp(selector + "[\\s\\n]*\\{([^}]+)");

    if (regExp.indexIn(stylesheet) == -1)
        return stylesheet + " " + selector + "{" + properties + "}";

    QString selectorStylesheet = regExp.capturedTexts()[1]; // get first capture group in reg exp (....), --> (name1: xxxx; name2: yyyy; ....)
    QString newStyle = selectorStylesheet;

    for (auto& property : properties.split(";")) {
        newStyle = SetStyleSheetProperty(newStyle, property + ";");
    }
    return stylesheet.replace(selectorStylesheet, newStyle);
}


int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    QString styleSheet = "QWidget { border: 10px solid red; color: green; opacity: 1234; font-size: 20px;}";
    SetStyleSheetProperties(styleSheet, "QWidget", "color: aaaa; opacity: bbbb");

    return a.exec();
}
