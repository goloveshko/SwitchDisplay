#pragma once
#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QPointer>
#include <QScopedPointer>
#include <QAbstractNativeEventFilter>
#include <Settings.h>
#include <QFile>

class MainWindow :
    public QWidget, public QAbstractNativeEventFilter
{
Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();

	void createTrayIcon();

	bool nativeEventFilter(const QByteArray& eventType, void* message, long* result);

private:
	void slotAboutToShowTrayMenu();
	void slotAboutToHideTrayMenu();

	void slotShow();

	void writeToLog(const QString& text);

	void changeModeToInternal();
	void changeModeToExtend();
	void closeEvent(QCloseEvent* event);

private:
	QPointer <QAction>			actionShowSettings;
	QPointer <QAction>			actionQuit;
	QPointer <QSystemTrayIcon>	trayIcon;
	QPointer <QMenu>			trayIconMenu;
	QPointer <Settings>			settings;
	QScopedPointer<QFile>		logFile;
};

