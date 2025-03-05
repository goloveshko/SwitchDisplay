#include "MainWindow.h"

#include <QApplication>
#include <QThread>
#include <QMessageBox>
#include <QCommandLineParser>
#include <QSharedMemory>
#include <QOperatingSystemVersion>
#ifdef Q_OS_WIN
#include <Windows.h>
#endif // Q_OS_WIN 

static const char* INSTANCE_GUID = "B327B8B8-D9F3-4763-9B39-99DD13D1FD92";

int main(int argc, char *argv[])
{
	QSharedMemory sharedMemory;
	sharedMemory.setKey(INSTANCE_GUID);
	if (sharedMemory.create(1) == false) {
		return 0;
	}

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Goloveshko");
	QCoreApplication::setApplicationName("Switch Display");
	QCoreApplication::setOrganizationDomain("https://github.com/goloveshko/");
	QCoreApplication::setApplicationVersion("0.1");

	QIcon icon = QIcon(":/res/SwitchDisplay.png");
	icon.setIsMask(true);
	app.setWindowIcon(icon);

	QCommandLineParser parser;
	parser.setApplicationDescription(qApp->applicationName());
	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption showSettings(QStringList() << QStringLiteral("s") << QStringLiteral("settings"), QCoreApplication::translate("main", "Show application settings"));
	parser.addOption(showSettings);

	QCommandLineOption setInternal(QStringList() << QStringLiteral("i") << QStringLiteral("internal"), QCoreApplication::translate("main", "Set internal mode"));
	parser.addOption(setInternal);

	parser.process(app);
	bool isShowSettings = parser.isSet(showSettings);
	bool isSetInternal = parser.isSet(setInternal);

	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(0, QObject::tr("System tray"), QObject::tr("I couldn't detect any system tray on this system."));
		return 1;
	}

    QApplication::setQuitOnLastWindowClosed(false);

	MainWindow w(isSetInternal);
	if(isShowSettings) {
		w.showSettings();
	} else if(isSetInternal) {
		//w.showSettings();
		w.changeModeToInternal();
	} else {
		w.changeModeToInternal();
		//w.showSettings();
	}


    int res = app.exec();
    
    return res;
}
