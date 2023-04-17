#ifndef UTIL_DIALOGS_H
#define UTIL_DIALOGS_H

#include <QMessageBox>

struct Dialog
{
    inline static void info(const QString &title, const QString &message)
    {
        QMessageBox::information(nullptr, title, message, QMessageBox::Ok);
    }

    inline static void warning(const QString &title, const QString &message)
    {
        QMessageBox::warning(nullptr, title, message, QMessageBox::Ok);
    }

    inline static void error(const QString &title, const QString &message)
    {
        QMessageBox::critical(nullptr, title, message, QMessageBox::Ok);
    }
};

#endif
