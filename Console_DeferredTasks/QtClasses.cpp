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

void Request::Get(std::function<void()> callback) {
	std::thread([this, callback] {
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		callback();
		callback();
		callback();
		}).detach();
}


Prototype::Prototype(QObject* parrent) : QObject(parrent) {
	
	asyncTasks.ConnectTaskExecuter(this, &Prototype::TaskExecuter);
	connect(&asyncTasks, &AsyncTasks::TasksFailed, this, &Prototype::TasksFailedHandler);
	connect(&asyncTasks, &AsyncTasks::TasksCompleted, this, &Prototype::TasksCompletedHandler);


	asyncTasks.AddBlockingTask("Task 1", [this] {
		request.Get();
		}
	,&request, &Request::Done);

	asyncTasks.AddBlockingTask("Task 2", [this] {
		request.Get();
		}
	,&request, &Request::Done);


	//asyncTasks.AddBlockingTask([this] (H::Signal completedSignal) {
	//	request.Get([this, completedSignal] {
	//		completedSignal();
	//		});
	//	});

	//asyncTasks.AddBlockingTask([this](H::Signal completedSignal) {
	//	request.Get([this, completedSignal] {
	//		completedSignal();
	//		});
	//	});


	//asyncTasks.AddBlockingTask("Task 3", [this] {
	//	request.Get();
	//	}
	//,&request, &Request::Done);

	//asyncTasks.AddBlockingTask("Task 4", [this] {
	//	request.Get();
	//	}
	//, &request, &Request::Done);


	asyncTasks.Execute();
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