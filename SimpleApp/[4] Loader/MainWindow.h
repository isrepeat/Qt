#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include <QLabel>
#include "Loader.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow() = default;

private:
    bool loaderShown = false;
    QLabel* label;
    Loader* loader;
    QTextEdit* textEdit;
    QPushButton* pushBtn;
};