#pragma once

#include <QString>
#include <taskschd.h>


class TaskSchedulerManager {
public:
	TaskSchedulerManager() = default;
	~TaskSchedulerManager() { cleanup(); }

	bool taskExists();
	bool createTask(const QString& programPath);
	bool deleteTask();

private:
	ITaskService* taskService = nullptr;

	bool initializeTaskService();
	void cleanup();
};
