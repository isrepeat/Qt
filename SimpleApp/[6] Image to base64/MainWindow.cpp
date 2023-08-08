#include "MainWindow.h"
#include <HelpersQt.h>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>
#include <QJsonArray>
#include <QMimeData>
#include <QLayout>
#include <QDebug>
#include <QBuffer>
#include <QTimer>
#include <QFile>
#include <thread>
#include <Windows.h>


struct UserInfo {
	QString id;
	QString accountName;
	QString email;
	QString name;
	QPixmap profilePicture;
	QString additionalData;
};

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

	QVBoxLayout* vLayout = new QVBoxLayout();
	centralWidget->setLayout(vLayout);
	setCentralWidget(centralWidget);

	QWidget* innerWidget = new QWidget(this);
	HelpersQt::SetStyleSheetFor({ innerWidget }, "QWidget", "border: 2px solid red;");
	innerWidget->setFixedHeight(300);
	vLayout->addWidget(innerWidget);

	auto vvLayout = new QVBoxLayout(innerWidget);
	auto hhLayout = new QHBoxLayout();
	auto hhLayout2 = new QHBoxLayout();

	auto lbA = new QLabel("...");
	lbA->setStyleSheet("QLabel{color: brown; border: 2px solid brown;}");
	lbA->setFixedWidth(150);
	lbA->setFixedHeight(150);

	hhLayout->addWidget(lbA);

	auto pbA = new QPushButton("...");
	pbA->setStyleSheet("QPushButton{color: brown; border: 2px solid brown;}");
	hhLayout2->addWidget(pbA);


	//vvLayout->addSpacing(100);
	vvLayout->addLayout(hhLayout);
	vvLayout->addLayout(hhLayout2);


	//connect(pbA, &QPushButton::clicked, [=] {
	//	QBuffer buffer;
	//	buffer.open(QIODevice::WriteOnly);
	//	QImage image("testdctua10_ms_acc_picture.png");

	//	image.save(&buffer, "PNG");
	//	QByteArray base64Image = buffer.data().toBase64();

	//	qDebug() << "base64 image = " << base64Image;

	//	QImage decodedImage;
	//	decodedImage.loadFromData(QByteArray::fromBase64(base64Image));

	//	//pbA->setIcon(QIcon(QPixmap::fromImage(decodedImage)));
	//	pbA->setIcon(QIcon(QPixmap::fromImage(decodedImage).scaled(50,50)));
	//	});


	connect(pbA, &QPushButton::clicked, [=] {
		QFile fileTxt("D:\\UserInfoJson.txt");
		if (fileTxt.exists()) {
			fileTxt.open(QIODevice::ReadOnly);
			auto fileData = fileTxt.readAll();

			auto userInfoJSON = QJsonDocument::fromJson(fileData).object();

			UserInfo userInfo;

			userInfo.id = userInfoJSON["Id"].toString();
			userInfo.name = userInfoJSON["name"].toString();
			userInfo.email = userInfoJSON["email"].toString();
			userInfo.accountName = userInfoJSON["accountName"].toString();
			userInfo.additionalData = userInfoJSON["additionalData"].toString();
			auto base64Image = userInfoJSON["profilePicture"].toString().toUtf8();
			userInfo.profilePicture.loadFromData(QByteArray::fromBase64(userInfoJSON["profilePicture"].toString().toUtf8()));

			QImage decodedImage;
			decodedImage.loadFromData(QByteArray::fromBase64(base64Image));

			//pbA->setIcon(QIcon(QPixmap::fromImage(decodedImage)));
			pbA->setIcon(QIcon(userInfo.profilePicture));
			lbA->setPixmap(userInfo.profilePicture.scaled(lbA->size(), Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation));
		}
		});


}