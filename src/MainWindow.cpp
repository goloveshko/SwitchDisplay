#include "MainWindow.h"
#include <QApplication>
#include <QDebug>
#include <QDateTime>
#include <QProcess>
#include <QThread>
#include <QTimer>


MainWindow::MainWindow(bool isExit, QWidget *parent)
	: QWidget(parent)
	, hotkey(new QHotkey(this))
	, settingsHelper(new SettingsHelper(this))
	, settings(new Settings(this))
	, audioDeviceManager(new AudioDeviceManager(this))
	, isDefaultMode(true)
	, exitOnDone(isExit) {
	displayThread.reset(new DisplayThread(this));
	auto devices = audioDeviceManager->getAudioDevicesInfo();
	settings->updateAudioDevices(devices);

	connect(displayThread.get(), &DisplayThread::signalModeChanged, this, &MainWindow::slotModeChanged);
	connect(audioDeviceManager.get(), &AudioDeviceManager::signalDeviceAdded, this, &MainWindow::slotAudioDeviceAdded);
	connect(audioDeviceManager.get(), &AudioDeviceManager::signalDeviceAdded, this, &MainWindow::slotAudioDeviceRemoved);

	writeToLog("Started");

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
	}
	else
	{
		settings->show();
		settings->raise();
		settings->activateWindow();
	}
}

void MainWindow::writeToLog(const QString& text) {
	settings->addToLog(text);
	//return;
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
}

void MainWindow::changeModeToExternal() {
	startDisplayThread(DISPLAYCONFIG_TOPOLOGY_EXTERNAL);
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
	isDefaultMode = topologyId == DISPLAYCONFIG_TOPOLOGY_INTERNAL;
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

	currentTopology = newTopology;

	if(currentTopology == "external") {
		auto deviceId = settings->getAudioToId();
		//auto deviceId = settings->getAudioDefaultId();
		audioDeviceManager->setDefaultAudioDevice(deviceId);

		auto devicename = audioDeviceManager->getAudioDeviceName(deviceId);
		qDebug() << "Run external app:" << devicename << "|" << deviceId;
		writeToLog("Run external app: " + devicename + " | " + deviceId);

		QTimer::singleShot(1000, this, &MainWindow::slotRunExternalApp);
	}
	else {
		auto deviceId = settings->getAudioDefaultId();
		//auto deviceId = settings->getAudioToId();
		audioDeviceManager->setDefaultAudioDevice(deviceId);
		
		auto devicename = audioDeviceManager->getAudioDeviceName(deviceId);
		qDebug() << "Run internal app:" << devicename << "|" << deviceId;
		writeToLog("Run internal app: " + devicename + " | " + deviceId);
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
	//qDebug() << "Is segistered:" << hotkey->isRegistered();

	connect(hotkey, &QHotkey::activated, [&]() {
		if(settings->isVisible()) {
			//return;
		}
		QString currentTopology = displayThread->getDisplayCurrentTopologyString();
		if(currentTopology == "internal") {
			//qDebug() << "DISPLAYCONFIG_TOPOLOGY_EXTERNAL";
			writeToLog("Hot key external");
			changeModeToExternal();
			//changeModeToInternal();
		}
		else {
			//qDebug() << "DISPLAYCONFIG_TOPOLOGY_INTERNAL";
			writeToLog("Hot key internal");
			changeModeToInternal();
		}
	});
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

void MainWindow::slotAudioDeviceAdded(const QString& deviceName, const QString& deviceId)
{
	if (settings->watchNewAudio() && currentTopology == "external")
		audioDeviceManager->setDefaultAudioDevice(deviceId);

	QString resultStr = QString("slotAudioDeviceAdded: Topology(%1), Name(%2), ID(%3)").arg(currentTopology).arg(deviceName).arg(deviceId);
	writeToLog(resultStr);
}

void MainWindow::slotAudioDeviceRemoved(const QString& deviceName, const QString& deviceId)
{
	QString resultStr = QString("slotAudioDeviceRemoved: Topology(%1), Name(%2), ID(%3)").arg(currentTopology).arg(deviceName).arg(deviceId);
	writeToLog(resultStr);
}