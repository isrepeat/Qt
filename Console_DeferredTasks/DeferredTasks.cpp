#include "DeferredTasks.h"
#include <Windows.h>
#include <thread>
#include <future>
#include <QDebug>


DeferredTasks::DeferredTasks() 
	: privateSignals{ new PrivateSignals() }
{
	connect(privateSignals.get(), &PrivateSignals::TasksFinished, this, [this] {
		inProgress = false;
		deferredTasks.clear();

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

DeferredTasks::~DeferredTasks() {
	qDebug() << "DeferredTasks::~DeferredTasks() ...";
	this->CancelTasks();


	//std::unique_lock<std::mutex> lk{ mxTask, std::defer_lock };
	//while (!lk.try_lock()) {
	//	qDebug() << "--- try lock ...";
	//	std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
	//}

	qDebug() << "--- wait finish deferred tasks thread ...";
	std::unique_lock<std::mutex> lk{ mxTask };
	qDebug() << "--- deferred tasks thread finished";

	qDebug() << "DeferredTasks::~DeferredTasks() success";
}

void DeferredTasks::ExecuteInOtherThread() {
	if (inProgress)
		return;

	if (deferredTasks.size() == 0)
		return;

	canceled = false;

	auto future = std::async(std::launch::async, [this] {
		std::lock_guard lk(mxTask);
		inProgress = true;

		int numTask = 0;
		for (auto& deferredTask : deferredTasks) {
			// Waiting part
			qDebug() << "\ncheck blocking deferred task ...";
			if (deferredTask->blockingTask) {
				qDebug() << " --- block ...";
				taskLocker.Lock();	// may block thread for a LONG TIME (wait completed signal from previous blocking task)
				qDebug() << " --- unblock";
				deferredTask->blockSignals(false); // allow emit signals from this task
			}
			

			//if (currentTask) {
			//	qDebug() << "---- block signals for previous task ";
			//	currentTask->blockSignals(true);
			//}
			qDebug() << "---- Task " << numTask++ << "started ----";
			//qDebug() << "---- unlock signals for current task ";
			//currentTask = deferredTask;
			//currentTask->blockSignals(false);

			Sleep(10);
			Sleep(10);
			Sleep(10);

			if (deferredTask->startAfterMs > 0 && !canceled) {
				qDebug() << "waiting ...";
				std::mutex m;
				std::unique_lock<std::mutex> lk(m);
				cvShiftStarting.wait_for(lk, std::chrono::milliseconds{ deferredTask->startAfterMs });
			}

			// Executing part
			//std::unique_lock<std::mutex> lk(mxExecuting);
			if (canceled) {
				qDebug() << "canceled ...";
				return;
			}

			if (deferredTask->task) {
				qDebug() << "perform deferred task via signal ...";
				if (auto ex_ptr = emit privateSignals->PerformTask(&deferredTask->task)) {
					qDebug() << "task throw exception";
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
			connect(deferredTasks.back().get(), &QDeferredTask::BlockingTaskCompleted, this, [this] {
				emit privateSignals->TasksFinished();
				});
		}
		else {
			emit privateSignals->TasksFinished();
		}
		}, Qt::ConnectionType::DirectConnection); // connect direct to catch signal even current thread blocked

	tasksCompletedWaiter->StartWaitinig();
}


void DeferredTasks::CancelTasks() {
	qDebug() << "DeferredTasks::CancelTasks() ...";
	if (inProgress) {
		canceled = true;
		taskLocker.Unlock();
		cvShiftStarting.notify_all();

	}
	qDebug() << "DeferredTasks::CancelTasks() success";
}
