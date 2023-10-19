#include "HelpersQt.h"
#include <QJsonDocument>
#include <QDateTime>
#include <QBuffer>
//#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
//#include <QRegExp>
//#else
#include <QRegularExpression>
//#endif
#include <qstyleoption.h>
#include <qevent.h>

namespace {
    const int64_t secondsInWeek = 7 * 24 * 60 * 60;
}


bool HelpersQt::IsValidJWT(const QString& jwt) {
    if (jwt.isEmpty())
        return false;

    auto splited = jwt.split('.');
    auto jdocHeader = QJsonDocument::fromJson(QByteArray::fromBase64(splited[1].toLocal8Bit()));
    auto expirationDate = jdocHeader["exp"];

    auto currentTime = QDateTime::currentDateTimeUtc();
    auto unixTimestamp = currentTime.toSecsSinceEpoch();

    return expirationDate.toInt() > unixTimestamp;
}

bool HelpersQt::IsTokenExpirationLessWeek(const QString& jwt) {
    if (jwt.isEmpty())
        return false;

    new QKeyEvent(QEvent::KeyPress, 0, 0, 0, 17, 0);

    auto splited = jwt.split('.');
    auto jdocHeader = QJsonDocument::fromJson(QByteArray::fromBase64(splited[1].toLocal8Bit()));
    auto expirationDate = jdocHeader["exp"].toInt();
    int64_t currTimestamp = QDateTime::currentSecsSinceEpoch();

    return (expirationDate - currTimestamp) < secondsInWeek;
}

QString HelpersQt::CurrencySymbolFromName(const QString& currencyName) {
    if (currencyName == "usd") {
        return "$";
    }
    // add other symbols ...

    return "";
}


//#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
//// Set new or replace esist property
//QString HelpersQt::SetStyleSheetProperty(QString stylesheet, const QString& property) {
//    if (QRegExp{ "[^:]+:[^;]+;" }.indexIn(property) == -1) // If not valid property (TODO: make better validation)
//        return stylesheet;
//
//    auto name = property.split(":")[0].remove(' ');
//    auto regExp = QRegExp("(^|[\\s]+|;)(" + name + "[\\s]*:[^;]+;)"); // find 'start' or 'some space' or ';' before name
//
//    if (regExp.indexIn(stylesheet) == -1)
//        return stylesheet + property;
//    else {
//        auto captureText = regExp.cap(2); // get second capture group in reg exp (....) --> (name: xxxx)
//        return stylesheet.replace(captureText, property);
//    }
//}
//
//QString HelpersQt::SetStyleSheetProperties(QString stylesheet, QString selector, const QString& properties) {
//    selector.replace("*", "\\*");
//    auto regExp = QRegExp(selector + "[\\s\\n]*\\{([^}]+)");
//
//    if (regExp.indexIn(stylesheet) == -1)
//        return stylesheet + " " + selector + "{" + properties + "}";
//
//    QString selectorStylesheet = regExp.capturedTexts()[1]; // get first capture group in reg exp (....), --> (name1: xxxx; name2: yyyy; ....)
//    QString newStyle = selectorStylesheet;
//
//    for (auto& property : properties.split(";")) {
//        newStyle = SetStyleSheetProperty(newStyle, property + ";");
//    }
//    return stylesheet.replace(selectorStylesheet, newStyle);
//}
//
//
//QString HelpersQt::DeleteStyleSheetProperty(QString stylesheet, const QString& property) {
//    auto name = QString{ property }.remove(' ');
//    auto regExp = QRegExp("(^|[\\s]+|;)(" + name + "[\\s]*:[^;]+;)");
//
//    if (regExp.indexIn(stylesheet) == -1)
//        return stylesheet;
//    else {
//        auto captureText = regExp.cap(2); // get second capture group in reg exp (....) --> (name: xxxx)
//        return stylesheet.replace(captureText, "");
//    }
//}
//
//QString HelpersQt::DeleteStyleSheetProperties(QString stylesheet, QString selector, const QString& properties) {
//    selector.replace("*", "\\*");
//    auto regExp = QRegExp(selector + "[\\s\\n]*\\{([^}]+)");
//
//    if (regExp.indexIn(stylesheet) == -1)
//        return stylesheet;
//
//    auto selectorStylesheet = regExp.capturedTexts()[1]; // get first capture group in reg exp (....), --> (name1: xxxx; name2: yyyy; ....)
//    QString newStyle = selectorStylesheet;
//
//    for (auto& property : properties.split(";")) {
//        newStyle = DeleteStyleSheetProperty(newStyle, property);
//    }
//
//    return stylesheet.replace(selectorStylesheet, newStyle);
//}
//
//#else // QT_VERSION < QT_VERSION_CHECK(6,0,0)
// Set new or replace esist property
QString HelpersQt::SetStyleSheetProperty(QString stylesheet, const QString& property) {
    auto matchProperty = QRegularExpression("[^:]+:[^;]+;").match(property);
    if (!matchProperty.hasMatch()) // If not valid property (TODO: make better validation)
        return stylesheet;

    auto name = property.split(":")[0].remove(' ');
    auto matchStylesheet = QRegularExpression("(^|[\\s]+|;)(" + name + "[\\s]*:[^;]+;)").match(stylesheet); // find 'start' or 'some space' or ';' before name

    if (matchStylesheet.hasMatch()) {
        auto capturedText = matchStylesheet.captured(2); // get second capture group in reg exp (....) --> (name: xxxx)
        return stylesheet.replace(capturedText, property);
    }
    else {
        return stylesheet + property;
    }
}

QString HelpersQt::SetStyleSheetProperties(QString stylesheet, QString selector, const QString& properties) {
    selector.replace("*", "\\*");
    
    auto matchStylesheet = QRegularExpression(selector + "[\\s\\n]*\\{([^}]+)").match(stylesheet);
    if (!matchStylesheet.hasMatch())
        return stylesheet + " " + selector + "{" + properties + "}";

    QString selectorStylesheet = matchStylesheet.capturedTexts()[1]; // get first capture group in reg exp (....), --> (name1: xxxx; name2: yyyy; ....)
    QString newStyle = selectorStylesheet;

    for (auto& property : properties.split(";")) {
        newStyle = SetStyleSheetProperty(newStyle, property + ";");
    }
    return stylesheet.replace(selectorStylesheet, newStyle);
}


QString HelpersQt::DeleteStyleSheetProperty(QString stylesheet, const QString& property) {
    auto name = QString{ property }.remove(' ');
    auto matchStylesheet = QRegularExpression("(^|[\\s]+|;)(" + name + "[\\s]*:[^;]+;)").match(stylesheet);

    if (matchStylesheet.hasMatch()) {
        auto captureText = matchStylesheet.captured(2); // get second capture group in reg exp (....) --> (name: xxxx)
        return stylesheet.replace(captureText, "");
    }
    else {
        return stylesheet;
    }
}

QString HelpersQt::DeleteStyleSheetProperties(QString stylesheet, QString selector, const QString& properties) {
    selector.replace("*", "\\*");

    auto matchStylesheet = QRegularExpression(selector + "[\\s\\n]*\\{([^}]+)").match(stylesheet);
    if (!matchStylesheet.hasMatch())
        return stylesheet;

    auto selectorStylesheet = matchStylesheet.capturedTexts()[1]; // get first capture group in reg exp (....), --> (name1: xxxx; name2: yyyy; ....)
    QString newStyle = selectorStylesheet;

    for (auto& property : properties.split(";")) {
        newStyle = DeleteStyleSheetProperty(newStyle, property);
    }
    return stylesheet.replace(selectorStylesheet, newStyle);
}
//#endif // QT_VERSION < QT_VERSION_CHECK(6,0,0)


void HelpersQt::SetStyleSheetFor(QList<QWidget*> widgets, QString selector, const QString& properties) {
    for (auto widget : widgets) {
        widget->setStyleSheet(HelpersQt::SetStyleSheetProperties(widget->styleSheet(), selector, properties));
    }
}

void HelpersQt::RemoveStyleSheetFor(QList<QWidget*> widgets, QString selector, const QString& properties) {
    for (auto widget : widgets) {
        widget->setStyleSheet(HelpersQt::DeleteStyleSheetProperties(widget->styleSheet(), selector, properties));
    }
}

std::vector<uint8_t> HelpersQt::PixmapToVectorBytes(const QPixmap& pixmap) {
    qDebug() << "cursor pixmap = " << pixmap;
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");
    return { bytes.begin(), bytes.end() };
}

QPixmap HelpersQt::PixmapFromVectorBytes(const std::vector<uint8_t>& bytes, double scale) {
    QPixmap pixmap;
    pixmap.loadFromData(bytes.data(), bytes.size());
    qDebug() << "cursor pixmap = " << pixmap;
    return pixmap.scaled(pixmap.width() * scale, pixmap.height() * scale, Qt::KeepAspectRatio);
}