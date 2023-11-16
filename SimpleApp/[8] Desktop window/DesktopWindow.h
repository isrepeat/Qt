#pragma once
#include "RenderingNode.h"
#include <QMainWindow>
#include <QPushButton>
#include <QPainter>
#include <QWidget>
#include <QLabel>

class DesktopWindow : public QWidget {
    Q_OBJECT;

public:
    DesktopWindow(QWidget* parent = nullptr);
    ~DesktopWindow() = default;

private:
    //bool event(QEvent* event) final;
    bool eventFilter(QObject* object, QEvent* event) final;
    void paintEvent(QPaintEvent* event) final;

private:
    QLabel* label;
    QPushButton* pushBtn;

    std::unique_ptr<Test::RenderingNode> renderingNode;
};