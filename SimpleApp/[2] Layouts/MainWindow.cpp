//#include "MainWindow.h"
//#include "../HelpersQt.h"
//#include <QApplication>
//#include <QMimeData>
//#include <QLayout>
//#include <QDebug>
//#include <QTimer>
//#include <thread>
//#include <Windows.h>
//
//
//MainWindow::MainWindow(QWidget* parent) 
//	: QMainWindow(parent) 
//{
//	QWidget* centralWidget = new QWidget(this);
//	HelpersQt::SetStyleSheetFor({ centralWidget }, "QWidget", "border: 1px solid green;");
//
//	QVBoxLayout* vLayout = new QVBoxLayout(centralWidget);
//	centralWidget->setLayout(vLayout);
//	setCentralWidget(centralWidget);
//
//	QWidget* innerWidget = new QWidget(this);
//	HelpersQt::SetStyleSheetFor({ innerWidget }, "QWidget", "border: 2px solid red;");
//	innerWidget->setFixedHeight(200);
//	vLayout->addWidget(innerWidget);
//
//	auto vvLayout = new QVBoxLayout(innerWidget);
//	auto hhLayout = new QHBoxLayout();
//	auto hhLayout2 = new QHBoxLayout();
//
//
//	auto lbA = new QLabel();
//	lbA->setText("link = <a href=\"https://google.com/\"><span>Click Here!</span></a>");
//	//lbA->setTextFormat(Qt::RichText);
//	lbA->setTextInteractionFlags(Qt::TextBrowserInteraction);
//	lbA->setOpenExternalLinks(true);
//	HelpersQt::SetStyleSheetFor({ lbA }, "QLabel", "color: gray; border: 2px solid gray;");
//	HelpersQt::SetStyleSheetFor({ lbA }, "span", "color: red;");
//
//	auto lbB = new QLabel("label B");
//	//hhLayout->setAlignment(Qt::AlignBottom);
//	hhLayout->addWidget(lbA);
//	hhLayout->addSpacing(50);
//	hhLayout->addWidget(lbB);
//
//
//	auto pbA = new QPushButton("Some button text");
//	pbA->setStyleSheet("QPushButton{color: brown; border: 2px solid brown;}");
//	//hhLayout2->setAlignment(Qt::AlignBottom);
//	hhLayout2->addWidget(pbA);
//
//
//	vvLayout->addSpacing(100);
//	vvLayout->addLayout(hhLayout);
//	vvLayout->addLayout(hhLayout2);
//
//
//	connect(pbA, &QPushButton::clicked, [this] {
//		});
//}