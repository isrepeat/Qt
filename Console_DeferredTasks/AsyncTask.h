#pragma once
#include <QObject>
#include <functional>
#include <memory>
#include <mutex>


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
