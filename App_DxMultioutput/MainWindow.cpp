#include "MainWindow.h"
#include <QApplication>
#include <QMimeData>
#include <QLayout>
#include <QDebug>
#include <QTimer>
#include <thread>

#include <Windows.h>
#include <QProcess>


MainWindow::MainWindow(QWidget* parent) 
	: QMainWindow(parent)
	, renderWindow{ new RenderWindow{} }
	, dxRenderer{ new Direct2DRenderer{} }
{
	QWidget* centralWidget = new QWidget(this);
	centralWidget->setStyleSheet("QWidget{border: 1px solid green;}");

	QVBoxLayout* vLayout = new QVBoxLayout(centralWidget);
	centralWidget->setLayout(vLayout);
	setCentralWidget(centralWidget);

	//QWidget* renderWidget = new QWidget(this);
	renderWindowWidget = QWidget::createWindowContainer(renderWindow.get());
	renderWindowWidget->setAttribute(Qt::WA_NoBackground);
	renderWindowWidget->setStyleSheet("QWidget{border: 2px solid gray;}");
	renderWindowWidget->setFixedWidth(1920);
	renderWindowWidget->setFixedHeight(1080);
	vLayout->addWidget(renderWindowWidget);
	vLayout->setAlignment(renderWindowWidget, Qt::AlignmentFlag::AlignHCenter);


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
		this->Render();
		int xxx = 9;
		});

	FrameConfigurationData frameConfiguration;
	frameConfiguration.startX = 0;
	frameConfiguration.startY = 0;
	frameConfiguration.size.width = 1920;
	frameConfiguration.size.height = 1080;
	frameConfiguration.pixelFormat = PixelFormat::BGRA32;

	dxRenderer->SetFrameConfiguration(frameConfiguration);
	dxRenderer->SetNeedTextureHandler([] {
		});
	dxRenderer->SetHwnd((HWND)renderWindowWidget->winId());
	dxRenderer->SetWindowSize(frameConfiguration.size);
	dxRenderer->Start();
}

void MainWindow::Render() {
	dxRenderer->AddFrameChunk(std::move(frameLocal), 0, 1);
}