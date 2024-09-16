#include "MainWindow.h"
#include <QApplication>
#include <QDebug>
#include <QDateTime>
#include <QProcess>
#include <QThread>
#include <QTimer>


// #include <windows.h>
// #pragma comment(lib, "user32.lib")

// #include <WtsApi32.h>
// #pragma comment(lib, "Wtsapi32.lib")

MainWindow::MainWindow(bool isExit, QWidget *parent)
	: QWidget(parent)
	, hotkey(new QHotkey(this))
	, settingsHelper(new SettingsHelper(this))
	, settings(new Settings(this))
	, exitOnDone(isExit) {
	//WTSRegisterSessionNotification((HWND)this->winId(), NOTIFY_FOR_THIS_SESSION);
	displayThread.reset(new DisplayThread(this));

	connect(displayThread.get(), &DisplayThread::signalModeChanged, this, &MainWindow::slotModeChanged);

	//settings = new Settings();

	//logFile.reset(new QFile(QCoreApplication::applicationDirPath() + "/log.txt"));
	//logFile->open(QFile::Append | QFile::Text | QFile::ReadWrite);

	writeToLog("Started");

	//changeModeToInternal();

	//QTimer::singleShot(1000, [this]() { this->createTrayIcon(); });

	if(!exitOnDone) {
		loadSettings();
		createTrayIcon();
		registerHotKey();
	}

	qApp->installNativeEventFilter(this);
}

MainWindow::~MainWindow() {
	writeToLog("Stopped");
	stopDisplayThread();
	//WTSUnRegisterSessionNotification((HWND)this->winId());
}

void MainWindow::createTrayIcon() {
	actionShowSettings = new QAction(tr("&Show"), this);
	connect(actionShowSettings, &QAction::triggered, this, &MainWindow::slotShowSettings);

	actionQuit = new QAction(tr("&Quit"), this);
	connect(actionQuit, &QAction::triggered, qApp, &QApplication::quit);

	trayIconMenu = new QMenu();
	trayIconMenu->addAction(actionShowSettings);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(actionQuit);

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);

	QIcon icon = QIcon(":/res/SwitchDisplay.png");
	icon.setIsMask(true);
	trayIcon->setIcon(icon);

	trayIcon->setToolTip(tr("Switch Display Mode"));
	trayIcon->show();

	connect(trayIcon, &QSystemTrayIcon::activated, [&](QSystemTrayIcon::ActivationReason reason) {
		if(reason == QSystemTrayIcon::Trigger) {
			slotShowSettings();
		}
	});

	connect(trayIconMenu, &QMenu::aboutToShow, this, &MainWindow::slotAboutToShowTrayMenu);
	connect(trayIconMenu, &QMenu::aboutToHide, this, &MainWindow::slotAboutToHideTrayMenu);
}

void MainWindow::slotAboutToShowTrayMenu() {
	if (actionShowSettings.isNull())
	{
		return;
	}

	if (settings->isVisible())
	{
		actionShowSettings->setText(tr("&Hide Settings"));
	}
	else
	{
		actionShowSettings->setText(tr("&Show Settings"));
	}
}

void MainWindow::slotAboutToHideTrayMenu() {

}

void MainWindow::slotShowSettings() {
	if (settings->isVisible())
	{
		settings->hide();
		//hotkey->setRegistered(true);
	}
	else
	{
		//hotkey->setRegistered(false);
		settings->show();
		settings->raise();
		settings->activateWindow();
	}

	//raise();
	//activateWindow();
}
/*
bool MainWindow::nativeEventFilter(const QByteArray& eventType, void* message, long* result) {
	//if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
	{
		MSG* pMsg = reinterpret_cast<MSG*>(message);
		
		if (pMsg->message == WM_ENDSESSION)
		{
			writeToLog("WM_ENDSESSION received");
			changeModeToInternal();
		}

		if (pMsg->message == WM_QUERYENDSESSION)
		{
			writeToLog("WM_QUERYENDSESSION received");
			changeModeToInternal();
		}
	}
	return false;
}
*/
void MainWindow::writeToLog(const QString& text) {
	settings->addToLog(text);
	return;
	if (!logFile.get()) {
		logFile.reset(new QFile(QCoreApplication::applicationDirPath() + "/log.txt"));
		logFile->open(QFile::Append | QFile::Text | QFile::ReadWrite);
	}

	QTextStream log(logFile.get());
	log << QDateTime::currentDateTime().toString() << " - " << text << Qt::endl; //Qt::ISODate
}

void MainWindow::changeModeToLog(const QString &result, const QString &previousTopology, const QString &currentTopology) {
	QApplication::beep();
	QString str = QString("%1 Change display mode from %2 to %3").arg(result, previousTopology, currentTopology);
	writeToLog(str);
}

void MainWindow::changeModeToInternal() {
	startDisplayThread(DISPLAYCONFIG_TOPOLOGY_INTERNAL);
// 	QString previousTopology;
// 	bool result = DisplayThread().Internal(previousTopology);
// 
// 	int count = 20;
// 
// 	while(!result && previousTopology != "internal" && count > 0) {
// 		QThread::usleep(100);
// 		count--;
// 		result = DisplayThread().Internal(previousTopology);
// 	}


// 	bool result = true;
// 
// 	QStringList env = QProcess::systemEnvironment();
// 	QString program = "DisplaySwitch.exe";
// 	QStringList arguments = QStringList() << "/internal";
// 	QProcess* process = new QProcess(qApp);
// 	process->setEnvironment(env);
// 	process->start(program, arguments);

	//changeModeToLog(result, previousTopology, "internal");
}

void MainWindow::changeModeToExtend() {
// 	QString previousTopology;
// 	bool result = DisplayThread().Extend(previousTopology);
// 	changeModeToLog(result, previousTopology, "extend");
}

void MainWindow::closeEvent(QCloseEvent* event) {
	writeToLog("closeEvent received");
	changeModeToInternal();
	QWidget::closeEvent(event);
}

void MainWindow::startDisplayThread(DISPLAYCONFIG_TOPOLOGY_ID topologyId) {
	writeToLog(QString("Start display thread 0x%1").arg(topologyId, 8, 16, QLatin1Char('0')));
	stopDisplayThread();
	displayThread->setDisplayMode(topologyId);
	displayThread->start();
}

void MainWindow::stopDisplayThread() {
	if(displayThread && displayThread->isRunning()) {
		displayThread->requestInterruption();
		int to = 10;
		while(displayThread->isRunning() && (to--))
			displayThread->wait(100);
		displayThread->quit();
	}
}

void MainWindow::slotModeChanged(long result, const QString &previousTopology, const QString &newTopology) {
	QString resultStr = QString("%1(0x%2)").arg(qt_error_string(result)).arg(result, 8, 16, QLatin1Char('0'));
	changeModeToLog(resultStr, previousTopology, newTopology);

	if(newTopology == "external") {
		qDebug() << "Run external app";
		writeToLog("Run external app");
		QTimer::singleShot(1000, this, &MainWindow::slotRunExternalApp);
	}

	if(exitOnDone){//settingsHelper->getExitOnDone()) {
		QApplication::quit();
	}
}

void MainWindow::showSettings() {
	slotShowSettings();
}

void MainWindow::registerHotKey() {
	hotkey->setShortcut(QKeySequence(settingsHelper->getKeySequence()), true); // QKeySequence("Meta+Y"), true);
	qDebug() << "Is segistered:" << hotkey->isRegistered();

	connect(hotkey, &QHotkey::activated, [&]() {
		if(settings->isVisible()) {
			return;
		}
		QString currentTopology = displayThread->getDisplayCurrentTopologyString();
		if(currentTopology == "internal") {
			qDebug() << "DISPLAYCONFIG_TOPOLOGY_EXTERNAL";
			writeToLog("Hot key external");
			startDisplayThread(DISPLAYCONFIG_TOPOLOGY_EXTERNAL);
		}
		else {
			qDebug() << "DISPLAYCONFIG_TOPOLOGY_INTERNAL";
			writeToLog("Hot key internal");
			startDisplayThread(DISPLAYCONFIG_TOPOLOGY_INTERNAL);
		}
	});

// 	connect(hotkey, &QHotkey::activated, qApp, [&]() {
// 		qDebug() << "Hotkey Activated - the application will quit now";
// 		qApp->quit();
// 	});
}

void MainWindow::slotRunExternalApp() {
	QString program = settingsHelper->getApplicationPath();//"C:\\Program Files\\Kodi\\kodi.exe";

	if(!settingsHelper->getAutorunApp() || program.isEmpty()) {
		return;
	}

	QStringList arguments = QStringList();

	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	QProcess *process = new QProcess(this);
	process->setProcessEnvironment(env);
	process->start(program, arguments);

    connect(process, &QProcess::readyReadStandardOutput, [=, this]() {
		QString output = process->readAllStandardOutput();
		writeToLog(output);
		//addConsoleOutput(output);
		qDebug() << output.trimmed();
	});

    connect(process, &QProcess::readyReadStandardError, [=, this]() {
		QString output = process->readAllStandardError();
		writeToLog(output);
		//addConsoleOutput(output);
		qDebug() << output.trimmed();
	});

	connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
		if(!process)
			return;
		process->deleteLater();
// 		if(settingsHelper->getExitOnDone()) {
// 			QApplication::quit();
// 		}
	}, Qt::QueuedConnection);
}

void MainWindow::loadSettings() {
	settings->setAutostart(settingsHelper->getAutostart());
	settings->setRunApplicationPath(settingsHelper->getApplicationPath());
	settings->setAutorunApplication(settingsHelper->getAutorunApp());
	settings->setKeySequence(settingsHelper->getKeySequence());

	connect(settings, &Settings::signalAutostart, [this](Qt::CheckState state) {
		settingsHelper->setAutostart(state != 0);
	});

	connect(settings, &Settings::signalAutorunApp, [this](Qt::CheckState state) {
		settingsHelper->setAutorunApp(state != 0);
	});

	connect(settings, &Settings::signalShowBrowseDialog, [this]() {
		settings->setRunApplicationPath(settingsHelper->showApplicationPath(this));
	});
	
	connect(settings, &Settings::signalKeySequenceChanged, [this](const QKeySequence &keySequence) {
		settingsHelper->setKeySequence(keySequence.toString(QKeySequence::NativeText));
		hotkey->setShortcut(QKeySequence(settingsHelper->getKeySequence()), true);
	});
}
bool MainWindow::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) {
//bool MainWindow::nativeEventFilter(const QByteArray &eventType, void *message, long *result) {
	Q_UNUSED(eventType);
	Q_UNUSED(result);
	MSG *m = static_cast<MSG *>(message);
	if(m->message == WM_HOTKEY) {
		qDebug() << "nativeEventFilter";
	}
	return false;
}

bool MainWindow::winEventFilter(MSG *msg, long *result) {
	switch(msg->message) {
	case 0x0319: /* WM_APPCOMMAND 0x0319 */
		DefWindowProc(msg->hwnd, msg->message,
					  msg->wParam, msg->lParam);

		qDebug() << "winEventFilter";
		break;
	}
	return false;
}
