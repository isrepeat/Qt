#include "MainWindow.h"
#include <QApplication>
#include <QMimeData>
#include <QLayout>
#include <QDebug>
#include <QTimer>
#include <QShowEvent>
#include <QHideEvent>
#include <thread>

#include <Windows.h>
#include <QProcess>


MainWindow::MainWindow(QWidget* parent) 
	: QMainWindow(parent)
	, renderingNode_1{ new RenderingNode{} }
	, renderingNode_2{ new RenderingNode{} }
{
	//this->setVisible(false);
	//setAttribute(Qt::WA_TranslucentBackground);
	//setWindowFlags(Qt::FramelessWindowHint);

	QWidget* centralWidget = new QWidget(this);
	centralWidget->setStyleSheet("QWidget{border: 1px solid green;}");

	QVBoxLayout* vLayout = new QVBoxLayout(centralWidget);
	centralWidget->setLayout(vLayout);
	setCentralWidget(centralWidget);

	auto renderWindowWidget_1 = QWidget::createWindowContainer(renderingNode_1->GetRenderWindow());
	//renderWindowWidget_1->setAttribute(Qt::WA_NoBackground);
	renderWindowWidget_1->setStyleSheet("QWidget{border: 2px solid gray;}");
	renderWindowWidget_1->setFixedWidth(800);
	renderWindowWidget_1->setFixedHeight(600);
	vLayout->addWidget(renderWindowWidget_1);
	vLayout->setAlignment(renderWindowWidget_1, Qt::AlignmentFlag::AlignHCenter);

	vLayout->addSpacing(100);

	auto renderWindowWidget_2 = QWidget::createWindowContainer(renderingNode_2->GetRenderWindow());
	renderWindowWidget_2->setAttribute(Qt::WA_NoBackground);
	renderWindowWidget_2->setStyleSheet("QWidget{border: 2px solid gray;}");
	renderWindowWidget_2->setFixedWidth(800);
	renderWindowWidget_2->setFixedHeight(600);
	vLayout->addWidget(renderWindowWidget_2);
	vLayout->setAlignment(renderWindowWidget_2, Qt::AlignmentFlag::AlignHCenter);


	QWidget* innerWidget = new QWidget(this);
	innerWidget->setStyleSheet("QWidget{border: 2px solid red;}");
	innerWidget->setFixedHeight(200);
	vLayout->addWidget(innerWidget);

	auto vvLayout = new QVBoxLayout(innerWidget);
	auto hhLayout = new QHBoxLayout();
	auto hhLayout2 = new QHBoxLayout();


	auto lbA = new QLabel("Some text label");
	lbA->setStyleSheet("QLabel{color: blue; border: 2px solid blue;}");
	//hhLayout->setAlignment(Qt::AlignBottom);
	hhLayout->addWidget(lbA);


	auto pbA = new QPushButton("Some button text");
	pbA->setStyleSheet("QPushButton{color: brown; border: 2px solid brown;}");
	//hhLayout2->setAlignment(Qt::AlignBottom);
	hhLayout2->addWidget(pbA);


	//vvLayout->addSpacing(100);
	vvLayout->addLayout(hhLayout);
	vvLayout->addLayout(hhLayout2);


	connect(pbA, &QPushButton::clicked, [this] {
		int xxx = 9;
		});

	FrameConfigurationData frameConfiguration;
	frameConfiguration.startX = 0;
	frameConfiguration.startY = 0;
	frameConfiguration.size.width = 1920;
	frameConfiguration.size.height = 1080;
	frameConfiguration.pixelFormat = PixelFormat::BGRA32;
	
	renderingNode_1->Init(frameConfiguration, { Color{30,30,30}, Color{90,90,90}, Color{150,150,150} });
	renderingNode_2->Init(frameConfiguration, { Color{0,30,0}, Color{0,90,0}, Color{0,150,0} });


	
}

void MainWindow::showEvent(QShowEvent* event) {
	//noParentWindow.resize(400, 400);
	//noParentWindow.show();
}

void MainWindow::hideEvent(QHideEvent* event) {
	int xxx = 9;
}