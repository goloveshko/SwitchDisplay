#pragma once
#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QPointer>
#include <QScopedPointer>
#include <QAbstractNativeEventFilter>
#include <Settings.h>
#include <QFile>

#include <QHotkey>

#include "DisplayThread.h"
#include "SettingsHelper.h"

class MainWindow :
    public QWidget, public QAbstractNativeEventFilter
{
Q_OBJECT

public:
	explicit MainWindow(bool isExit = false, QWidget* parent = nullptr);
	~MainWindow();

	void createTrayIcon();

	//bool nativeEventFilter(const QByteArray& eventType, void* message, long* result);
	bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result);

	void changeModeToInternal();
	void changeModeToExtend();

	void showSettings();

private:
	void slotAboutToShowTrayMenu();
	void slotAboutToHideTrayMenu();

	void slotShowSettings();

	void writeToLog(const QString& text);

	void closeEvent(QCloseEvent* event);

	void changeModeToLog(const QString &result, const QString &previousTopology, const QString &currentTopology);

	void startDisplayThread(DISPLAYCONFIG_TOPOLOGY_ID topologyId);
	void stopDisplayThread();

	void registerHotKey();
	void slotRunExternalApp();

	void loadSettings();

	virtual bool winEventFilter(MSG *msg, long *result);

private slots:
	void slotModeChanged(long result, const QString &previousTopology, const QString &newTopology);

private:
	QPointer <QAction>				actionShowSettings;
	QPointer <QAction>				actionQuit;
	QPointer <QSystemTrayIcon>		trayIcon;
	QPointer <QMenu>				trayIconMenu;
	QPointer <Settings>				settings;
	QPointer <QHotkey>				hotkey;
	QScopedPointer <SettingsHelper>	settingsHelper;
	QScopedPointer <QFile>			logFile;
	QScopedPointer <DisplayThread>	displayThread;
	bool							exitOnDone;
	
};

