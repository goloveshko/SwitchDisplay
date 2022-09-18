#include "MainWindow.h"

#include <QApplication>
#include <QThread>
#include <QMessageBox>
#include <QCommandLineParser>
#ifdef Q_OS_WIN
#include <Windows.h>
#endif // Q_OS_WIN 

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
	//SetProcessDPIAware(); // call before the main event loop
#endif // Q_OS_WIN 

	// Set sensible defaults
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

/*
#if QT_VERSION >= QT_VERSION_CHECK(5,6,0)
	//QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
// 	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // DPI support
// 	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps); //HiDPI pixmaps

		//QSettings settings(_organizationName, "3");
	bool automaticScaling = true;//settings.value(SettingKey::toString(SettingKey::APPEARANCE_AUTO_HIDPI_SCALING), false).toBool();

	qunsetenv("QT_DEVICE_PIXEL_RATIO");
	if(qgetenv("QT_AUTO_SCREEN_SCALE_FACTOR").isEmpty()) {
		qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", automaticScaling ? "1" : "0");
// 		QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // DPI support
// 		QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps); //HiDPI pixmaps
	}

	QApplication::setAttribute(Qt::AA_DisableHighDpiScaling, false);
	QApplication::setHighDpiScaleFactorRoundingPolicy(
		Qt::HighDpiScaleFactorRoundingPolicy::Floor);
	
	//qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", QByteArray("1"));
#else
	qputenv("QT_DEVICE_PIXEL_RATIO", QByteArray("1"));
#endif // QT_VERSION


// 	if(qgetenv("QT_SCALE_FACTOR").isEmpty() && !automaticScaling) {
// 		auto factor = 60 / 100.0;//settings.value(SettingKey::toString(SettingKey::APPEARANCE_HIDPI_SCALING_FACTOR), 100).toInt() / 100.0;
// 		qputenv("QT_SCALE_FACTOR", QString::number(factor).toLocal8Bit());
// 	}
*/
	//qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    QApplication app(argc, argv);
	//qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
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
	}
// 	} else {
// 		w.changeModeToInternal();
// 	}

    int res = app.exec();
    
    return res;
}
