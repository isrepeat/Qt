#include "MainWindow.h"
#include <Windows.h>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include "CustomShadowEffect.h"
#include <QLayout>
#include <QDebug>
#include <QTimer>
#include <HelpersQt.h>
#include <thread>

const QString styleButton =
	"font-family: \"Segoe UI\";"
	"font-size: 18px;"
	//"width: 280px;"
	//"height: 32px;"
	"background-color: #644CFF;"
	"color: #ffffff;"
	"border: 0px;";

const QString styleButtonHover =
	"border-left: 10px solid black;"
	"border-top: 10px solid black;"
	"border-right: 10px solid black;"
	"border-bottom: 10px solid black;"
	"border-left-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop: 0 rgb(195,195,195), stop: 1 rgb(240, 240, 240));"
	"border-top-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop: 0 rgb(195,195,195), stop: 1 rgb(240, 240, 240));"
	"border-right-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop: 0 rgb(240, 240, 240), stop: 1 rgb(195,195,195));"
	"border-bottom-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop: 0 rgb(240, 240, 240), stop: 1 rgb(195,195,195));"
	"background-color: rgb(240, 240, 240);"
	;


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
	innerWidget->setFixedHeight(400);
	vLayout->addWidget(innerWidget);

	auto vvLayout = new QVBoxLayout(innerWidget);
	auto hhLayout = new QHBoxLayout();



	////auto effect = QGraphicsDropShadowEffect(offset = QPoint(3, 3), blurRadius = 25, color = QColor("#111"))
	//auto effect = new QGraphicsDropShadowEffect();
	//effect->setOffset(QPoint(0,0));
	//effect->setBlurRadius(50);
	//effect->setColor(QColor("#644CFF"));

	QColor colorPurple("#644CFF");

	auto bodyShadow =  std::make_unique<CustomShadowEffect>();
	//auto bodyShadow = new CustomShadowEffect();
	//bodyShadow->setDpiScaleFactor(devicePixelRatioF());
	bodyShadow->setBorder(10);
	bodyShadow->setBlurRadius(30);
	//bodyShadow->setDistance(15.0);
	//bodyShadow->setColor(QColor(0, 0, 0, 80));
	bodyShadow->setColor(colorPurple);

	pushBtn = new CustomButton("Button");
	pushBtn->setStyleSheet(
		"QPushButton {" + styleButton + "}"
		//"QPushButton:hover {" + styleButtonHover + "}"
	);
	pushBtn->setFixedSize(QSize{ 280, 32 });
	pushBtn->SetOnHoverEffect(std::move(bodyShadow));
	//pushBtn->SetOnHoverEffect(bodyShadow);
	hhLayout->addWidget(pushBtn);

	auto widget = new QWidget(this);
	widget->setStyleSheet(
		"QWidget {" + styleButton + "}"
		"QWidget:hover {" + styleButtonHover + "}"
	);
	hhLayout->addWidget(widget);

	widget->setAutoFillBackground(true);
	//widget->setGraphicsEffect(nullptr);
	//widget->setGraphicsEffect(bodyShadow.get());
	//widget->setGraphicsEffect(bodyShadow);
	//widget->setGraphicsEffect(bodyShadow);
	widget->setFixedSize(QSize{ 400, 40 });
	//widget->setFixedSize(QSize{ 1000, 100 });

	vvLayout->addSpacing(100);
	vvLayout->addLayout(hhLayout);

	connect(pushBtn, &QPushButton::clicked, [=] {
		//bodyShadow->setEnabled(false);
		//bodyShadow->setBorder(40);
		});
}