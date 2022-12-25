#include "QtClasses.h"
#include <thread>

//template <typename Func1, typename Func2>
//static inline QMetaObject::Connection connect(const typename QtPrivate::FunctionPointer<Func1>::Object* sender, Func1 signal,
//	const typename QtPrivate::FunctionPointer<Func2>::Object* receiver, Func2 slot,
//	Qt::ConnectionType type = Qt::AutoConnection)

void Request::Get() {
	std::thread([this] {
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		emit Done();
		emit Done();
		emit Done();
		}).detach();
}


Prototype::Prototype(QObject* parrent) : QObject(parrent) {
	
	deferredTasks.ConnectTaskExecuter(this, &Prototype::TaskExecuter);
	connect(&deferredTasks, &DeferredTasks::TasksFailed, this, &Prototype::TasksFailedHandler);
	connect(&deferredTasks, &DeferredTasks::TasksCompleted, this, &Prototype::TasksCompletedHandler);


	deferredTasks.AddBlockingTask([this] {
		request.Get();
		}
	,&request, &Request::Done);

	deferredTasks.AddBlockingTask([this] {
		request.Get();
		}
	,&request, &Request::Done);

	deferredTasks.AddBlockingTask([this] {
		request.Get();
		}
	,&request, &Request::Done);


	deferredTasks.ExecuteInOtherThread();
}


std::exception_ptr Prototype::TaskExecuter(std::function<void()>* task) {
	try {
		(*task)();
	}
	catch (...) {
		return std::current_exception();
	}
	return nullptr;
}

void Prototype::TasksCompletedHandler() {
	qDebug() << "SUCCESS";
	emit request.Done();
	emit request.Done();
	emit request.Done();
}

void Prototype::TasksFailedHandler() {
	qDebug() << "FAILED";
}