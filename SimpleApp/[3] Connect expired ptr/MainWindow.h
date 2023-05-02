//#pragma once
//#include <QMainWindow>
//#include <QPushButton>
//#include <QTextEdit>
//#include <QWidget>
//#include <QLabel>
//#include <memory>
//#include "RemoteDesktopWindow.h"
//
//class MainWindow : public QMainWindow {
//    Q_OBJECT
//
//public:
//    MainWindow(QWidget* parent = nullptr);
//    ~MainWindow() = default;
//
//private:
//    void SetupButtonsEvents();
//
//private:
//    QLabel* label;
//    QPushButton* btnMonitorA;
//    QPushButton* btnMonitorB;
//    QPushButton* btnMonitorC;
//    QPushButton* btnPlusA;
//    QPushButton* btnMinusA;
//    QPushButton* btnPlusB;
//    QPushButton* btnMinusB;
//    QPushButton* btnPlusC;
//    QPushButton* btnMinusC;
//
//    std::vector<std::shared_ptr<RemoteDesktopWindow>> remoteDesktopWindows;
//    RemoteDesktopWindow* currentRemoteDesktopWindows = nullptr;
//};