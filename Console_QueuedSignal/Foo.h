#pragma once
#include <QObject>
#include "QueuedSignal.h"

class FooPublicSignals;
class FooPrivateSignals;

class Foo : public QObject, public QtConnectionHelper<Qt::QueuedConnection, FooPublicSignals, FooPrivateSignals> {
	Q_OBJECT

public:
	Foo();

	void DoWork();
};


class FooPublicSignals : public QObject {
	Q_OBJECT

signals:
	void Hello(QString);
};

class FooPrivateSignals : public QObject {
	Q_OBJECT

signals:
	void Error(int);
};