#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include <QLabel>
#include "Popup.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow() = default;

private:

    QLabel* label;
    QTextEdit* textEdit;
    QPushButton* pushBtn;

    PopUp* popUp;
};
