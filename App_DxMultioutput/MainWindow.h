#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include <QLabel>
#include "RenderWindow.h"
#include "Direct2DRenderer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow() = default;

private:
    void Render();

private:
    QLabel* label;
    QTextEdit* textEdit;
    QPushButton* pushBtn;

    std::unique_ptr<Direct2DRenderer> dxRenderer;
    std::unique_ptr<RenderWindow> renderWindow;
    QWidget* renderWindowWidget;
};
