#pragma once
#include <mutex>
#include <condition_variable>
#include <QObject>
#include <QDebug>
#include "FutureWaiter.h"
#include "UniqueThreadAccessor.h"
#include "Signal.h"


class QDeferredTask : public QObject {
	Q_OBJECT

public:
	QDeferredTask() = default;
	~QDeferredTask() = default;
	QDeferredTask(std::function<void()> task, bool blockingTask = true, int startAfterMs = 0)
		: task{ task }
		, blockingTask{ blockingTask }
		, startAfterMs{ startAfterMs }
	{}

signals:
	void BlockingTaskCompleted();

public:
	std::function<void()> task = nullptr;
	bool blockingTask = false;
	int startAfterMs = 0;
};



class DeferredTasks : public QObject {
	Q_OBJECT

public:
	DeferredTasks();
	~DeferredTasks();

	template <typename Fn>
	void ConnectTaskExecuter(const typename QtPrivate::FunctionPointer<Fn>::Object* receiver, Fn slot) {
		connect(privateSignals.get(), &PrivateSignals::PerformTask, receiver, slot, Qt::BlockingQueuedConnection);
	}

	void AddTask(std::function<void()> task, int startAfterMs = 0) {
		std::lock_guard lk(mxTask);
		deferredTasks.push_back(std::make_shared<QDeferredTask>(task, false, startAfterMs));
	}

	// For qt class signals
	template <typename Fn>
	void AddBlockingTask(std::function<void()> task, const typename QtPrivate::FunctionPointer<Fn>::Object* sender, Fn completedSignal, int startAfterMs = 0) {
		std::lock_guard lk(mxTask);
		auto deferredTask = std::make_shared<QDeferredTask>(task, true, startAfterMs);
		deferredTask->blockSignals(true);

		connect(sender, completedSignal, deferredTask.get(), &QDeferredTask::BlockingTaskCompleted);
		connect(deferredTask.get(), &QDeferredTask::BlockingTaskCompleted, this, [this, deferredTask] {
			deferredTask->blockSignals(true); // block again emits signals before unlock this task
			taskLocker.Unlock();
			});

		deferredTasks.push_back(std::move(deferredTask));
	}

	void ExecuteInOtherThread();
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
	
	std::shared_ptr<QDeferredTask> currentTask;
	std::vector<std::shared_ptr<QDeferredTask>> deferredTasks;


	class PrivateSignals;
	std::unique_ptr<PrivateSignals> privateSignals;
};


// must implement in .h file beacuse .cpp file not moc'ing
class DeferredTasks::PrivateSignals : public QObject {
	Q_OBJECT

public:
	PrivateSignals() = default;

signals:
	std::exception_ptr PerformTask(std::function<void()>* task); // connect in UI class as Qt::BlockingQueuedConnection to correct working
	//void BlockingTaskCompleted();
	void TasksFinished();
};