#include "MainWindow.h"
#include <QApplication>
#include <QMimeData>
#include <QLayout>
#include <QDebug>
#include <QTimer>
#include <thread>

#include <Windows.h>
#include <QProcess>
#include <QtWin>

// 0x1e1314fb

QPixmap getMouseCursorIconWin()
{
	// Get information about the global cursor.
	CURSORINFO ci;
	ci.cbSize = sizeof(ci);
	GetCursorInfo(&ci);

	qDebug() << "hCursor = " << ci.hCursor;

	// Get Cursor Size
	int cursorWidth = GetSystemMetrics(SM_CXCURSOR) + 10;
	int cursorHeight = GetSystemMetrics(SM_CYCURSOR) + 10;

	// Get your device contexts.
	HDC hdcScreen = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);

	// Create the bitmap to use as a canvas.
	HBITMAP hbmCanvas = CreateCompatibleBitmap(hdcScreen, cursorWidth, cursorHeight);

	// Select the bitmap into the device context.
	HGDIOBJ hbmOld = SelectObject(hdcMem, hbmCanvas);

	// Draw the cursor into the canvas.
	//DrawIcon(hdcMem, 0, 0, ci.hCursor);
	DrawIcon(hdcMem, 0, 0, (HICON)0x38d047e);

	// Convert to QPixmap
	QPixmap cursorPixmap = QtWin::fromHBITMAP(hbmCanvas, QtWin::HBitmapAlpha);

	

	// Clean up after yourself.
	SelectObject(hdcMem, hbmOld);
	DeleteObject(hbmCanvas);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);

	return cursorPixmap;
}

MainWindow::MainWindow(QWidget* parent) 
	: QMainWindow(parent) 
{
	QWidget* centralWidget = new QWidget(this);
	centralWidget->setStyleSheet("QWidget{border: 2px solid green;}");

	QVBoxLayout* vLayout = new QVBoxLayout(centralWidget);
	centralWidget->setLayout(vLayout);
	setCentralWidget(centralWidget);

	label = new QLabel(this);
	label->setStyleSheet("QLabel{border: 2px solid red; background-color: gray;}");
	vLayout->addWidget(label);


	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, [this] {
		label->setPixmap(getMouseCursorIconWin());
		this->setCursor(getMouseCursorIconWin());
		});

	timer->start(100);
	

	//QWidget* innerWidget = new QWidget(this);
	//innerWidget->setStyleSheet("QWidget{border: 2px solid red;}");
	//innerWidget->setFixedHeight(200);
	//vLayout->addWidget(innerWidget);

	//auto vvLayout = new QVBoxLayout(innerWidget);
	//auto hhLayout = new QHBoxLayout();
	//auto hhLayout2 = new QHBoxLayout();


	//auto lbA = new QLabel();
	////lbA->setPixmap(QPixmap("D:\\icon.ico"));
	//lbA->setPixmap(getMouseCursorIconWin());
	////lbA->setStyleSheet("QLabel{color: blue; border: 2px solid blue;}");
	////hhLayout->setAlignment(Qt::AlignBottom);
	//hhLayout->addWidget(lbA);


	//auto pbA = new QPushButton("Some button text");
	//pbA->setStyleSheet("QPushButton{color: brown; border: 2px solid brown;}");
	////hhLayout2->setAlignment(Qt::AlignBottom);
	//hhLayout2->addWidget(pbA);


	////vvLayout->addSpacing(100);
	//vvLayout->addLayout(hhLayout);
	//vvLayout->addLayout(hhLayout2);


	//connect(pbA, &QPushButton::clicked, [this] {
	//	//Beep(500, 500);
	//	//Sleep(1500);
	//	QString program = qApp->arguments()[0];
	//	QStringList arguments = qApp->arguments().mid(1); // remove the 1st argument - the program name
	//	QProcess::startDetached(program, arguments);
	//	//qApp->exit(0);
	//	exit(0);
	//	Sleep(10000);
	//	int xxx = 9;
	//	});
}