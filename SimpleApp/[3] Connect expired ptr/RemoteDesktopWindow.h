#include <SharedMacros.h>
#if CONNECT_EXPIRED_PTR == 1
#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include <QLabel>

class RemoteDesktopWindow : public QWidget {
    Q_OBJECT

public:
    RemoteDesktopWindow(QWidget* parent = nullptr);
    ~RemoteDesktopWindow() = default;

    QSize GetSize();
    void SetSize(QSize size);

signals:
    void NewSize(QSize newSize);

private:
    QSize currentSize = { 100, 100 };
};
#endif