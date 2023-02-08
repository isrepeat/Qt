// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QProcess>
#include <QApplication>
#include "MainWindow.h"
#include "HelpersQt.h"
#include <Windows.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    
    auto window = new MainWindow();
    window->resize(800,600);
    window->show();

    QPushButton* tmpButton = new QPushButton();

    qDebug() << "style orig window = " << window->styleSheet();
    qDebug() << "style orig button = " << tmpButton->styleSheet();

    //window->setStyleSheet(HelpersQt::SetStyleSheetProperties(window->styleSheet(), "QWidget", "color: red; broder: 1px solid green; font-size: 18px; margin: 1 0 1 1;" ));
    HelpersQt::SetStyleSheetFor({window, tmpButton}, "QWidget", "color: red; broder: 1px solid green; font-size: 18px; margin: 1 0 1 1;");
    qDebug() << "new style_1 window = " << window->styleSheet();
    qDebug() << "new style_1 button = " << tmpButton->styleSheet();

    //window->setStyleSheet(HelpersQt::DeleteStyleSheetProperties(window->styleSheet(), "QWidget", "broder; color; margin; font-size;"));
    HelpersQt::RemoveStyleSheetFor({window, tmpButton}, "QWidget", "broder; color; margin; font-size;");
    qDebug() << "del style_1 window = " << window->styleSheet();
    qDebug() << "del style_1 button = " << tmpButton->styleSheet();


    return app.exec();
}
