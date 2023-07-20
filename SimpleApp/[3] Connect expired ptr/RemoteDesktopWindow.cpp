#include "RemoteDesktopWindow.h"
#include <HelpersQt.h>
#include <QApplication>
#include <QMimeData>
#include <QLayout>
#include <QDebug>
#include <QTimer>
#include <thread>
#include <Windows.h>


RemoteDesktopWindow::RemoteDesktopWindow(QWidget* parent)
	: QWidget(parent) 
{
}

QSize RemoteDesktopWindow::GetSize() {
	return currentSize;
}
void RemoteDesktopWindow::SetSize(QSize size) {
	currentSize = size;
	emit NewSize(size);
}