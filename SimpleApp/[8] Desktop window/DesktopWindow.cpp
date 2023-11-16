#include "DesktopWindow.h"
#include <HelpersQt.h>

#include <QApplication>
#include <QMimeData>
#include <QLayout>
#include <QDebug>
#include <QTimer>

#include <Windows.h>
#include <thread>



DesktopWindow::DesktopWindow(QWidget* parent)
	: QWidget(parent) 
{
	this->installEventFilter(this);

	//setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_UpdatesDisabled);
	//setAttribute(Qt::WA_NoSystemBackground);
	//setAttribute(Qt::WA_OpaquePaintEvent);
	//setAttribute(Qt::WA_PaintUnclipped);

	//setBackgroundRole(QPalette::Base);
	//setAutoFillBackground(false);
	//setAttribute(Qt::WA_OpaquePaintEvent);

	////QWidget* centralWidget = new QWidget(this);
	////HelpersQt::SetStyleSheetFor({ centralWidget }, "QWidget", "border: 1px solid green;");

	//QVBoxLayout* vLayout = new QVBoxLayout();
	//this->setLayout(vLayout);

	//QWidget* innerWidget = new QWidget(this);
	////HelpersQt::SetStyleSheetFor({ innerWidget }, "QWidget", "border: 2px solid red;");
	////HelpersQt::SetStyleSheetFor({ innerWidget }, "QWidget > QPushButton", "color: dark-gray; border: 2px dotted gray;");
	//innerWidget->setFixedHeight(300);
	//vLayout->addWidget(innerWidget);

	//auto vvLayout = new QVBoxLayout(innerWidget);
	//

	//vvLayout->addSpacing(100);

	//auto btnPrintWindowInfo = new QPushButton("Print window info");
	//auto btnChangeScreenMode = new QPushButton("Change screen mode");
	//auto btnChangeFlags = new QPushButton("Change flags");
	//
	//vvLayout->addWidget(btnPrintWindowInfo);
	//vvLayout->addWidget(btnChangeScreenMode);
	//vvLayout->addWidget(btnChangeFlags);

	//connect(btnPrintWindowInfo, &QPushButton::clicked, [=] {
	//	qDebug() << "Current window flags =" << this->windowFlags();
	//	});

	//connect(btnChangeScreenMode, &QPushButton::clicked, [=] {
	//	if (this->isFullScreen()) {
	//		this->showNormal();
	//	}
	//	else {
	//		this->showFullScreen();
	//	}
	//	});

	//connect(btnChangeFlags, &QPushButton::clicked, [=, srcFlags = this->windowFlags() ] {
	//	static int mode = -1;
	//	mode = (mode + 1) % 2;
	//	
	//	qDebug() << "srcFlags =" << this->windowFlags();

	//	if (mode == 0) {
	//		qDebug() << "set flags = Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint";
	//		this->setWindowFlags(this->windowFlags() | Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	//		this->show();
	//	}
	//	else if (mode == 1) {
	//		qDebug() << "delete flags = Qt::WindowStaysOnTopHint";
	//		this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);
	//		this->show();
	//	}
	//	//else if (mode == 2) {
	//	//	qDebug() << "set flags = Qt::ToolTip";
	//	//	this->setWindowFlags(srcFlags | Qt::ToolTip);
	//	//	this->show();
	//	//}
	//	//else if (mode == 3) {
	//	//	qDebug() << "set flags = Qt::Dialog";
	//	//	this->setWindowFlags(srcFlags | Qt::Dialog);
	//	//	this->show();
	//	//}

	//	qDebug() << "Current flags =" << this->windowFlags();
	//	});

	FrameConfigurationData frameConfiguration;
	frameConfiguration.startX = 0;
	frameConfiguration.startY = 0;
	frameConfiguration.size.width = 1920;
	frameConfiguration.size.height = 1080;
	frameConfiguration.pixelFormat = PixelFormat::BGRA32;

	std::vector<Color> monitorColors = { Color{30,30,30}, Color{90,90,90}, Color{150,150,150} };

	renderingNode = std::make_unique<Test::RenderingNode>();
	renderingNode->Init((HWND)this->winId(), frameConfiguration, monitorColors);
	renderingNode->Start();

	////QTimer::singleShot(5'000, [this] {
	////	this->showFullScreen();
	////	});
}


/* -------------- */
/*     Events     */
/* -------------- */
//bool DesktopWindow::event(QEvent* event) {
//	switch (event->type()) {
//	case QEvent::Paint:
//		return true;
//	}
//
//	return QWidget::event(event);
//}

bool DesktopWindow::eventFilter(QObject* object, QEvent* event) {
	//if (event->type() == QEvent::Paint) { 
	//	return true;
	//}

	return false;
}

void DesktopWindow::paintEvent(QPaintEvent* event) {
	//QPainter painter(this);
	////Here we set the brush to white with alpha
	//painter.setBrush(QColor(255, 255, 255, 64));
	//painter.drawRect(0, 0, 200, 200);
}