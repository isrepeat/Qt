#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QLabel>

class DesktopWindow : public QWidget {
    Q_OBJECT;

public:
    DesktopWindow(QWidget* parent = nullptr);
    ~DesktopWindow() = default;

private:
    QLabel* label;
    QPushButton* pushBtn;
};