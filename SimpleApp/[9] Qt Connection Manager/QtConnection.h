#pragma once
#include <QObject>
#include <Helpers.h>

/// <summary>
/// Class provides BaseSignal struct and Connect(...) method to connect typical qt signal by specified connection type.
/// Usage: QtConnection::BaseSignal<int> SomeSignal(int);
/// </summary>
class QtConnection {
private:
	template <typename Ret, Qt::ConnectionType Ct>
	struct BaseSignal {
		Ret result;
	};
	template <Qt::ConnectionType Ct>
	struct BaseSignal<void, Ct> {};

public:
	template<typename R> using Auto = QtConnection::BaseSignal<R, Qt::AutoConnection>;
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


namespace Hqt {
	enum ConnectFlags { // declare without 'class' to simplify flags concatenation
		None = 0x00,
		AutoDisconnect = 0x01,
		// Here may be flags associated with "optional value"
	};

	// Connect to lambda [implementation]
	template <typename TObject, typename R, typename... A, typename Fn, typename TOptional>
	void ConnectInternal(TObject* sender, R(TObject::* signal)(A...), const QObject* context, Fn lambda,
		Qt::ConnectionType cType, H::Flags<ConnectFlags> cFlags, TOptional&& optionalValue)
	{
		auto moveLambda = H::create_move_lambda(std::move(optionalValue), [sender, signal, lambda, cFlags](H::moved_value<TOptional> movedOptValueRef, A... args) {
			auto optValue = std::move(movedOptValueRef.get()); // it may be custom type that use RAII idiom

			if (cFlags & ConnectFlags::AutoDisconnect) {
				QObject::disconnect(sender, signal, nullptr, nullptr);
			}
			return lambda(std::forward<A>(args)...);
			});

		QObject::connect(sender, signal, context ? context : sender, std::move(moveLambda), cType);
	}

	// Connect to lambda [wrapper to expand TSignal]
	template <typename TObject, typename TSignal, typename Fn, typename TOptional = void*>
	void Connect(TObject* sender, TSignal signal, const QObject* context, Fn lambda,
		Qt::ConnectionType cType = Qt::ConnectionType::AutoConnection, H::Flags<ConnectFlags> cFlags = ConnectFlags::None, TOptional&& optionalValue = nullptr)
	{
		// NOTE: 
		// - FunctionPointer<TSignal>::Object - get real function member owner.
		// - Not pass TSignal explicitly to avoid compile errors (TObject may be ambiguous).
		ConnectInternal<typename QtPrivate::FunctionPointer<TSignal>::Object>(sender, signal, context, lambda, cType, cFlags, std::forward<TOptional&&>(optionalValue));
	}

	template <typename TObject, typename TSignal, typename Fn, typename TOptional = void*>
	void ConnectOnce(TObject* sender, TSignal signal, const QObject* context, Fn lambda,
		Qt::ConnectionType cType = Qt::ConnectionType::AutoConnection, H::Flags<ConnectFlags> cFlags = ConnectFlags::None, TOptional&& optionalValue = nullptr)
	{
		ConnectInternal<typename QtPrivate::FunctionPointer<TSignal>::Object>(sender, signal, context, lambda, cType, cFlags | ConnectFlags::AutoDisconnect, std::forward<TOptional&&>(optionalValue));
	}
}