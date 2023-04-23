#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include <QLabel>
#include "RenderingNode.h"
#include "NoParentWindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow() = default;

private:
    //void InitRender(RenderingNode* renderNode, FrameConfigurationData frameConfiguration, HWND hwnd, std::vector<Color>::iterator& itColors, const std::vector<Color>::iterator& itColorsEnd);

    void showEvent(QShowEvent* event) final;
    void hideEvent(QHideEvent* event) final;

private:
    QLabel* label;
    QTextEdit* textEdit;
    QPushButton* pushBtn;

    std::vector<Color>::iterator itColors_1;
    std::vector<Color>::iterator itColors_2;

    std::unique_ptr<RenderingNode> renderingNode_1;
    std::unique_ptr<RenderingNode> renderingNode_2;
};
