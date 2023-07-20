#include "MainWindow.h"
#include "../HelpersQt/HelpersQt.h"
#include <QApplication>
#include <QMimeData>
#include <QLayout>
#include <QDebug>
#include <QTimer>
#include <thread>
#include <Windows.h>


MainWindow::MainWindow(QWidget* parent) 
	: QMainWindow(parent) 
{
	QWidget* centralWidget = new QWidget(this);
	HelpersQt::SetStyleSheetFor({ centralWidget }, "QWidget", "border: 1px solid green;");

	QVBoxLayout* vLayout = new QVBoxLayout(centralWidget);
	centralWidget->setLayout(vLayout);
	setCentralWidget(centralWidget);

	QWidget* innerWidget = new QWidget(this);
	HelpersQt::SetStyleSheetFor({ innerWidget }, "QWidget", "border: 2px solid lightgray;");
	innerWidget->setFixedHeight(200);
	vLayout->addWidget(innerWidget);

	auto vvLayout = new QVBoxLayout(innerWidget);
	auto hhLayout = new QHBoxLayout();
	auto hhLayout2 = new QHBoxLayout();
	auto hhLayout3 = new QHBoxLayout();

	// HLayout 1
	label = new QLabel();
	label->setText("some text label");
	HelpersQt::SetStyleSheetFor({ label }, "QLabel", "color: gray; border: 2px solid gray;");
	hhLayout->addWidget(label);


	// HLayout 2
	btnMonitorA = new QPushButton("Button A");
	btnMonitorA->setStyleSheet("QPushButton{color: red; border: 2px solid gray;}");
	btnMonitorB = new QPushButton("Button B");
	btnMonitorB->setStyleSheet("QPushButton{color: green; border: 2px solid gray;}");
	btnMonitorC = new QPushButton("Button C");
	btnMonitorC->setStyleSheet("QPushButton{color: blue; border: 2px solid gray;}");
	hhLayout2->addWidget(btnMonitorA);
	hhLayout2->addWidget(btnMonitorB);
	hhLayout2->addWidget(btnMonitorC);


	// HLayout 3
	btnPlusA = new QPushButton("++[A]++");
	btnPlusA->setStyleSheet("QPushButton{color: black; border: 2px solid red;}");
	btnMinusA = new QPushButton("--[A]--");
	btnMinusA->setStyleSheet("QPushButton{color: black; border: 2px solid blue;}");
	hhLayout3->addWidget(btnPlusA);
	hhLayout3->addWidget(btnMinusA);
	hhLayout3->addSpacing(10);

	btnPlusB = new QPushButton("++[B]++");
	btnPlusB->setStyleSheet("QPushButton{color: black; border: 2px solid red;}");
	btnMinusB = new QPushButton("--[B]--");
	btnMinusB->setStyleSheet("QPushButton{color: black; border: 2px solid blue;}");
	hhLayout3->addWidget(btnPlusB);
	hhLayout3->addWidget(btnMinusB);
	hhLayout3->addSpacing(10);

	btnPlusC = new QPushButton("++[C]++");
	btnPlusC->setStyleSheet("QPushButton{color: black; border: 2px solid red;}");
	btnMinusC = new QPushButton("--[C]--");
	btnMinusC->setStyleSheet("QPushButton{color: black; border: 2px solid blue;}");
	hhLayout3->addWidget(btnPlusC);
	hhLayout3->addWidget(btnMinusC);

	vvLayout->addSpacing(100);
	vvLayout->addLayout(hhLayout);
	vvLayout->addSpacing(10);
	vvLayout->addLayout(hhLayout2);
	vvLayout->addSpacing(10);
	vvLayout->addLayout(hhLayout3);

	remoteDesktopWindows.push_back(std::make_shared<RemoteDesktopWindow>());
	remoteDesktopWindows.push_back(std::make_shared<RemoteDesktopWindow>());
	remoteDesktopWindows.push_back(std::make_shared<RemoteDesktopWindow>());


	connect(btnMonitorA, &QPushButton::clicked, [this] {
		currentRemoteDesktopWindows = remoteDesktopWindows[0].get();
		SetupButtonsEvents();
		});

	connect(btnMonitorB, &QPushButton::clicked, [this] {
		currentRemoteDesktopWindows = remoteDesktopWindows[1].get();
		SetupButtonsEvents();
		});

	connect(btnMonitorC, &QPushButton::clicked, [this] {
		currentRemoteDesktopWindows = remoteDesktopWindows[2].get();
		SetupButtonsEvents();
		});

	connect(btnPlusA, &QPushButton::clicked, [this] {
		remoteDesktopWindows[0]->SetSize(remoteDesktopWindows[0]->GetSize() + QSize{ 0, 10 });
		});
	connect(btnMinusA, &QPushButton::clicked, [this] {
		remoteDesktopWindows[0]->SetSize(remoteDesktopWindows[0]->GetSize() - QSize{ 0, 10 });
		});

	connect(btnPlusB, &QPushButton::clicked, [this] {
		remoteDesktopWindows[1]->SetSize(remoteDesktopWindows[1]->GetSize() + QSize{ 0, 10 });
		});
	connect(btnMinusB, &QPushButton::clicked, [this] {
		remoteDesktopWindows[1]->SetSize(remoteDesktopWindows[1]->GetSize() - QSize{ 0, 10 });
		});

	connect(btnPlusC, &QPushButton::clicked, [this] {
		remoteDesktopWindows[2]->SetSize(remoteDesktopWindows[2]->GetSize() + QSize{ 0, 10 });
		});
	connect(btnMinusC, &QPushButton::clicked, [this] {
		remoteDesktopWindows[2]->SetSize(remoteDesktopWindows[2]->GetSize() - QSize{ 0, 10 });
		});
}

void MainWindow::SetupButtonsEvents() {
	connect(currentRemoteDesktopWindows, &RemoteDesktopWindow::NewSize, [this] (QSize size){
		label->setText(QString("current size = { %1; %2 }").arg(size.width()).arg(size.height()));
		});
}