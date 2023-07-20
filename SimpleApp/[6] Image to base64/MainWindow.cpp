#include "MainWindow.h"
#include <HelpersQt.h>
#include <QApplication>
#include <QMimeData>
#include <QLayout>
#include <QDebug>
#include <QBuffer>
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

	auto pbA = new QPushButton("...");
	pbA->setStyleSheet("QPushButton{color: brown; border: 2px solid brown;}");
	hhLayout2->addWidget(pbA);


	




	vvLayout->addSpacing(100);
	vvLayout->addLayout(hhLayout);
	vvLayout->addLayout(hhLayout2);


	connect(pbA, &QPushButton::clicked, [=] {
		QBuffer buffer;
		buffer.open(QIODevice::WriteOnly);
		QImage image("icon.png");

		image.save(&buffer, "PNG");
		QByteArray base64Image = buffer.data().toBase64();

		qDebug() << "base64 image = " << base64Image;

		QImage decodedImage;
		decodedImage.loadFromData(QByteArray::fromBase64(base64Image));

		pbA->setIcon(QIcon(QPixmap::fromImage(decodedImage)));
		});
}