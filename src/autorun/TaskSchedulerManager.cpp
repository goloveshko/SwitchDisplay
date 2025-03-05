#include <windows.h>
#include <taskschd.h>
#include <comutil.h> // Для работы с BSTR
#include <QVariant>
#include <QDebug>

#include "TaskSchedulerManager.h"

const QString taskName = "Switch Display";

bool TaskSchedulerManager::initializeTaskService() {
	if (taskService) return true; // Уже инициализировано

	// Инициализация COM
	if (FAILED(CoInitialize(NULL))) {
		qWarning() << "Failed to initialize COM.";
		return false;
	}

	// Создание экземпляра TaskScheduler
	HRESULT hr = CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (LPVOID*)&taskService);
	if (FAILED(hr)) {
		qWarning() << "Failed to create TaskScheduler instance.";
		cleanup();
		return false;
	}

	// Подключаемся к планировщику задач
	hr = taskService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
	if (FAILED(hr)) {
		qWarning() << "Failed to connect to TaskScheduler.";
		cleanup();
		return false;
	}

	return true;
}

void TaskSchedulerManager::cleanup() {
	if (taskService) {
		taskService->Release();
		taskService = nullptr;
	}
	CoUninitialize();
}

bool TaskSchedulerManager::taskExists() {
	if (!initializeTaskService()) return false;

	ITaskFolder* rootFolder = nullptr;
	HRESULT hr = taskService->GetFolder(_bstr_t("\\"), &rootFolder);
	if (FAILED(hr)) {
		qWarning() << "Failed to get root folder.";
		cleanup();
		return false;
	}

	IRegisteredTask* registeredTask = nullptr;
	hr = rootFolder->GetTask(_bstr_t(taskName.toStdWString().c_str()), &registeredTask);
	bool exists = SUCCEEDED(hr);

	if (registeredTask) registeredTask->Release();
	if (rootFolder) rootFolder->Release();

	return exists;
}

bool TaskSchedulerManager::createTask(const QString& programPath) {
	if (!initializeTaskService()) return false;

	ITaskFolder* rootFolder = nullptr;
	HRESULT hr = taskService->GetFolder(_bstr_t("\\"), &rootFolder);
	if (FAILED(hr)) {
		qWarning() << "Failed to get root folder.";
		cleanup();
		return false;
	}

	ITaskDefinition* taskDefinition = nullptr;
	hr = taskService->NewTask(0, &taskDefinition);
	if (FAILED(hr)) {
		qWarning() << "Failed to create new task definition.";
		if (rootFolder) rootFolder->Release();
		cleanup();
		return false;
	}

	// Настройка триггера (запуск при входе пользователя)
	ITriggerCollection* triggers = nullptr;
	hr = taskDefinition->get_Triggers(&triggers);
	if (FAILED(hr)) {
		qWarning() << "Failed to get trigger collection.";
		if (taskDefinition) taskDefinition->Release();
		if (rootFolder) rootFolder->Release();
		cleanup();
		return false;
	}

	ITrigger* trigger = nullptr;
	hr = triggers->Create(TASK_TRIGGER_LOGON, &trigger);
	if (FAILED(hr)) {
		qWarning() << "Failed to create logon trigger.";
		if (triggers) triggers->Release();
		if (taskDefinition) taskDefinition->Release();
		if (rootFolder) rootFolder->Release();
		cleanup();
		return false;
	}

	// Настройка действия (запуск программы)
	IActionCollection* actions = nullptr;
	hr = taskDefinition->get_Actions(&actions);
	if (FAILED(hr)) {
		qWarning() << "Failed to get action collection.";
		if (trigger) trigger->Release();
		if (triggers) triggers->Release();
		if (taskDefinition) taskDefinition->Release();
		if (rootFolder) rootFolder->Release();
		cleanup();
		return false;
	}

	IAction* action = nullptr;
	hr = actions->Create(TASK_ACTION_EXEC, &action);
	if (FAILED(hr)) {
		qWarning() << "Failed to create exec action.";
		if (actions) actions->Release();
		if (trigger) trigger->Release();
		if (triggers) triggers->Release();
		if (taskDefinition) taskDefinition->Release();
		if (rootFolder) rootFolder->Release();
		cleanup();
		return false;
	}

	IExecAction* execAction = nullptr;
	hr = action->QueryInterface(IID_IExecAction, (LPVOID*)&execAction);
	if (FAILED(hr)) {
		qWarning() << "Failed to query exec action interface.";
		if (action) action->Release();
		if (actions) actions->Release();
		if (trigger) trigger->Release();
		if (triggers) triggers->Release();
		if (taskDefinition) taskDefinition->Release();
		if (rootFolder) rootFolder->Release();
		cleanup();
		return false;
	}

	hr = execAction->put_Path(_bstr_t(programPath.toStdWString().c_str()));
	if (FAILED(hr)) {
		qWarning() << "Failed to set program path.";
		if (execAction) execAction->Release();
		if (action) action->Release();
		if (actions) actions->Release();
		if (trigger) trigger->Release();
		if (triggers) triggers->Release();
		if (taskDefinition) taskDefinition->Release();
		if (rootFolder) rootFolder->Release();
		cleanup();
		return false;
	}

	// Регистрация задачи для текущего пользователя без прав администратора
	IRegisteredTask* registeredTask = nullptr;
	hr = rootFolder->RegisterTaskDefinition(
		_bstr_t(taskName.toStdWString().c_str()),
		taskDefinition,
		TASK_CREATE_OR_UPDATE,
		_variant_t(), // Задача выполняется для текущего пользователя
		_variant_t(), // Без пароля
		TASK_LOGON_INTERACTIVE_TOKEN, // Логон-токен для текущего пользователя
		_variant_t(),
		&registeredTask
	);
	if (FAILED(hr)) {
		qWarning() << "Failed to register task.";
		if (execAction) execAction->Release();
		if (action) action->Release();
		if (actions) actions->Release();
		if (trigger) trigger->Release();
		if (triggers) triggers->Release();
		if (taskDefinition) taskDefinition->Release();
		if (rootFolder) rootFolder->Release();
		cleanup();
		return false;
	}

	// Освобождаем ресурсы
	if (registeredTask) registeredTask->Release();
	if (execAction) execAction->Release();
	if (action) action->Release();
	if (actions) actions->Release();
	if (trigger) trigger->Release();
	if (triggers) triggers->Release();
	if (taskDefinition) taskDefinition->Release();
	if (rootFolder) rootFolder->Release();

	return true;
}

bool TaskSchedulerManager::deleteTask() {
	if (!initializeTaskService()) return false;

	ITaskFolder* rootFolder = nullptr;
	HRESULT hr = taskService->GetFolder(_bstr_t("\\"), &rootFolder);
	if (FAILED(hr)) {
		qWarning() << "Failed to get root folder.";
		cleanup();
		return false;
	}

	// Удаляем задачу
	hr = rootFolder->DeleteTask(_bstr_t(taskName.toStdWString().c_str()), 0);
	if (FAILED(hr)) {
		qWarning() << "Failed to delete task.";
		if (rootFolder) rootFolder->Release();
		cleanup();
		return false;
	}

	if (rootFolder) rootFolder->Release();
	return true;
}
