#pragma once
#include <memory>
#include <QObject>
#include "AsyncTasks.h"


class Request : public QObject {
	Q_OBJECT

public:
	Request() = default;
	~Request() {
	}

	void Get();
	void Get(std::function<void()> callback);

signals:
	void Done();
};


class Prototype : public QObject {
	Q_OBJECT

public:
	Prototype(QObject* parrent = nullptr);
	~Prototype() = default;


private slots:
	std::exception_ptr TaskExecuter(std::function<void()>* task);
	void TasksCompletedHandler();
	void TasksFailedHandler();

private:
	Request request;
	AsyncTasks asyncTasks;
};