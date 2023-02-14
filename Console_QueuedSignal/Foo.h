#pragma once
#include <QObject>
#include <functional>
#include "QueuedSignal.h"
#include <map>
#include <optional>


//template <class _Ret, class... _Types>
//class Qt_signal_impl : public QObject {
//    Q_OBJECT
//
//signals:
//    _Ret Signal(_Types... _Args);
//};
//
//template <class _Tx>
//struct Get_qt_signal_impl{
//    static_assert(_Always_false<_Tx>, "QtSignal does not accept non-function types as template arguments.");
//};
//
//template <class _Ret, class... _Types>
//struct Get_qt_signal_impl<_Ret _cdecl(_Types...)> { /* determine type from argument list */
//    using type = Qt_signal_impl<_Ret, _Types...>;
//};
//
//template <typename _Fty>
//class QtSignal : public Get_qt_signal_impl<_Fty>::type {
//public:
//    using _Mybase = typename Get_qt_signal_impl<_Fty>::type;
//};
//
//
//class QtConnectionHelper {
//public:
//    template <typename _Fty, typename Fn>
//    static void Connect(QtSignal<_Fty>* signal, Fn slot) {
//	    QObject::connect(signal, &QtSignal<_Fty>::_Mybase::Signal, signal, slot, Qt::QueuedConnection);
//    }
//};
//
//
//class TempSignals : public QObject {
//    Q_OBJECT
//
//signals:
//    void Foo(int);
//
//public:
//    QtSignal<void(int)> Hello;
//    QtSignal<void(float)> Error;
//};


//template<typename Type, template<typename...> class Args>
//struct is_specialization_of : std::false_type {};
//
//template<template<typename...> class PointerType, typename... Args>
//struct is_specialization_of<PointerType<Args...>, PointerType> : std::true_type {};
//

template<typename Type, template<Qt::ConnectionType, typename...> class Args>
struct is_specialization_of : std::false_type {};

template<template<Qt::ConnectionType, typename...> class SignalType, Qt::ConnectionType Ct, typename... Args>
struct is_specialization_of<SignalType<Ct, Args...>, SignalType> : std::true_type {};





class QtConnection {
public:
	template <Qt::ConnectionType connectionType, typename Ret>
	struct Public {
		std::optional<Ret> result;
	};

	//template <typename R, typename... A, typename Fn>
	//std::enable_if_t<is_specialization_of<R, QtSignal>::value, void>

	// connect to functor
	template <typename TObject, Qt::ConnectionType Ct, typename R, typename... A, typename Fn>
	static void Connect(TObject* sender, Public<Ct, R>(TObject::* signal)(A...), const QObject* context, Fn lambda) {
		QObject::connect(sender, signal, context ? context : sender, [=](A... args) {
			return Public<Ct, R> { lambda(std::forward<A>(args)...) };
			}, Ct);
	}

	// connect to class member
	template <typename TObject, Qt::ConnectionType Ct, typename R, typename... A, typename Fn>
	static void Connect(TObject* sender, Public<Ct, R>(TObject::* signal)(A...), typename QtPrivate::FunctionPointer<Fn>::Object* receiver, Fn slot) {
		QObject::connect(sender, signal, receiver, [=](A... args) { // receiver must be captured by value
			return Public<Ct, R> { (receiver->*slot)(std::forward<A>(args)...) };
			}, Ct);
	}

protected:
	template <Qt::ConnectionType connectionType, typename Ret>
	struct Private {
		std::optional<Ret> result;
	};

	template <typename TObject, Qt::ConnectionType Ct, typename R, typename... A, typename Fn>
	static void Connect(TObject* sender, Private<Ct, R>(TObject::* signal)(A...), const QObject* context, Fn lambda) {
		QObject::connect(sender, signal, context ? context : sender, [=](A... args) {
			return Private<Ct, R> { lambda(std::forward<A>(args)...) };
			}, Ct);
	}

	template <typename TObject, Qt::ConnectionType Ct, typename R, typename... A, typename Fn>
	static void Connect(TObject* sender, Private<Ct, R>(TObject::* signal)(A...), typename QtPrivate::FunctionPointer<Fn>::Object* receiver, Fn slot) {
		QObject::connect(sender, signal, receiver, [=](A... args) {
			return Private<Ct, R> { (receiver->*slot)(std::forward<A>(args)...) };
			}, Ct);
	}
};

//template<Qt::ConnectionType Ct, typename Ret>
//using Public = QtConnection::PublicSignal<Ct, Ret>;
//
////template<Qt::ConnectionType Ct, typename Ret>
////using Private = QtConnection::PrivateSignal<Ct, Ret>;
//
//
//template <Qt::ConnectionType connectionType, typename Ret>
//struct _PrivateSignal : public QtConnection {
//	template<Qt::ConnectionType Ct, typename Ret>
//	using Private = QtConnection::PrivateSignal<Ct, Ret>;
//};

//template<Qt::ConnectionType Ct, typename Ret>
//using PrivateSignal = QtConnection::QtPrivateSignal<Ct, Ret>;


class Second : public QObject {
	Q_OBJECT

public slots:
	int MySlot(int value);
	int MySlot2(int value);

};

class Temp : public QObject, public QtConnection {
	Q_OBJECT

public:
	Temp();

signals:
	void Foo(int);
	QtConnection::Public<Qt::DirectConnection, int> Hello(int);
	QtConnection::Private<Qt::BlockingQueuedConnection, int> Error(int);

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