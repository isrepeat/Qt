#pragma once
#include <memory>
#include <QObject>
#include "DeferredTasks.h"


class Request : public QObject {
	Q_OBJECT

public:
	Request() = default;
	~Request() {
	}

	void Get();

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
	DeferredTasks deferredTasks;
};