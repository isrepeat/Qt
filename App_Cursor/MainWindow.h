#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QPaintEvent>
#include <QTimer>


struct Cursor {
    QPixmap pixmapCursor;
    int hotX = 0;
    int hotY = 0;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    QPixmap GetCursorInfo();

private:
    bool showNativeCursor = true;

    //auto dpiForSystem = GetDpiForSystem();
    const float dpiScaleFactorSystem;


    void paintEvent(QPaintEvent* event) final;

    QLabel* label;
    QTimer* timer;
    QTextEdit* textEdit;
    QPushButton* pushBtn;
    Cursor myCursor;

    unsigned long long gdiplusToken;
    
    //Gdiplus::GdiplusStartupInput gdiplusStartupInput;
};
