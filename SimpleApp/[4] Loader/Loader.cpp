#include "Loader.h"
#include <QLayout>
#include <QScreen>
#include <QGuiApplication>
#include "../HelpersQt.h"

Loader::Loader(const QString& filename, QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);

	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->setAlignment(Qt::AlignHCenter);
	setLayout(vLayout);

    movie = new QMovie(filename, QByteArray(), this);
    movie->setScaledSize(QSize(50, 50));

    label = new QLabel();
    label->setMovie(movie);
	label->setFixedSize(QSize(50,50));
	//HelpersQt::SetStyleSheetFor({ label }, "QLabel", "color: gray; border: 2px solid gray;");
	vLayout->addWidget(label);
}

void Loader::Show() {
    show();
    label->show();
    movie->start();
    MoveToCenter();
}

void Loader::Hide() {
    movie->stop();
    label->hide();
    hide();
}

// NOTE: need call after show to obtain correct width & height
void Loader::MoveToCenter() {
    QWidget* parent = parentWidget();
    if (parent) {
        this->move((parent->width() - this->width()) / 2, (parent->height() - this->height()) / 2);
    }
    else {
        auto currentScreen = QGuiApplication::primaryScreen();
        this->move((currentScreen->availableGeometry().width() - this->width()) / 2, (currentScreen->availableGeometry().height() - this->height()) / 2);
    }
}