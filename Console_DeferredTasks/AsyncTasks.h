#pragma once
#include <mutex>
#include <condition_variable>
#include <QObject>
#include <QDebug>
#include "FutureWaiter.h"
#include "UniqueThreadAccessor.h"
#include "Signal.h"


class AsyncTask : public QObject {
	Q_OBJECT

public:
	AsyncTask();
	~AsyncTask() = default;
	AsyncTask(std::string name, std::function<void()> task, bool blockingTask = true, int startAfterMs = 0);

	void SetTask(std::string name, std::function<void()> task, bool blockingTask = true, int startAfterMs = 0);

	// connect to functor
	template <typename Fn>
	void ConnectBlockingTaskCompletedHandler(const QObject* context, Fn slot) {
		connect(privateSignals.get(), &PrivateSignals::BlockingTaskCompleted, context, slot, Qt::QueuedConnection);
	}

	// connect to class member
	template <typename Fn>
	void ConnectBlockingTaskCompletedHandler(const typename QtPrivate::FunctionPointer<Fn>::Object* receiver, Fn slot) {
		connect(privateSignals.get(), &PrivateSignals::BlockingTaskCompleted, receiver, slot, Qt::QueuedConnection);
	}


	std::function<void()> GetTask();
	bool IsBlockingTask();
	int GetStartOffset();

public slots:
	void Activate();
	void BlockingTaskComplete();

public:
	std::string name;
private:
	std::mutex mx;
	std::function<void()> task = nullptr;
	int startAfterMs = 0;
	bool activated = false;
	bool wasEmitted = false;
	bool blockingTask = false;

	class PrivateSignals;
	std::unique_ptr<PrivateSignals> privateSignals;
};


// must implement in .h file beacuse .cpp file not moc'ing
class AsyncTask::PrivateSignals : public QObject {
	Q_OBJECT

public:
	PrivateSignals() = default;

signals:
	void BlockingTaskCompleted();
};




class AsyncTasks : public QObject {
	Q_OBJECT

public:
	AsyncTasks();
	~AsyncTasks();

	
	// connect to functor
	template <typename Fn>
	void ConnectTaskExecuter(const QObject* context, Fn slot) {
		connect(privateSignals.get(), &PrivateSignals::PerformTaskSynchronously, context, slot, Qt::BlockingQueuedConnection);
	}

	// connect to class member
	template <typename Fn>
	void ConnectTaskExecuter(const typename QtPrivate::FunctionPointer<Fn>::Object* receiver, Fn slot) {
		connect(privateSignals.get(), &PrivateSignals::PerformTaskSynchronously, receiver, slot, Qt::BlockingQueuedConnection);
	}


	void AddTask(std::string name, std::function<void()> task, int startAfterMs = 0) {
		std::lock_guard lk(mxTask);
		asyncTasks.push_back(std::make_shared<AsyncTask>(name, task, false, startAfterMs));
	}

	// For qt class signals
	template <typename Fn>
	void AddBlockingTask(std::string name, std::function<void()> task, const typename QtPrivate::FunctionPointer<Fn>::Object* sender, Fn completedSignal, int startAfterMs = 0) {
		std::lock_guard lk(mxTask);
		auto asyncTask = std::make_shared<AsyncTask>(name, task, true, startAfterMs);
		asyncTask->blockSignals(true);

		////connect(sender, completedSignal, asyncTask.get(), &AsyncTask::BlockingTaskCompleted);
		//connect(sender, completedSignal, [this, asyncTask] {
		//	int xxx = 9;
		//	emit asyncTask->BlockingTaskCompleted();
		//	int aaa = 9;
		//	});
		//connect(asyncTask.get(), &AsyncTask::BlockingTaskCompleted, this, [this, asyncTask] {
		//	asyncTask->blockSignals(true); // block again emits signals before unlock this task
		//	taskLocker.Unlock();
		//	});

		 
		std::weak_ptr<AsyncTask> asyncTaskWeak = asyncTask;
		connect(sender, completedSignal, asyncTask.get(), [asyncTaskWeak] {
			if (asyncTaskWeak.expired())
				return;

			int xxx = 9;
			asyncTaskWeak.lock()->BlockingTaskComplete();
			int aaa = 9;
			});

		asyncTask->ConnectBlockingTaskCompletedHandler(this, [this, asyncTaskWeak] {
			taskLocker.Unlock();
			});

		asyncTasks.push_back(std::move(asyncTask));
	}

	//// For std::function callback
	//void AsyncTasks::AddBlockingTask(std::string name, std::function<void(H::Signal)> task, int startAfterMs = 0) {
	//	std::lock_guard lk(mxTask);
	//	asyncTasks.push_back(std::make_shared<AsyncTask>());

	//	auto asyncTask = asyncTasks.back();
	//	asyncTask->SetTask(name, [this, task, asyncTask] {
	//		//H::Signal completedSignalTmp(H::Signal::Once);
	//		H::Signal completedSignalTmp(H::Signal::Multi);
	//		completedSignalTmp.AddFinish([this, asyncTask] {
	//			emit asyncTask->BlockingTaskCompleted(); // emit after user manually called completedSignalTmp outside
	//			});

	//		task(completedSignalTmp);
	//		}
	//	, true, startAfterMs);

	//	asyncTask->blockSignals(true);
	//	connect(asyncTask.get(), &AsyncTask::BlockingTaskCompleted, this, [this, asyncTask] {
	//		asyncTask->blockSignals(true); // block again emits signals before unlock this task
	//		taskLocker.Unlock();
	//		});
	//}

	void Execute();
	void CancelTasks();

signals:
	void TasksCompleted();
	void TasksFailed();

private:
	std::mutex mxTask;
	std::mutex mxExecuting;
	H::UniqueThreadAccessor taskLocker;
	std::condition_variable cvShiftStarting;
	std::atomic<bool> canceled = false;
	std::atomic<bool> inProgress = false;
	std::unique_ptr<FutureWaiter<void>> tasksCompletedWaiter;
	
	std::shared_ptr<AsyncTask> currentTask;
	std::vector<std::shared_ptr<AsyncTask>> asyncTasks;


	class PrivateSignals;
	std::unique_ptr<PrivateSignals> privateSignals;
};


// must implement in .h file beacuse .cpp file not moc'ing
class AsyncTasks::PrivateSignals : public QObject {
	Q_OBJECT

public:
	PrivateSignals() = default;

signals:
	std::exception_ptr PerformTaskSynchronously(std::function<void()>* task); // connect in UI class as Qt::BlockingQueuedConnection to correct working
	void TasksFinished();
};