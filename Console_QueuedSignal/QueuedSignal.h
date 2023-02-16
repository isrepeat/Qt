#pragma once
#include <QObject>

class QtConnection {
private:
	template <typename Ret, Qt::ConnectionType Ct>
	struct BaseSignal {
		Ret result;
	};
	template <Qt::ConnectionType Ct>
	struct BaseSignal<void, Ct> {};

public:
	template<typename R> using Direct = QtConnection::BaseSignal<R, Qt::DirectConnection>;
	template<typename R> using Queued = QtConnection::BaseSignal<R, Qt::QueuedConnection>;
	template<typename R> using Blocked = QtConnection::BaseSignal<R, Qt::BlockingQueuedConnection>;


	template <typename R, Qt::ConnectionType Ct, typename TObject, typename... A, typename Fn>
	static void Connect(TObject* sender, BaseSignal<R, Ct>(TObject::* signal)(A...), const QObject* context, Fn lambda) {
		ConnectInternal<R, Ct>(sender, signal, context, lambda);
	}

	template <typename R, Qt::ConnectionType Ct, typename TObject, typename... A, typename Fn>
	static void Connect(TObject* sender, BaseSignal<R, Ct>(TObject::* signal)(A...), typename QtPrivate::FunctionPointer<Fn>::Object* receiver, Fn slot) {
		ConnectInternal<R, Ct>(sender, signal, receiver, slot);
	}



private:
	// connect to lambda
	template <typename R, Qt::ConnectionType Ct, typename TSignal, typename TObject, typename... A, typename Fn>
	static void ConnectInternal(TObject* sender, TSignal(TObject::* signal)(A...), const QObject* context, Fn lambda) {
		QObject::connect(sender, signal, context ? context : sender, [=](A... args) {
			if constexpr (std::is_same_v<R, void>) {
				lambda(std::forward<A>(args)...);
			}
			else {
				return TSignal{ lambda(std::forward<A>(args)...) };
			}
			}, Ct);
	}

	// connect to class member
	template <typename R, Qt::ConnectionType Ct, typename TSignal, typename TObject, typename... A, typename Fn>
	static void ConnectInternal(TObject* sender, TSignal(TObject::* signal)(A...), typename QtPrivate::FunctionPointer<Fn>::Object* receiver, Fn slot) {
		QObject::connect(sender, signal, receiver, [=](A... args) {
			if constexpr (std::is_same_v<R, void>) {
				(receiver->*slot)(std::forward<A>(args)...);
			}
			else {
				return TSignal{ (receiver->*slot)(std::forward<A>(args)...) };
			}
			}, Ct);
	}
};


//template<typename R> using Direct = QtConnection::BaseSignal<R, Qt::DirectConnection>;
//template<typename R> using Queued = QtConnection::BaseSignal<R, Qt::QueuedConnection>;
//template<typename R> using Blocked = QtConnection::BaseSignal<R, Qt::BlockingQueuedConnection>;