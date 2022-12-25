#include "AsyncTasks.h"
#include <Windows.h>
#include <thread>
#include <future>
#include <QDebug>



AsyncTasks::AsyncTasks()
	: privateSignals{ new PrivateSignals() }
{
	connect(privateSignals.get(), &PrivateSignals::TasksFinished, this, [this] {
		inProgress = false;
		asyncTasks.clear();

		if (canceled) {
			emit TasksFailed();
			return;
		}

		try {
			tasksCompletedWaiter->GetResult(); // check if was exception
			emit TasksCompleted();
		}
		catch (std::exception ex) {
			emit TasksFailed();
			// add logs ...
		}
		catch (...) {
			emit TasksFailed();
		}
		});
}

AsyncTasks::~AsyncTasks() {
	qDebug() << "AsyncTasks::~AsyncTasks() ...";
	this->CancelTasks();

	qDebug() << "--- wait finish async tasks thread ...";
	std::unique_lock<std::mutex> lk{ mxTask };
	qDebug() << "--- async tasks thread finished";

	qDebug() << "AsyncTasks::~AsyncTasks() success";
}

void AsyncTasks::Execute() {
	if (inProgress)
		return;

	if (asyncTasks.size() == 0)
		return;

	canceled = false;

	auto future = std::async(std::launch::async, [this] {
		std::lock_guard lk(mxTask);
		inProgress = true;

		int numTask = 0;
		for (auto& asyncTask : asyncTasks) {
			qDebug() << "\ncheck blocking async task ...";
			if (asyncTask->IsBlockingTask()) {
				qDebug() << " --- block ...";
				taskLocker.Lock();	// may block thread for a LONG TIME (wait completed signal from previous blocking task)
				qDebug() << " --- unblock";
				asyncTask->Activate();
			}

			qDebug() << "\ncheck start offset ...";
			if (asyncTask->GetStartOffset() > 0 && !canceled) {
				qDebug() << "waiting before start ...";
				std::mutex m;
				std::unique_lock<std::mutex> lk(m);
				cvShiftStarting.wait_for(lk, std::chrono::milliseconds{ asyncTask->GetStartOffset() });
			}

			if (canceled) {
				qDebug() << "canceled";
				return;
			}

			if (asyncTask->GetTask()) {
				qDebug() << "perform async task via signal ...";
				if (auto ex_ptr = emit privateSignals->PerformTaskSynchronously(&asyncTask->GetTask())) {
					qDebug() << "task throwed exception";
					std::rethrow_exception(ex_ptr);
				}
				qDebug() << "task completed";
			}
		}
		});


	tasksCompletedWaiter = std::make_unique<FutureWaiter<void>>(std::move(future));
	connect(tasksCompletedWaiter.get(), &FutureWaiter<void>::Ready, this, [this] {
		// Thread finished successfully or throwed exception ...
		if (taskLocker.IsLocked()) { // if last was blocking task and it is still locked then wait completed signal
			asyncTasks.back()->ConnectBlockingTaskCompletedHandler(this, [this] {
				emit privateSignals->TasksFinished();
				});
		}
		else {
			emit privateSignals->TasksFinished();
		}
		}, Qt::ConnectionType::DirectConnection); // connect direct to catch signal even current thread blocked

	tasksCompletedWaiter->StartWaitinig();
}


void AsyncTasks::CancelTasks() {
	qDebug() << "AsyncTasks::CancelTasks() ...";
	if (inProgress) {
		canceled = true;
		taskLocker.Unlock();
		cvShiftStarting.notify_all();

	}
	qDebug() << "AsyncTasks::CancelTasks() success";
}
