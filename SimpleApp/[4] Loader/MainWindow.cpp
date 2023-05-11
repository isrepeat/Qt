#include "MainWindow.h"
#include "../HelpersQt.h"
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


	//loader = new Loader("D:\\loader.gif", this);
	loader = new Loader("D:\\loader.gif");
	
	//hhLayout->addWidget(loader);
	//hhLayout->addSpacing(30);



	auto pbA = new QPushButton("Show lodaer");
	pbA->setStyleSheet("QPushButton{color: brown; border: 2px solid brown;}");
	hhLayout2->addWidget(pbA);


	vvLayout->addSpacing(100);
	vvLayout->addLayout(hhLayout);
	vvLayout->addLayout(hhLayout2);


	connect(pbA, &QPushButton::clicked, [=] {
		if (loaderShown) {
			loader->Hide();
			pbA->setText("Show lodaer");
		}
		else {
			loader->Show();
			pbA->setText("Hide lodaer");
		}
		loaderShown = !loaderShown;
		});
}