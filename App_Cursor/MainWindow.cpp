#include "MainWindow.h"
#include <Windows.h>
#include <QByteArray>
#include <QMimeData>
#include <QProcess>
#include <QLayout>
#include <QBuffer>
#include <QDebug>
#include <QTimer>
#include <QtWin>
#include <thread>
#include <vector>

struct CursorInfo {
	int hCursor = 0;
	std::vector<uint8_t> image;
};

std::vector<uint8_t> PixmapToVectorBytes(const QPixmap& pixmap) {
	QByteArray bytes;
	QBuffer buffer(&bytes);
	buffer.open(QIODevice::WriteOnly);
	pixmap.save(&buffer, "PNG");
	return { bytes.begin(), bytes.end() };
}

QPixmap PixmapFromVectorBytes(const std::vector<uint8_t>& bytes) {
	QPixmap pixmap;
	pixmap.loadFromData(bytes.data(), bytes.size());
	return pixmap;
}

//CursorInfo g_cursorInfo;


MainWindow::MainWindow(QWidget* parent) 
	: QMainWindow(parent)
	, dpiScaleFactorSystem{ GetDpiForSystem() / 96.0f }
{
	QWidget* centralWidget = new QWidget(this);
	centralWidget->setStyleSheet("QWidget{border: 2px solid green;}");

	QVBoxLayout* vLayout = new QVBoxLayout(centralWidget);
	centralWidget->setLayout(vLayout);
	setCentralWidget(centralWidget);

	label = new QLabel(this);
	label->setStyleSheet("QLabel{border: 2px solid red; background-color: gray;}");
	vLayout->addWidget(label);

	pushBtn = new QPushButton("Switch cursor");
	//pushBtn->setStyleSheet("QPushButton{color: brown; border: 2px solid brown;}");
	vLayout->addWidget(pushBtn);


	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, [this] {
		//auto cursorInfo = GetCursorInfo();
		//label->setPixmap(PixmapFromVectorBytes(cursorInfo.image));

		//auto pixmapCursor = GetCursorInfo();

		if (showNativeCursor) {
			GetCursorInfo();
		}
		else {
			QCursor cursror(this->pixmapCursor, 0, 0);
			this->setCursor(cursror);
		}

		label->setPixmap(this->pixmapCursor);
		});

	timer->start(500);
	

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



	////vvLayout->addSpacing(100);
	//vvLayout->addLayout(hhLayout);
	//vvLayout->addLayout(hhLayout2);


	connect(pushBtn, &QPushButton::clicked, [this] {
		showNativeCursor = !showNativeCursor;
		pushBtn->setText(QString{ "showNativeCursor = %1" }.arg(showNativeCursor ? "true" : "false"));
		});
}


QPixmap MainWindow::GetCursorInfo() {
	// Get information about the global cursor.
	CURSORINFO ci;
	ci.cbSize = sizeof(ci);
	::GetCursorInfo(&ci);

	//auto logicalDpiX = this->logicalDpiX();
	//auto logicalDpiY = this->logicalDpiY();
	//auto physicalDpiX = this->physicalDpiX();
	//auto physicalDpiY = this->physicalDpiY();
	auto dpi = devicePixelRatio();
	auto qtDpi = devicePixelRatioF();
	float dtDpi = qtDpi / dpiScaleFactorSystem;

	// Get Cursor Size.
	//int cursorWidth = GetSystemMetrics(SM_CXCURSOR) * dtDpi;
	//int cursorHeight = GetSystemMetrics(SM_CYCURSOR) * dtDpi;
	int cursorWidth = GetSystemMetrics(SM_CXCURSOR) + 100;
	int cursorHeight = GetSystemMetrics(SM_CYCURSOR) + 100;
	//int cursorWidth = GetSystemMetrics(SM_CXCURSOR);
	//int cursorHeight = GetSystemMetrics(SM_CYCURSOR);

	// Get your device contexts.
	HDC hdcScreen = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);

	// Create the bitmap to use as a canvas.
	HBITMAP hbmCanvas = CreateCompatibleBitmap(hdcScreen, cursorWidth, cursorHeight);

	// Select the bitmap into the device context.
	HGDIOBJ hbmOld = SelectObject(hdcMem, hbmCanvas);

	// Draw the cursor into the canvas.
	DrawIcon(hdcMem, 0, 0, ci.hCursor);

	// Convert to QPixmap.
	QPixmap cursorPixmap = QtWin::fromHBITMAP(hbmCanvas, QtWin::HBitmapNoAlpha);

	// Clean up after yourself.
	SelectObject(hdcMem, hbmOld);
	DeleteObject(hbmCanvas);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);


	//g_cursorInfo = CursorInfo{ reinterpret_cast<int>(ci.hCursor), PixmapToVectorBytes(cursorPixmap) };
	////return CursorInfo{ reinterpret_cast<int>(ci.hCursor), PixmapToVectorBytes(cursorPixmap) };

	//return g_cursorInfo;
	//return cursorPixmap;

	auto image = PixmapToVectorBytes(cursorPixmap);
	auto pixmap = PixmapFromVectorBytes(image);

	auto w = pixmap.width();
	auto h = pixmap.height();
	

	

	//this->pixmapCursor = pixmap.scaled(float(cursorWidth) / dpi, float(cursorHeight) / dpi, Qt::AspectRatioMode::KeepAspectRatio);
	this->pixmapCursor = pixmap.scaled(cursorWidth / qtDpi, cursorHeight / qtDpi);
	//this->pixmapCursor = pixmap.scaled(24, 24);
	//this->pixmapCursor = pixmap;
	return {};
}