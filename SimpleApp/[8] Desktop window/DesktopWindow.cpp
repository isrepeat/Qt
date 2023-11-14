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
	//QWidget* centralWidget = new QWidget(this);
	//HelpersQt::SetStyleSheetFor({ centralWidget }, "QWidget", "border: 1px solid green;");

	QVBoxLayout* vLayout = new QVBoxLayout();
	this->setLayout(vLayout);

	QWidget* innerWidget = new QWidget(this);
	//HelpersQt::SetStyleSheetFor({ innerWidget }, "QWidget", "border: 2px solid red;");
	//HelpersQt::SetStyleSheetFor({ innerWidget }, "QWidget > QPushButton", "color: dark-gray; border: 2px dotted gray;");
	innerWidget->setFixedHeight(300);
	vLayout->addWidget(innerWidget);

	auto vvLayout = new QVBoxLayout(innerWidget);
	

	vvLayout->addSpacing(100);

	auto btnPrintWindowInfo = new QPushButton("Print window info");
	auto btnChangeScreenMode = new QPushButton("Change screen mode");
	auto btnChangeFlags = new QPushButton("Change flags");
	
	vvLayout->addWidget(btnPrintWindowInfo);
	vvLayout->addWidget(btnChangeScreenMode);
	vvLayout->addWidget(btnChangeFlags);

	connect(btnPrintWindowInfo, &QPushButton::clicked, [=] {
		qDebug() << "Current window flags =" << this->windowFlags();
		});

	connect(btnChangeScreenMode, &QPushButton::clicked, [=] {
		if (this->isFullScreen()) {
			this->showNormal();
		}
		else {
			this->showFullScreen();
		}
		});



	connect(btnChangeFlags, &QPushButton::clicked, [=, srcFlags = this->windowFlags() ] {
		static int mode = -1;
		mode = (mode + 1) % 3;
		
		qDebug() << "srcFlags =" << srcFlags;

		if (mode == 0) {
			qDebug() << "set flag = Qt::Tool";
			this->setWindowFlags(srcFlags | Qt::Tool);
			this->show();
		}
		else if (mode == 1) {
			qDebug() << "set flag = Qt::ToolTip";
			this->setWindowFlags(srcFlags | Qt::ToolTip);
			this->show();
		}
		else if (mode == 2) {
			qDebug() << "set flag = Qt::Dialog";
			this->setWindowFlags(srcFlags | Qt::Dialog);
			this->show();
		}

		qDebug() << "Current flags =" << this->windowFlags();
		});
}