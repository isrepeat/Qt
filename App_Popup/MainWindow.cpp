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

std::unique_ptr<PopUpMessage> g_popUpMessage;

AppFeatures& AppFeatures::GetInstance() {
	static AppFeatures instance;
	return instance;
}

AppFeatures::AppFeatures() 
{
}

void AppFeatures::Hello() {
#ifdef SINGLETON
	PopUpMessage::ShowMessage("H E L L O");
#else
	popupMessage.ShowMessage("H E L L O");
#endif
}


MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	QWidget* centralWidget = new QWidget(this);
	centralWidget->setStyleSheet("QWidget{border: 1px solid green;}");

	//g_popUpMessage = std::make_unique<PopUpMessage>();

	//QObject::connect(this, &MainWindow::Exit, qApp, &QApplication::quit);
	QObject::connect(qApp, &QCoreApplication::aboutToQuit, this, [this] {
		int xx = 9;
		});


	QObject::connect(this, &MainWindow::Exit, qApp, [this] {
		QApplication::quit();
		//QApplication::exit(0);
		//exit(0);
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
		//PopUpMessage::ShowMessage("Added to clipboard");
		AppFeatures::GetInstance().Hello();

		//{
		//	PopUpMessage* popUpMessage = new PopUpMessage();
		//	popUpMessage->ShowMessage("AAA BBB");
		//}

		//g_popUpMessage->ShowMessage("Global");
		////g_popUpMessage->deleteLater();

		//m_popUpMessage.ShowMessage("Member");

		QTimer::singleShot(1'500, [this] {
			emit Exit();
			});
		});
}

MainWindow::~MainWindow() {
	int xxx = 9;
}
