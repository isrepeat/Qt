#include "AsyncTask.h"

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
