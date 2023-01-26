#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QTimer>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow() = default;

private:
    QPixmap GetCursorInfo();

private:
    bool showNativeCursor = true;

    //auto dpiForSystem = GetDpiForSystem();
    const float dpiScaleFactorSystem;

    QLabel* label;
    QTimer* timer;
    QTextEdit* textEdit;
    QPushButton* pushBtn;
    QPixmap pixmapCursor;
};
