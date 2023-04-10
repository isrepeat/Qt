// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QProcess>
#include <QApplication>
#include "MainWindow.h"
#include <Windows.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    
    auto window = new MainWindow();
    window->resize(2880, 1620); // 1920x1080 * 1.5
    window->show();

    return app.exec();
}
