#include "MainWindow.h"
#include <QApplication>
#include <QMimeData>
#include <QLayout>
#include <QClipboard>
#include <QDebug>
#include <QTimer>
#include <thread>

#include <Windows.h>
#include <QProcess>


MainWindow::MainWindow(QWidget* parent) 
	: QMainWindow(parent) 
{
	QWidget* centralWidget = new QWidget(this);
	centralWidget->setStyleSheet("QWidget{border: 1px solid green;}");

	QVBoxLayout* vLayout = new QVBoxLayout(centralWidget);
	centralWidget->setLayout(vLayout);
	setCentralWidget(centralWidget);

	QWidget* innerWidget = new QWidget(this);
	innerWidget->setStyleSheet("QWidget{border: 2px solid red;}");
	innerWidget->setFixedHeight(200);
	vLayout->addWidget(innerWidget);

	auto vvLayout = new QVBoxLayout(innerWidget);
	auto hhLayout = new QHBoxLayout();
	auto hhLayout2 = new QHBoxLayout();


	label = new QLabel("Some text label");
	label->setStyleSheet("QLabel{color: blue; border: 2px solid blue;}");
	//hhLayout->setAlignment(Qt::AlignBottom);
	hhLayout->addWidget(label);


	pushBtn = new QPushButton("Some button text");
	pushBtn->setStyleSheet("QPushButton{color: brown; border: 2px solid brown;}");
	//hhLayout2->setAlignment(Qt::AlignBottom);
	hhLayout2->addWidget(pushBtn);


	//vvLayout->addSpacing(100);
	vvLayout->addLayout(hhLayout);
	vvLayout->addLayout(hhLayout2);


	connect(pushBtn, &QPushButton::clicked, [this] {
		});


	connect(QGuiApplication::clipboard(), &QClipboard::changed, this, [this] {
		popUpMessage.ShowMessage("Added to Clipboard");

		auto clipboard = QGuiApplication::clipboard();
		auto mime = clipboard->mimeData();
		auto formats = mime->formats();
		auto text = clipboard->text();
		auto list = text.split("file:///");

		if (mime->hasImage())
		{
			QImage img(clipboard->image()); label->setPixmap(QPixmap::fromImage(img));
			//label->setPixmap(clipboard->pixmap().scaled(300,300, Qt::KeepAspectRatio, Qt::TransformationMode::FastTransformation));
		}
		else if (mime->hasText()) {
			int xxx = 9;
		}
		else if (mime->hasUrls()) {
			int xxx = 9;
		}


		//if (list.size() > 1) {	// If found at least one file:///
		//	list.pop_front();	// correct list

		//	std::vector<std::wstring> filesList;
		//	for (auto& file : list) {
		//		filesList.push_back(file.remove('\n').toStdWString());
		//	}

		//	H::FS::CorrectPathWithBackslashes(filesList);

		//	std::visit([&filesList](auto&& ptr) {
		//		if (ptr != nullptr)
		//			ptr->SendFiles(filesList);
		//		}, vncComponent);
		//}
		//else {
		//	std::visit([&text](auto&& ptr) {
		//		if (ptr != nullptr)
		//			ptr->SendText(text.toStdWString().c_str());
		//		}, vncComponent);
		//}
		});
}