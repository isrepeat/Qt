#pragma once
#include <QObject>
#include "QueuedSignal.h"

class BarPublicSignals : public QObject {
	Q_OBJECT

signals:
	void Hello(QString);
};

class BarPrivateSignals : public QObject {
	Q_OBJECT

signals:
	void Error(int);
};


class Bar : public QObject {
	Q_OBJECT

public:
	Bar() = default;

//public slots:
//	void Handler(int type);
};