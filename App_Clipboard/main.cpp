// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QProcess>
#include <QApplication>
#include "MainWindow.h"
#include <Windows.h>

int main(int argc, char **argv)
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    
    auto window = new MainWindow();
    window->resize(800,600);
    window->show();

    return app.exec();
}
