#pragma once
#include <QObject>

template <typename ReturnType, typename ClassType, typename... Args>
struct _FunctionTraitsBase {
	enum {
		ArgumentCount = sizeof...(Args)
	};
	static constexpr bool IsPointerToMemberFunction = !std::is_same_v<ClassType, void>;

	using Ret = ReturnType;
	using Class = ClassType;
	using Function = Ret(Class::*) (Args...);
	using Arguments = std::tuple<Args...>;

	template <size_t i>
	struct arg {
		using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
		// the i-th argument is equivalent to the i-th tuple element of a tuple
		// composed of those arguments.
	};
};


// Matches when T=lambda or T=Functor
// For generic types, directly use the result of the signature of its 'operator()'
template <typename T>
struct FunctionTraits : public FunctionTraits<decltype(&T::operator())>
{};

// For Functor L-value or R-value
template <typename R, typename C, typename... A>
struct FunctionTraits<R(C::*)(A...)> : public _FunctionTraitsBase<R, C, A...>
{};

// For lambdas (need const)
template <typename R, typename C, typename... A>
struct FunctionTraits<R(C::*)(A...) const> : public _FunctionTraitsBase<R, C, A...>
{};

// For C-style functions
template <typename R, typename... A>
struct FunctionTraits<R(*)(A...)> : public _FunctionTraitsBase<R, void, A...>
{};


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
	enum class ConnectFlags {
		None = 0x00,
		DisconnectAfterInvoke = 0x01,
	};

	// connect to lambda [implementation]
	template <typename TObject, typename R, typename... A, typename Fn>
	void ConnectInternal(TObject* sender, R(TObject::* signal)(A...), const QObject* context, Fn lambda, Qt::ConnectionType cType, ConnectFlags cFlags) {
		QObject::connect(sender, signal, context ? context : sender, [=](A... args) {
			if (cFlags == ConnectFlags::DisconnectAfterInvoke) {
				QObject::disconnect(sender, signal, nullptr, nullptr);
			}
			return lambda(std::forward<A>(args)...);
			}, cType);
	}

	// connect to lambda [wrapper to expand TSignal]
	template <typename TObject, typename TSignal, typename Fn>
	void Connect(TObject* sender, TSignal signal, const QObject* context, Fn lambda,
		Qt::ConnectionType cType = Qt::ConnectionType::AutoConnection, ConnectFlags cFlags = ConnectFlags::None)
	{
		// NOTE: 
		// - FunctionPointer<TSignal>::Object - get real function member owner.
		// - Not pass TSignal explicitly to avoid compile errors (TObject may be ambiguous).
		ConnectInternal<typename QtPrivate::FunctionPointer<TSignal>::Object>(sender, signal, context, lambda, cType, cFlags);
	}
}