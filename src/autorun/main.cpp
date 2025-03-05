#include <QApplication>
#include <QMessageBox>
#include <QCommandLineParser>
#include <QDir>
#include <QDebug>

#include "AuthorizationHelper.h"
#include "TaskSchedulerManager.h"

bool ifArgument(const QString& arg)
{
	bool result = false;

	QStringList arguments = QCoreApplication::arguments();

	if (arguments.indexOf(arg) > -1)
	{
		result = true;
	}

	return result;
}

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	if (!AuthorizationHelper::hasAdminRights())
	{
		return 1;
	}

	TaskSchedulerManager manager;

	//QString taskName = "Switch Display 3";
	//QString programPath = "C:\\Windows\\System32\\calc.exe";

	bool install = ifArgument("-i");
	bool uninstall = ifArgument("-u");

	if (install) {
		// Проверка существования задачи
		if (manager.taskExists()) {
			qDebug() << "Task already exists.";
		}
		else {
			// Создание задачи
			const QString programPath = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("SwitchDisplay.exe");
			if (manager.createTask(programPath)) {
				qDebug() << "Task created successfully.";
			}
			else {
				qWarning() << "Failed to create task.";
			}
		}
	}
	else if (uninstall) {
		if (manager.deleteTask()) {
			qDebug() << "Task deleted successfully.";
		}
		else {
			qWarning() << "Failed to delete task.";
		}
	}

	return 0;
}
