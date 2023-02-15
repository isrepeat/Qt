#pragma once
#include <QObject>
#include <functional>
#include "QueuedSignal.h"
#include <map>
#include <optional>

//template<typename Type, template<Qt::ConnectionType, typename...> class Args>
//struct is_specialization_of : std::false_type {};
//
//template<template<Qt::ConnectionType, typename...> class SignalType, Qt::ConnectionType Ct, typename... Args>
//struct is_specialization_of<SignalType<Ct, Args...>, SignalType> : std::true_type {};


// TODO: mb incapsulate to QtConnection
template<template<typename, Qt::ConnectionType> typename T, typename U>
struct isDerivedFrom {
private:
	template<typename V, Qt::ConnectionType Ct>
	static decltype(static_cast<const T<V, Ct>&>(std::declval<U>()), std::true_type{})
		test(const T<V, Ct>&);

	static std::false_type test(...);
public:
	static constexpr bool value = decltype(isDerivedFrom::test(std::declval<U>()))::value;
};


class QtConnection {
private:
	template <typename Ret, Qt::ConnectionType connectionType>
	struct BaseSignal {
		Ret result;
		using R = Ret;
		static const Qt::ConnectionType ct = connectionType;
	};

	template <Qt::ConnectionType connectionType>
	struct BaseSignal<void, connectionType> {
		using R = void;
		static const Qt::ConnectionType ct = connectionType;
	};

	template <typename Ret, Qt::ConnectionType connectionType> 
	struct PublicSignal : public BaseSignal<Ret, connectionType> {};

	template <typename Ret, Qt::ConnectionType connectionType>
	struct PrivateSignal : public BaseSignal<Ret, connectionType> {};

public:
	template<typename R> using PublicDirect = QtConnection::PublicSignal<R, Qt::DirectConnection>;
	template<typename R> using PublicQueued = QtConnection::PublicSignal<R, Qt::QueuedConnection>;
	template<typename R> using PublicBlocked = QtConnection::PublicSignal<R, Qt::BlockingQueuedConnection>;

	template<typename R> using PrivateDirect = QtConnection::PrivateSignal<R, Qt::DirectConnection>;
	template<typename R> using PrivateQueued = QtConnection::PrivateSignal<R, Qt::QueuedConnection>;
	template<typename R> using PrivateBlocked = QtConnection::PrivateSignal<R, Qt::BlockingQueuedConnection>;

public:
	//// Qt can't work with "using" because use inheritance
	//template <typename Ret> struct PublicDirect : public PublicSignal<Ret, Qt::DirectConnection> {};
	//template <typename Ret> struct PublicQueued : public PublicSignal<Ret, Qt::QueuedConnection> {};
	//template <typename Ret> struct PublicBlocked : public PublicSignal<Ret, Qt::BlockingQueuedConnection> {};

	////// connect to functor
	////template <typename TObject, Qt::ConnectionType Ct, typename R, typename... A, typename Fn>
	////static void Connect(TObject* sender, Public<Ct, R>(TObject::* signal)(A...), const QObject* context, Fn lambda) {
	////	QObject::connect(sender, signal, context ? context : sender, [=](A... args) {
	////		return Public<Ct, R> { lambda(std::forward<A>(args)...) };
	////		}, Ct);
	////}

	////// connect to class member
	////template <typename TObject, Qt::ConnectionType Ct, typename R, typename... A, typename Fn>
	////static void Connect(TObject* sender, Public<Ct, R>(TObject::* signal)(A...), typename QtPrivate::FunctionPointer<Fn>::Object* receiver, Fn slot) {
	////	QObject::connect(sender, signal, receiver, [=](A... args) { // receiver must be captured by value
	////		return Public<Ct, R> { (receiver->*slot)(std::forward<A>(args)...) };
	////		}, Ct);
	////}

	
public:
	//template <typename TObject, typename TSignal, typename... A, typename Fn>
	//static std::enable_if_t<isDerivedFrom<Signal, TSignal>::value, void>
	//	Connect(TObject* sender, TSignal(TObject::* signal)(A...), const QObject* context, Fn lambda) {
	//	QObject::connect(sender, signal, context ? context : sender, [=](A... args) {
	//		if constexpr (std::is_same_v<TSignal::R, void>) {
	//			lambda(std::forward<A>(args)...);
	//		}
	//		else {
	//			return TSignal{ lambda(std::forward<A>(args)...) };
	//		}
	//		}, TSignal::ct);
	//}

	//template <typename R, Qt::ConnectionType Ct, typename TObject, typename... A, typename Fn>
	////static std::enable_if_t<isDerivedFrom<PrivateSignal, TSignal>::value, void>
	//static void Connect(TObject* sender, PublicSignal<R, Ct>(TObject::* signal)(A...), const QObject* context, Fn lambda) {
	//	ConnectInternal<R, Ct>(sender, signal, context, lambda);
	//}


	template <typename R, Qt::ConnectionType Ct, typename TObject, typename... A, typename Fn>
	static void Connect(TObject* sender, PublicSignal<R, Ct>(TObject::* signal)(A...), const QObject* context, Fn lambda) {
		ConnectInternal<R, Ct>(sender, signal, context, lambda);
	}


protected:
	template <typename R, Qt::ConnectionType Ct, typename TObject, typename... A, typename Fn>
	static void Connect(TObject* sender, PrivateSignal<R, Ct>(TObject::* signal)(A...), const QObject* context, Fn lambda) {
		ConnectInternal<R, Ct>(sender, signal, context, lambda);
	}


private:
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







	//template <typename TObject, typename TSignal, typename... A, typename Fn>
	//static void ConnectInternal(TObject* sender, TSignal(TObject::* signal)(A...), const QObject* context, Fn lambda) {
	//	QObject::connect(sender, signal, context ? context : sender, [=](A... args) {
	//		if constexpr (std::is_same_v<TSignal::R, void>) {
	//			lambda(std::forward<A>(args)...);
	//		}
	//		else {
	//			return TSignal{ lambda(std::forward<A>(args)...) };
	//		}
	//		}, TSignal::ct);
	//}


	//template <typename TObject, typename TSignal, typename... A, typename Fn>
	//static std::enable_if_t<isDerivedFrom<Private, TSignal>::value, void>
	//Connect(TObject* sender, TSignal(TObject::* signal)(A...), typename QtPrivate::FunctionPointer<Fn>::Object* receiver, Fn slot) {
	//	QObject::connect(sender, signal, receiver, [=](A... args) {
	//		return TSignal{ (receiver->*slot)(std::forward<A>(args)...) };
	//		}, TSignal::ct);
	//}
};



class Second : public QObject {
	Q_OBJECT

public slots:
	void ErrorSlot(int value);

};

class Temp : public QObject, public QtConnection {
	Q_OBJECT

public:
	Temp();

signals:
	void Foo(int);
	//QtConnection::Public<Qt::DirectConnection, int> Hello(int);
	//QtConnection::Private<void, Qt::BlockingQueuedConnection> Error(int);
	//QtConnection::PrivateDirect<int> Error(int);
	//QtConnection::PrivateQueued<int> Error(int);
	
	//QtConnection::PublicBlocked<void> Error(int);
	QtConnection::PublicDirect<float> ErrorPublic(int);
	QtConnection::PrivateDirect<float> ErrorPrivate(int);

public:
	void EmitSignal();

public:
	Second second;
};





//// Force connect publicSignals as specified defaultConnectionType
//template <typename TSignals>
//class QtConnectionHelper {
//public:
//	QtConnectionHelper()
//		: classSignals{ new TSignals() }
//	{}
//	~QtConnectionHelper() = default;
//
//
//	// connect to functor
//	//template <typename R, typename... A, typename Fn>
//	//std::enable_if_t<is_specialization_of<R, QtSignal>::value, void>
//	template <Qt::ConnectionType Ct, typename R, typename... A, typename Fn>
//	void Connect(QtSignal<Ct, R>(TSignals::* signal)(A...), Fn slot) {
//		QObject::connect(classSignals.get(), signal, classSignals.get(), [slot](A... args) -> R {
//			return slot(std::forward<A>(args)...);
//			}, Ct);
//	}
//
//protected:
//	std::unique_ptr<TSignals> classSignals;
//};
//
//
//class TempSignals : public QObject {
//	Q_OBJECT
//signals:
//	QtSignal<Qt::QueuedConnection, void> Hello(int);
//};
//
//
//class Temp : public QObject, public QtConnectionHelper<TempSignals> {
//    Q_OBJECT
//
//signals:
//    void Foo(int);
//
//public:
//	void EmitSignal();
//};