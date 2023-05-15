#if LAYOUTS == 1
#include "MainWindow.h"
#include <HelpersQt.h>
#include <QApplication>
#include <QMimeData>
#include <QLayout>
#include <QDebug>
#include <QTimer>
#include <thread>
#include <Windows.h>


void ClearLayout(QLayout* layout) {
	if (layout == nullptr)
		return;

	QLayoutItem* item;
	while ((item = layout->takeAt(0))) {
		if (item->layout()) {
			ClearLayout(item->layout());
			delete item->layout();
		}
		if (item->widget()) {
			delete item->widget();
		}
		delete item;
	}
}

MainWindow::MainWindow(QWidget* parent) 
	: QMainWindow(parent) 
{
	QWidget* centralWidget = new QWidget(this);
	HelpersQt::SetStyleSheetFor({ centralWidget }, "QWidget", "border: 1px solid green;");

	QVBoxLayout* vLayout = new QVBoxLayout(centralWidget);
	centralWidget->setLayout(vLayout);
	setCentralWidget(centralWidget);

	QWidget* innerWidget = new QWidget(this);
	HelpersQt::SetStyleSheetFor({ innerWidget }, "QWidget", "border: 2px solid red;");
	innerWidget->setFixedHeight(200);
	vLayout->addWidget(innerWidget);

	auto vvLayout = new QVBoxLayout(innerWidget);
	auto hhLayout = new QHBoxLayout();
	auto hhLayout2 = new QHBoxLayout();


	auto lbA = new QLabel();
	lbA->setText("link = <a href=\"https://google.com/\"><span>Click Here!</span></a>");
	//lbA->setTextFormat(Qt::RichText);
	lbA->setTextInteractionFlags(Qt::TextBrowserInteraction);
	lbA->setOpenExternalLinks(true);
	HelpersQt::SetStyleSheetFor({ lbA }, "QLabel", "color: gray; border: 2px solid gray;");
	HelpersQt::SetStyleSheetFor({ lbA }, "span", "color: red;");

	auto lbB = new QLabel("label B");
	auto lbC = new QLabel("label C");


	hhLayout->addWidget(lbA);
	hhLayout->addSpacing(30);
	hhLayout->addWidget(lbB);
	hhLayout->addSpacing(30);
	hhLayout->addWidget(lbC);


	auto pbA = new QPushButton("Some button text");
	pbA->setStyleSheet("QPushButton{color: brown; border: 2px solid brown;}");
	//hhLayout2->setAlignment(Qt::AlignBottom);
	hhLayout2->addWidget(pbA);


	vvLayout->addSpacing(100);
	vvLayout->addLayout(hhLayout);
	vvLayout->addLayout(hhLayout2);


	connect(pbA, &QPushButton::clicked, [=] {
		ClearLayout(hhLayout);
		auto lbD = new QLabel("label D");
		auto lbE = new QLabel("label E");
		hhLayout->addWidget(lbD);
		hhLayout->addSpacing(70);
		hhLayout->addWidget(lbE);
		});
}
#endif