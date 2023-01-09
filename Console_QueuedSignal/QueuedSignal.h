#pragma once
#include <QObject>
#include <QDebug>
#include <QTimer>
#include <memory>
#include <QObject>
#include <memory>

class EmptyPublicSignals : public QObject {
	Q_OBJECT
};

class EmptyPrivateSignals : public QObject {
	Q_OBJECT
};

// Force connect publicSignals as specified defaultConnectionType
template <Qt::ConnectionType defaultConnectionType, typename TPublic, typename TPrivate = EmptyPrivateSignals>
class QtConnectionHelper {
public:
	QtConnectionHelper()
		: publicSignals{ new TPublic() }
		, privateSignals{ new TPrivate() }
	{}
	~QtConnectionHelper() = default;


	// connect to functor
	template <typename R, typename... A, typename Fn>
	void Connect(R(TPublic::* signal)(A...), Fn slot) {
		QObject::connect(publicSignals.get(), signal, publicSignals.get(), slot, defaultConnectionType);
	}

	// connect to functor
	template <typename R, typename... A, typename Fn>
	void Connect(R(TPublic::* signal)(A...), const QObject* context, Fn slot) {
		QObject::connect(publicSignals.get(), signal, context, slot, defaultConnectionType);
	}

	// connect to class member
	template <typename R, typename... A, typename Fn>
	void Connect(R(TPublic::* signal)(A...), const typename QtPrivate::FunctionPointer<Fn>::Object* receiver, Fn slot) {
		QObject::connect(publicSignals.get(), signal, receiver, slot, defaultConnectionType);
	}

protected:
	std::unique_ptr<TPublic> publicSignals;
	std::unique_ptr<TPrivate> privateSignals; // connect manually
};



class BarPrivateSignals : public QObject {
	Q_OBJECT

signals:
	void One(int);
	void Two(float);
};

class BarPublicSignals : public QObject {
	Q_OBJECT

signals:
	void Three();
	void Four(int, int);
};



class FooPrivateSignals : public QObject {
	Q_OBJECT

signals:
	void One(int);
	void Two(float);
};

class FooPublicSignals : public QObject {
	Q_OBJECT

signals:
	void Three();
	void Four(int, int);
};


class Foo : public QObject, protected QtConnectionHelper<Qt::QueuedConnection, FooPublicSignals, FooPrivateSignals> {
	Q_OBJECT

public:
	Foo();

	void DoWork();
};


class Bar : public QObject {
	Q_OBJECT

public:
	Bar() = default;

public slots:
	void Handler(int type);
};