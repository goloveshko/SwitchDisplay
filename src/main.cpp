#include "MainWindow.h"

#include <QApplication>
#include <QThread>
#include <QMessageBox>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	QCoreApplication::setApplicationName("Goloveshko");
	QCoreApplication::setOrganizationDomain("https://github.com/goloveshko/");
	QCoreApplication::setApplicationVersion("0.1");

	QIcon icon = QIcon(":/res/tray.png");
	icon.setIsMask(true);
	a.setWindowIcon(icon);

// 	QCommandLineParser parser;
// 	parser.setApplicationDescription("Switch Display");
// 	parser.addHelpOption();
// 	parser.addVersionOption();
// 
// 	QCommandLineOption showProgressOption("p", QCoreApplication::translate("main", "Show progress during copy"));
// 	parser.addOption(showProgressOption);
// 
// 	QCommandLineOption forceOption(QStringList() << "f" << "force",
// 		QCoreApplication::translate("main", "Overwrite existing files."));
// 	parser.addOption(forceOption);
// 
// 	// An option with a value
// 	QCommandLineOption targetDirectoryOption(QStringList() << "t" << "target-directory",
// 		QCoreApplication::translate("main", "Copy all source files into <directory>."),
// 		QCoreApplication::translate("main", "directory"));
// 	parser.addOption(targetDirectoryOption);
// 
// 	// Process the actual command line arguments given by the user
// 	parser.process(app);
// 
// 	const QStringList args = parser.positionalArguments();
// 	// source is args.at(0), destination is args.at(1)
// 
// 	bool showProgress = parser.isSet(showProgressOption);
// 	bool force = parser.isSet(forceOption);
// 	QString targetDir = parser.value(targetDirectoryOption);



    // Settings w;
    // w.show();
//     SwitchDisplay display;
    //display.External();
    //QApplication::beep();
    //QThread::sleep(5);
//     display.Internal();
//     QApplication::beep();
    //return a.exec();

	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(0, QObject::tr("System tray"), QObject::tr("I couldn't detect any system tray on this system."));
		return 1;
	}

    QApplication::setQuitOnLastWindowClosed(false);

    MainWindow w;
    w.changeModeToInternal();

    //qApp->installNativeEventFilter(&w);

    int res = 0;//a.exec();

    //qApp->removeNativeEventFilter(&w);
    
    return res;//a.exec();
}
