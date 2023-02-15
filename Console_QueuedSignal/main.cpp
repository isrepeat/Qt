#include <QtCore/QCoreApplication>
#include <Windows.h>
#include <string>
#include <any>
#include <variant>
#include <QTimer>
#include <QDebug>
#include "Foo.h"
#include "Bar.h"


////template<typename T>
////struct is_derived_from_base_interface {
////private:
////    template<size_t S1, size_t S2>
////    static decltype(static_cast<const BaseInterface<S1, S2>&>(std::declval<T>()), std::true_type{})
////        test(const BaseInterface<S1, S2>&);
////
////    static std::false_type test(...);
////public:
////    static constexpr bool value =
////        decltype(is_derived_from_base_interface::test(std::declval<T>()))::value;
////};
//
//
////template<typename Type, template<int, typename> class Args>
////struct is_specialization_of : std::false_type {};
////
////template<template<int, typename> class SignalType, int Ct, typename T>
////struct is_specialization_of<SignalType<Ct, T>, SignalType> : std::true_type {};
//
//
//
//
////template<template<int, class> class T, class U>
////struct isDerivedFrom
////{
////private:
////    template<int N, class V>
////    static decltype(static_cast<const T<N, V>&>(std::declval<U>()), std::true_type{})
////        test(const T<N, V>&);
////
////    static std::false_type test(...);
////public:
////    static constexpr bool value = decltype(isDerivedFrom::test(std::declval<U>()))::value;
////};
//
//
//template <int V, typename T>
//struct Base {
//    using RetType = T;
//    static const int val = V;
//};
//
//template <typename T>
//struct Derrived : public Base<111, T> {
//};
//
//template <typename T>
//struct Derrived2 {
//};
//
//
//
//
//
//template<typename T>
//typename std::enable_if<isDerivedFrom<Base, T>::value>::type
//Foo(T(*ptr)()) {
//    int xxx = 0;
//}
//
////template <template<int, typename> typename TBase, int Val, typename T>
//////template <typename TBase, typename T>
////void Foo(Base<Val, T> (*ptr)()) {
////    int xxx = 0;
////}
//
//Base<66, float> baseFunc() {
//}
//Derrived<float> derrivedFunc() {
//}
//
//Derrived2<float> derrivedFunc2() {
//}
//
//
////template <typename ChildTemplate, template<int, class...> class Template>
////struct is_convertible_specialization : std::false_type {};
////
////template <template<class...> class ChildTemplate, template<int, class...> class Template, class... Args>
////struct is_convertible_specialization<ChildTemplate<Args...>, Template>
////    : std::is_convertible<ChildTemplate<Args...>*, Template<Args...>*>
////{};
////
////
////template<int, typename ... Args>
////class Base {};
////
////template<typename ... Args>
////class Child : public Base<Args...> {};
////
////template<typename ... Args>
////class Unrelated {};
//
//
////template<int, typename T>
////class Base {};
////
////template<typename T>
////class Child : public Base<12, T> {};
////
////template<typename T>
////class Unrelated {};
//
//
////template <typename T>
////inline constexpr bool isDerived = isDerivedFrom<Base, T>::value;



template <typename R, int C>
struct Base {
    static const int ct = C;
};

template <typename R>
struct Derived : Base<R, 111> {
};

Base<float, 333> baseFunc() {
    return {};
};

Derived<float> derivedFunc() {
    return {};
};


//template <typename R, int C>
//void Foo(Base<R,C> (*ptrFunc)()) {
//};

//template <template<typename> class TObj, typename R>
//void Foo(Base<R, TObj<R>::ct>(*ptrFunc)()) {
//};

//struct Some {
//    template<typename V, Qt::ConnectionType Ct>
//    static decltype(static_cast<const T<V, Ct>&>(std::declval<U>()), std::true_type{})
//        test(const T<V, Ct>&);
//};


//template<typename V, int Ct>
//decltype(static_cast<const Base<V, Ct>&>(std::declval<U>()))
void test(Base<float, 111> (*ptr)()) {
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    
    
    //Foo(derivedFunc);

    //test(derivedFunc);


    //static_cast<const Base<float, Ct>&>(std::declval<Derived<float>>())
    //Foo(derivedFunc);

    //std::unique_ptr<void> aaa;
    //std::unique_ptr<void> aaa;

    //static_assert(is_convertible_specialization<Child<float>, Base>::value); // True

    //Foo(derrivedFunc);

    //static_assert(isDerivedFrom<Base, Child<float>>::value);

    Temp temp;
    
    QtConnection::Connect(&temp, &Temp::ErrorPublic, &temp, [](int value) {
        return value / 100.0f;
        });




    //std::variant<void> var;

    //Second second;

    //QObject::connect(&tempSignals.bar, &std::function<void(int)>::operator(), [] (int value) {
    //    int xxx = 9;
    //    });

    //QObject::connect(&tempSignals.temp, &Temp::Bar, [](int value) {
    //    int xxx = 9;
    //    });

    //QObject::connect(&tempSignals, &TempSignals::Foo<void>, [](int value) {
    //    int xxx = 9;
    //    });

    //QtSignal<void(int,float)>::_Mybase::Signal

    //temp.Connect(&TempSignals::Hello, [](int value) {
    //    int xxx = 9;
    //    });


    //QtConnection::Connect(&temp, &Temp::Hello, &second, &Second::MySlot);

    //QtConnection::Connect(&temp, &Temp::Hello, nullptr, [](int value) -> float {
    //    int xxx = 9;
    //    return 3.11f;
    //    });

    QTimer::singleShot(600, [&temp] {
        temp.EmitSignal();
        });

    return a.exec();
}
