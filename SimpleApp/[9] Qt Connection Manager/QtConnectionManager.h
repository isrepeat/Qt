#pragma once
#include "QtConnection.h"
#include <memory>

class QtEmptySignals : public QObject {
	Q_OBJECT
};
#define QT_EMPTY_SIGNALS QtEmptySignals

/// <summary>
/// Class contains private and public signal classes and manages their connection. It should be inherited.
/// </summary>
template <typename TPublic, typename TPrivate = QT_EMPTY_SIGNALS>
class QtConnectionManager {
protected:
	QtConnectionManager()
		: publicSignals{ new TPublic() }
		, privateSignals{ new TPrivate() }
	{}
	~QtConnectionManager() = default;

public:
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
	std::unique_ptr<TPrivate> privateSignals;
};
