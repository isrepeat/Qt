#pragma once
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
	
	//template <typename R, typename... A, typename Fn>
	//void Connect(R(TPublic::* signal)(A...), Fn slot) {
	//	static_assert(defaultConnectionType != Qt::ConnectionType::UniqueConnection, "Uniqie Connection 1 !!!");
	//	QObject::connect(publicSignals.get(), signal, publicSignals.get(), slot, defaultConnectionType);
	//}

	template <typename R, typename... A, typename Fn>
	void Connect(R(TPublic::* signal)(A...), Fn slot, Qt::ConnectionType connectionType = defaultConnectionType) {
		if (connectionType == Qt::ConnectionType::UniqueConnection) {
			int xx = 9;
		}
		//static_assert(connectionType != Qt::ConnectionType::UniqueConnection, "Uniqie Connection 1 !!!");
		//QObject::connect(publicSignals.get(), signal, publicSignals.get(), slot, defaultConnectionType);
	}


	// connect to functor
	template <typename R, typename... A, typename Fn>
	void Connect(R(TPublic::* signal)(A...), const QObject* context, Fn slot) {
		static_assert(defaultConnectionType != Qt::ConnectionType::UniqueConnection, "Uniqie Connection 2 !!!");
		QObject::connect(publicSignals.get(), signal, context, slot, defaultConnectionType);
	}

	// connect to class member
	template <typename R, typename... A, typename Fn>
	void Connect(R(TPublic::* signal)(A...), const typename QtPrivate::FunctionPointer<Fn>::Object* receiver, Fn slot) {
		static_assert(defaultConnectionType != Qt::ConnectionType::UniqueConnection, "Uniqie Connection 3 !!!");
		QObject::connect(publicSignals.get(), signal, receiver, slot, defaultConnectionType);
	}

protected:
	std::unique_ptr<TPublic> publicSignals;
	std::unique_ptr<TPrivate> privateSignals; // connect manually
};