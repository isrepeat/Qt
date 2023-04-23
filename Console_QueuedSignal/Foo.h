#pragma once
#include <QObject>
#include <functional>
#include "QueuedSignal.h"
#include <map>
#include <optional>



class QtEmptySignals : public QObject {
	Q_OBJECT
};
#define QT_EMPTY_SIGNALS QtEmptySignals



template <typename TPublic, typename TPrivate = QT_EMPTY_SIGNALS>
class QtConnectionHelper {
public:
	QtConnectionHelper()
		: publicSignals{ new TPublic() }
		, privateSignals{ new TPrivate() }
	{}
	~QtConnectionHelper() = default;

	// connect to lambda
	template <typename R, typename... A, typename Fn>
	void Connect(R(TPublic::* signal)(A...), const QObject* context, Fn lambda) {
		QtConnection::Connect(publicSignals.get(), signal, context, lambda);
	}

	// connect to class member
	template <typename R, typename... A, typename Fn>
	void Connect(R(TPublic::* signal)(A...), /*const*/ typename QtPrivate::FunctionPointer<Fn>::Object* receiver, Fn slot) {
		QtConnection::Connect(publicSignals.get(), signal, receiver, slot);
	}

protected:
	template <typename R, typename... A, typename Fn>
	void Connect(R(TPrivate::* signal)(A...), const QObject* context, Fn lambda) {
		QtConnection::Connect(privateSignals.get(), signal, context, lambda);
	}

	template <typename R, typename... A, typename Fn>
	void Connect(R(TPrivate::* signal)(A...), /*const*/ typename QtPrivate::FunctionPointer<Fn>::Object* receiver, Fn slot) {
		QtConnection::Connect(privateSignals.get(), signal, receiver, slot);
	}

protected:
	std::unique_ptr<TPublic> publicSignals;
	std::unique_ptr<TPrivate> privateSignals; // connect manually
};



class Second : public QObject {
	Q_OBJECT

public slots:
	float ErrorSlot(int value);

};


class TempPublicSignals;
class TempPrivateSignals;

class Temp : public QObject, public QtConnectionHelper<QT_EMPTY_SIGNALS, TempPrivateSignals> {
	Q_OBJECT

public:
	Temp();
	void EmitSignal();

signals:
	void Foo(int);

private:
	Second second;
};


class TempPublicSignals : public QObject {
	Q_OBJECT
signals:
	QtConnection::Queued<void> ErrorPublic(int);
};

class TempPrivateSignals : public QObject {
	Q_OBJECT
signals:
	QtConnection::Blocked<float> ErrorPrivate(int);
};