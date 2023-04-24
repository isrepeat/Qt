#pragma once
#include <QString>
#include <QWidget>
#include <QPixmap>
#include <vector>

class HelpersQt {
public:
	static bool IsValidJWT(const QString& jwt);
	static bool IsTokenExpirationLessWeek(const QString& token);

	static QString CurrencySymbolFromName(const QString& currencyName);

	static QString SetStyleSheetProperty(QString stylesheet, const QString& property);
	static QString SetStyleSheetProperties(QString stylesheet, QString selector, const QString& properties);
	static QString DeleteStyleSheetProperty(QString stylesheet, const QString& property);
	static QString DeleteStyleSheetProperties(QString stylesheet, QString selector, const QString& properties);
	static void SetStyleSheetFor(QList<QWidget*> widgets, QString selector, const QString& properties);
	static void RemoveStyleSheetFor(QList<QWidget*> widgets, QString selector, const QString& properties);

	static std::vector<uint8_t> PixmapToVectorBytes(const QPixmap& pixmap);
	static QPixmap PixmapFromVectorBytes(const std::vector<uint8_t>& byteArray, double scale = 1.0);
};