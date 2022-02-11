#include "MainWindow.h"
#include <QApplication>
#include <QDebug>
#include <QDateTime>
#include <QProcess>

#include "Display.h"

#include <windows.h>
//#pragma comment(lib, "user32.lib")

#include <WtsApi32.h>
#pragma comment(lib, "Wtsapi32.lib")

MainWindow::MainWindow(QWidget* parent)
	: QWidget(parent) {
	WTSRegisterSessionNotification((HWND)this->winId(), NOTIFY_FOR_THIS_SESSION);
	settings = new Settings();

	logFile.reset(new QFile(QCoreApplication::applicationDirPath() + "/log.txt"));
	logFile->open(QFile::Append | QFile::Text | QFile::ReadWrite);

	writeToLog("MainWindow started");

	changeModeToInternal();

	createTrayIcon();
}

MainWindow::~MainWindow() {
	writeToLog("MainWindow stopped");
	WTSUnRegisterSessionNotification((HWND)this->winId());
}

void MainWindow::createTrayIcon() {
	actionShowSettings = new QAction(tr("&Show"), this);
	connect(actionShowSettings, &QAction::triggered, this, &MainWindow::slotShow);

	actionQuit = new QAction(tr("&Quit"), this);
	connect(actionQuit, &QAction::triggered, qApp, &QApplication::quit);

	trayIconMenu = new QMenu();
	trayIconMenu->addAction(actionShowSettings);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(actionQuit);

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);

	QIcon icon = QIcon(":/res/icon.png");
	icon.setIsMask(true);
	trayIcon->setIcon(icon);

	trayIcon->setToolTip(tr("Switch Display Mode"));
	trayIcon->show();

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

void MainWindow::slotShow() {
	if (settings->isVisible())
	{
		settings->hide();
	}
	else
	{
		settings->show();
	}

	//raise();
	//activateWindow();
}

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

void MainWindow::writeToLog(const QString& text) {
	QTextStream log(logFile.get());
	log << QDateTime::currentDateTime().toString() << " - " << text << Qt::endl; //Qt::ISODate
}

void MainWindow::changeModeToInternal() {
	//bool result = Display::Internal();
	bool result = true;

	QStringList env = QProcess::systemEnvironment();
	QString program = "DisplaySwitch.exe";
	QStringList arguments = QStringList() << "/internal";
	QProcess* process = new QProcess(qApp);
	process->setEnvironment(env);
	process->start(program, arguments);

	QString currentTopologyString = Display::getDisplayCurrentTopologyString();
	QString str = QString("%1(%2) - change mode to internal").arg(result ? "success" : "fall", currentTopologyString);
	writeToLog(str);
}

void MainWindow::changeModeToExtend() {
	bool result = Display::Extend();

	QString currentTopologyString = Display::getDisplayCurrentTopologyString();
	QString str = QString("%1(%2) - change mode to extend").arg(result ? "success" : "fall", currentTopologyString);
	writeToLog(str);
}

void MainWindow::closeEvent(QCloseEvent* event) {
	writeToLog("closeEvent received");
	changeModeToInternal();
	QWidget::closeEvent(event);
}