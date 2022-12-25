#include "AsyncTasks.h"
#include <Windows.h>
#include <thread>
#include <future>
#include <QDebug>



AsyncTask::AsyncTask()
	: privateSignals{ new PrivateSignals() }
{}

AsyncTask::AsyncTask(std::string name, std::function<void()> task, bool blockingTask, int startAfterMs) : AsyncTask() {
	SetTask(name, task, blockingTask, startAfterMs);
}

void AsyncTask::SetTask(std::string name, std::function<void()> task, bool blockingTask, int startAfterMs) {
	std::lock_guard lk{ mx };
	this->name = name;
	this->task = task;
	this->blockingTask = blockingTask;
	this->startAfterMs = startAfterMs;
}


void AsyncTask::Activate() {
	std::lock_guard lk{ mx };
	activated = true;
}

std::function<void()> AsyncTask::GetTask() {
	return task;
}

bool AsyncTask::IsBlockingTask() {
	return blockingTask;
}

int AsyncTask::GetStartOffset() {
	return startAfterMs;
}

void AsyncTask::BlockingTaskComplete() {
	std::lock_guard lk{ mx };
	if (!blockingTask || !activated || wasEmitted)
		return;

	wasEmitted = true;
	emit privateSignals->BlockingTaskCompleted();
}


















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


	//std::unique_lock<std::mutex> lk{ mxTask, std::defer_lock };
	//while (!lk.try_lock()) {
	//	qDebug() << "--- try lock ...";
	//	std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
	//}

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
			// Waiting part
			qDebug() << "\ncheck blocking async task ...";
			if (asyncTask->IsBlockingTask()) {
				qDebug() << " --- block ...";
				taskLocker.Lock();	// may block thread for a LONG TIME (wait completed signal from previous blocking task)
				qDebug() << " --- unblock";
				asyncTask->Activate();
				//asyncTask->blockSignals(false); // allow emit signals from this task
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

			if (asyncTask->GetStartOffset() > 0 && !canceled) {
				qDebug() << "waiting ...";
				std::mutex m;
				std::unique_lock<std::mutex> lk(m);
				cvShiftStarting.wait_for(lk, std::chrono::milliseconds{ asyncTask->GetStartOffset() });
			}

			// Executing part
			//std::unique_lock<std::mutex> lk(mxExecuting);
			if (canceled) {
				qDebug() << "canceled ...";
				return;
			}

			if (asyncTask->name == "Task 2" && !taskLocker.IsLocked()) {
				qDebug() << "FAIL ...";
				qDebug() << "FAIL ...";
				qDebug() << "FAIL ...";
				qDebug() << "FAIL ...";
				qDebug() << "FAIL ...";
				qDebug() << "FAIL ...";
			}

			if (asyncTask->GetTask()) {
				qDebug() << "perform async task via signal ...";
				if (auto ex_ptr = emit privateSignals->PerformTaskSynchronously(&asyncTask->GetTask())) {
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
			//connect(asyncTasks.back().get(), &AsyncTask::BlockingTaskCompleted, this, [this] {
			//	emit privateSignals->TasksFinished();
			//	});
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
