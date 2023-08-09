#include "MainWindow.h"
#include "PopUpMessage.h"
#include <QApplication>
#include <QMimeData>
#include <QLayout>
#include <QDebug>
#include <QTimer>
#include <thread>
#include <Windows.h>
#include <QProcess>
#include <QDeferred/QDeferred>

AppFeatures& AppFeatures::GetInstance() {
	static AppFeatures instance;
	return instance;
}

AppFeatures::AppFeatures() 
{
}

void AppFeatures::Hello() {
	//PopUpMessage::ShowMessage("H E L L O");
}


MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	QWidget* centralWidget = new QWidget(this);
	centralWidget->setStyleSheet("QWidget{border: 1px solid green;}");

	QObject::connect(this, &MainWindow::Exit, qApp, [this] {
		QApplication::quit();
		}, Qt::QueuedConnection);


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
		//PopUpMessager::ShowMessage("O N E");
		//PopUpMessager::ShowMessage("T W O");
		//PopUpMessager::ShowMessage("T H R E E"); 


		// init
		QDefer defer1;
		// reject before
		defer1.reject(); // reject here
		// subscribe then callback
		defer1.then<int>([]() {
			QDeferred<int> defer2;
			//REQUIRE(false);
			// return deferred
			return defer2;
			}, []() { // fail over 'defer1' with zero arguments
				// test that gets called
				//REQUIRE(true);
			}).fail([](int val) {
				// fail over 'defer2', which is not resolved nor rejected, therefore not called
				Q_UNUSED(val);
				//REQUIRE(false);
				});




		//QTimer::singleShot(500, [this] { 
		//	PopUpMessager::ShowMessage("T W O"); 

		//	QTimer::singleShot(500, [this] { 
		//		
		//		QTimer::singleShot(500, [this] {
		//			PopUpMessager::ShowMessage("F O U R");
		//			});
		//		});
		//	});
		


		//QTimer::singleShot(2000, [this] {
		//	emit Exit();
		//	});
		});
}

MainWindow::~MainWindow() {
	int xxx = 9;
}
