#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include <QLabel>
#include "PopUpMessage.h"


class AppFeatures : public QObject {
private:
    AppFeatures();
public:
    ~AppFeatures() = default;
    static AppFeatures& GetInstance();

    void Hello();

#ifndef SINGLETON
private:
    PopUpMessage popupMessage;
#endif
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

signals:
    void Exit();

private:
    QLabel* label;
    QTextEdit* textEdit;
    QPushButton* pushBtn;
};
