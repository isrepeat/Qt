#pragma once
#include <QObject>
#include <functional>
#include "QueuedSignal.h"
#include <map>
#include <optional>


//// TODO: mb incapsulate to QtConnection
//template<template<typename, Qt::ConnectionType> typename T, typename U>
//struct isDerivedFrom {
//private:
//	template<typename V, Qt::ConnectionType Ct>
//	static decltype(static_cast<const T<V, Ct>&>(std::declval<U>()), std::true_type{})
//		test(const T<V, Ct>&);
//
//	static std::false_type test(...);
//public:
//	static constexpr bool value = decltype(isDerivedFrom::test(std::declval<U>()))::value;
//};


class QtConnection {
private:
	template <typename Ret, Qt::ConnectionType Ct>
	struct BaseSignal {
		Ret result;
	};
	template <Qt::ConnectionType Ct>
	struct BaseSignal<void, Ct> {};

	template <typename Ret, Qt::ConnectionType Ct> 
	struct PublicSignal : public BaseSignal<Ret, Ct> {};

	template <typename Ret, Qt::ConnectionType Ct>
	struct PrivateSignal : public BaseSignal<Ret, Ct> {};

public:
	template<typename R> using PublicDirect = QtConnection::PublicSignal<R, Qt::DirectConnection>;
	template<typename R> using PublicQueued = QtConnection::PublicSignal<R, Qt::QueuedConnection>;
	template<typename R> using PublicBlocked = QtConnection::PublicSignal<R, Qt::BlockingQueuedConnection>;

	template<typename R> using PrivateDirect = QtConnection::PrivateSignal<R, Qt::DirectConnection>;
	template<typename R> using PrivateQueued = QtConnection::PrivateSignal<R, Qt::QueuedConnection>;
	template<typename R> using PrivateBlocked = QtConnection::PrivateSignal<R, Qt::BlockingQueuedConnection>;
	

	template <typename R, Qt::ConnectionType Ct, typename TObject, typename... A, typename Fn>
	static void Connect(TObject* sender, PublicSignal<R, Ct>(TObject::* signal)(A...), const QObject* context, Fn lambda) {
		ConnectInternal<R, Ct>(sender, signal, context, lambda);
	}
	
	template <typename R, Qt::ConnectionType Ct, typename TObject, typename... A, typename Fn>
	static void Connect(TObject* sender, PublicSignal<R, Ct>(TObject::* signal)(A...), typename QtPrivate::FunctionPointer<Fn>::Object* receiver, Fn slot) {
		ConnectInternal<R, Ct>(sender, signal, receiver, slot);
	}

protected:
	template <typename R, Qt::ConnectionType Ct, typename TObject, typename... A, typename Fn>
	static void Connect(TObject* sender, PrivateSignal<R, Ct>(TObject::* signal)(A...), const QObject* context, Fn lambda) {
		ConnectInternal<R, Ct>(sender, signal, context, lambda);
	}

	template <typename R, Qt::ConnectionType Ct, typename TObject, typename... A, typename Fn>
	static void Connect(TObject* sender, PrivateSignal<R, Ct>(TObject::* signal)(A...), typename QtPrivate::FunctionPointer<Fn>::Object* receiver, Fn slot) {
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



class Second : public QObject {
	Q_OBJECT

public slots:
	float ErrorSlot(int value);

};

class Temp : public QObject, public QtConnection {
	Q_OBJECT

public:
	Temp();

signals:
	void Foo(int);
	
	QtConnection::PublicQueued<void> ErrorPublic(int);
	QtConnection::PrivateBlocked<void> ErrorPrivate(int);

public:
	void EmitSignal();

public:
	Second second;
};