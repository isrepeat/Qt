#include <SharedMacros.h>
#if STYLE_SHEETS == 1
#pragma once
#include "CustomButton.h"
#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow() = default;

private:
    QLabel* label;
    //QPushButton* pushBtn;
    CustomButton* pushBtn;
};
#endif