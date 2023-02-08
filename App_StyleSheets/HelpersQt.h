#pragma once
#include <QString>
#include <QWidget>

	class HelpersQt {
	public:
		static bool IsValidJWT(const QString& jwt);
		static bool IsTokenExpirationLessWeek(const QString& token);
		
		static QString SetStyleSheetProperty(QString stylesheet, const QString& property);
		static QString SetStyleSheetProperties(QString stylesheet, QString selector, const QString& properties);
		static QString DeleteStyleSheetProperty(QString stylesheet, const QString& property);
		static QString DeleteStyleSheetProperties(QString stylesheet, QString selector, const QString& properties);
		static void SetStyleSheetFor(QList<QWidget*> widgets, QString selector, const QString& properties);
		static void RemoveStyleSheetFor(QList<QWidget*> widgets, QString selector, const QString& properties);
	};
	