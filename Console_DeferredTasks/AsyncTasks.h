#pragma once
#include "AsyncTask.h"
#include "UniqueThreadAccessor.h"
#include <condition_variable>
#include "FutureWaiter.h"
#include "Signal.h"
#include <QDebug>


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
		asyncTasks.push_back(std::make_shared<AsyncTask>(name, task, true, startAfterMs));
		auto asyncTask = asyncTasks.back();
 
		connect(sender, completedSignal, asyncTask.get(), &AsyncTask::BlockingTaskComplete);
		asyncTask->ConnectBlockingTaskCompletedHandler(this, [this] {
			taskLocker.Unlock();
			});
	}

	// For std::function callbacks
	void AsyncTasks::AddBlockingTask(std::string name, std::function<void(H::Signal)> task, int startAfterMs = 0) {
		std::lock_guard lk(mxTask);
		asyncTasks.push_back(std::make_shared<AsyncTask>());
		auto asyncTask = asyncTasks.back();
		auto asyncTaskWeak = std::weak_ptr<AsyncTask>(asyncTask);

		asyncTask->SetTask(name, [this, task, asyncTaskWeak] {
			H::Signal completedSignalTmp(H::Signal::Once);
			completedSignalTmp.AddFinish([asyncTaskWeak] {
				if (asyncTaskWeak.expired())
					return;

				int xxx = 9;
				asyncTaskWeak.lock()->BlockingTaskComplete(); // emit after user manually called completedSignalTmp outside
				int aaa = 9;
				});

			task(completedSignalTmp);
			}
		, true, startAfterMs);

		asyncTask->ConnectBlockingTaskCompletedHandler(this, [this] {
			taskLocker.Unlock();
			});
	}

	void Execute();
	void CancelTasks();

signals:
	void TasksCompleted();
	void TasksFailed();

private:
	std::mutex mxTask;
	H::UniqueThreadAccessor taskLocker;
	std::condition_variable cvShiftStarting;
	std::atomic<bool> canceled = false;
	std::atomic<bool> inProgress = false;
	std::unique_ptr<FutureWaiter<void>> tasksCompletedWaiter;
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
	std::exception_ptr PerformTaskSynchronously(std::function<void()>* task);
	void TasksFinished();
};