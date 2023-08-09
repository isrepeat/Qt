// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QProcess>
#include <QTimer>
#include <QApplication>
#include "MainWindow.h"
#include <Windows.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    
    auto window = MainWindow();
    window.resize(800,600);
    window.show();

    auto exitCode = app.exec();
    return exitCode;
}
