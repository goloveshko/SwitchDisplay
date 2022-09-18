#include "SettingsHelper.h"
#include <QSettings>
#include <QCoreApplication>
#include <QFileDialog>
#include <QTextStream>


#define APPLICATION_PATH "Settings/Application path"
#define AUTOSTART_KEY qApp->applicationName()
#define AUTOSTART_PATH "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"

#define EXIT_ON_DONE "Settings/Exit on done"
#define AUTORUN_APP "Settings/Autorun app"
#define KEY_SEQUENCE "Settings/Key sequence"

SettingsHelper::SettingsHelper(QObject *parent)
    : QObject(parent)
{
}

SettingsHelper::~SettingsHelper()
{
}


QString SettingsHelper::showApplicationPath(QWidget *parent)
{
	QString defaultFile = getApplicationPath();
	QString file = QFileDialog::getOpenFileName(parent, tr("Run Script File"), defaultFile);
	if(!file.isEmpty())
	{
		setApplicationPath(file);
	}
	return file;
}

void SettingsHelper::setApplicationPath(const QString &script) {
	QSettings settings;
	settings.setValue(APPLICATION_PATH, script);
}

QString SettingsHelper::getApplicationPath(const QString &key)
{
	QSettings settings;
	QString result = settings.value(key).toString();

	return result;
}

QString SettingsHelper::getApplicationPath()
{
	QString result = getApplicationPath(APPLICATION_PATH);
	if(!QFile::exists(result)) {
		//result = writeDefaultScript();
	}
	return result;
}

void SettingsHelper::setAutostart(bool autostart) {
	QSettings settings(AUTOSTART_PATH, QSettings::NativeFormat);
	if(autostart) {
		settings.setValue(AUTOSTART_KEY, QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
	} else {
		settings.remove(AUTOSTART_KEY);
	}
}

bool SettingsHelper::getAutostart() {
	QSettings settings(AUTOSTART_PATH, QSettings::NativeFormat);
	bool result = settings.contains(AUTOSTART_KEY);
	return result;
}

void SettingsHelper::setExitOnDone(bool exitOnDone) {
	QSettings settings;
	settings.setValue(EXIT_ON_DONE, exitOnDone);
}

bool SettingsHelper::getExitOnDone() {
	QSettings settings;
	bool result = settings.value(EXIT_ON_DONE, false).toBool();
	return result;
}

void SettingsHelper::setAutorunApp(bool autorunApp) {
	QSettings settings;
	settings.setValue(AUTORUN_APP, autorunApp);
}

bool SettingsHelper::getAutorunApp() {
	QSettings settings;
	bool result = settings.value(AUTORUN_APP, false).toBool();
	return result;
}

void SettingsHelper::setKeySequence(const QString &keySequence) {
	QSettings settings;
	settings.setValue(KEY_SEQUENCE, keySequence);
}

QString SettingsHelper::getKeySequence() {
	QSettings settings;
	QString keySequence = settings.value(KEY_SEQUENCE).toString();

	return keySequence;
}

/*
QString SettingsHelper::getRecordingDirectoryFfmpeg()
{
	QString result = getRecordingDirectory(RECORDING_DIRECTORY_FFMPEG);

	return result;
}

void SettingsHelper::setRecordingDirectory(const QString &key, const QString &directory)
{
	QString fileName = getSettingsPath();
	QSettings settings(fileName, QSettings::NativeFormat);
	settings.setValue(key, directory);
}

QString SettingsHelper::getRecordingDuration()
{
	QString fileName = getSettingsPath();
	QSettings settings(fileName, QSettings::NativeFormat);
	QString result = settings.value(RECORDING_DURATION, "20").toString();

	return result;
}

void SettingsHelper::setRecordingDuration(const QString &duration)
{
	QString fileName = getSettingsPath();
	QSettings settings(fileName, QSettings::NativeFormat);
	settings.setValue(RECORDING_DURATION, duration);
}

void SettingsHelper::saveDBParams(const QString &hostname, const QString &port, const QString &username, const QString &password)
{
	QString fileName = getSettingsPath();

	QSettings settings(fileName, QSettings::NativeFormat);
	settings.beginGroup("MySQL");
	settings.setValue(MYSQL_HOSTNAME, hostname);
	settings.setValue(MYSQL_PORT, port);
	settings.setValue(MYSQL_USERNAME, username);
	settings.setValue(MYSQL_PASSWORD, setParams(MYSQL_PASSWORD, password));
	QSettings::Status status = settings.status();
	settings.endGroup();
}

void SettingsHelper::loadDBParams(QString &hostname, QString &port, QString &username, QString &password)
{
	QString fileName = getSettingsPath();
	QSettings settings(fileName, QSettings::NativeFormat);

	settings.beginGroup("MySQL");
	hostname = settings.value(MYSQL_HOSTNAME, "127.0.0.1").toString();
	port = settings.value(MYSQL_PORT, "3306").toString();
	username = settings.value(MYSQL_USERNAME, "user").toString();
	password = getParams(MYSQL_PASSWORD, settings.value(MYSQL_PASSWORD).toString());

	settings.endGroup();
}

QString SettingsHelper::getParams(const QString &key, const QString &value)
{
	QByteArray ba1;
	ba1.append(key.toUtf8());
	ba1 = ba1.toHex();

	QByteArray ba2;
	ba2.append(value.toUtf8());

	QString result = QByteArray::fromBase64(ba2);
	result = result.left(result.count() - ba1.count());

	return result;
}

QString SettingsHelper::setParams(const QString &key, const QString &value)
{
	QByteArray ba1;
	ba1.append(key.toUtf8());
	ba1 = ba1.toHex();

	QByteArray ba2;
	ba2.append(value.toUtf8() + ba1);
	return ba2.toBase64();
}

QString SettingsHelper::showFfmpegExe(QWidget *parent)
{
	QString defDir = SettingsHelper::getRecordingFfmpegExe();
	QString ffmpegExe = QFileDialog::getOpenFileName(parent, QString(), defDir, "ffmpeg.exe");
	if(!ffmpegExe.isEmpty())
	{
		setRecordingFfmpegExe(ffmpegExe);
	}
	return ffmpegExe;
}

void SettingsHelper::setRecordingFfmpegExe(const QString &ffmpegExe)
{
	QString fileName = getSettingsPath();
	QSettings settings(fileName, QSettings::NativeFormat);
	settings.setValue(RECORDING_FFMPEG_EXE, ffmpegExe);
}

QString SettingsHelper::getRecordingFfmpegExe()
{
	QString defArg = "";
	QString fileName = getSettingsPath();
	QSettings settings(fileName, QSettings::NativeFormat);
	QString result = settings.value(RECORDING_FFMPEG_EXE, defArg).toString();

	return result;
}

QString SettingsHelper::showRecordingDirectoryFfmpeg(QWidget *parent)
{
	QString defDir = SettingsHelper::getRecordingDirectory(RECORDING_DIRECTORY_FFMPEG);
	QString dir = QFileDialog::getExistingDirectory(parent, tr("Open Directory"), defDir, QFileDialog::ShowDirsOnly);
	if(!dir.isEmpty())
	{
		setRecordingDirectory(RECORDING_DIRECTORY_FFMPEG, dir);
	}
	return dir;
}

QString SettingsHelper::showNginxExe(QWidget *parent)
{
	QString defDir = SettingsHelper::getNginxExe();
	QString exe = QFileDialog::getOpenFileName(parent, QString(), defDir, "nginx.exe");
	if(!exe.isEmpty())
	{
		setNginxExe(exe);
	}
	return exe;
}

void SettingsHelper::setNginxExe(const QString &nginxExe)
{
	QString fileName = getSettingsPath();
	QSettings settings(fileName, QSettings::NativeFormat);
	settings.setValue(NGINX_EXE, nginxExe);
}

QString SettingsHelper::getNginxExe()
{
	QString defArg = "";
	QString fileName = getSettingsPath();
	QSettings settings(fileName, QSettings::NativeFormat);
	QString result = settings.value(NGINX_EXE, defArg).toString();

	return result;
}

void SettingsHelper::setRecordingEnable(const QString &key, bool enable)
{
	QString fileName = getSettingsPath();
	QSettings settings(fileName, QSettings::NativeFormat);
	settings.setValue(key, enable);
}

bool SettingsHelper::getRecordingEnable(const QString &key, bool defArg)
{
	QString fileName = getSettingsPath();
	QSettings settings(fileName, QSettings::NativeFormat);
	bool result = settings.value(key, defArg).toBool();

	return result;
}

void SettingsHelper::setRecordingEnable(bool enable)
{
	setRecordingEnable(RECORDING_ENABLE, enable);
}

bool SettingsHelper::getRecordingEnable()
{
	bool result = getRecordingEnable(RECORDING_ENABLE, false);
	return result;
}

void SettingsHelper::setRecordingFfmpegEnable(bool enable)
{
	setRecordingEnable(RECORDING_FFMPEG_ENABLE, enable);
}

bool SettingsHelper::getRecordingFfmpegEnable()
{
	bool result = getRecordingEnable(RECORDING_FFMPEG_ENABLE, false);
	return result;
}

void SettingsHelper::setNginxEnable(bool enable)
{
	setRecordingEnable(NGINX_ENABLE, enable);
}

bool SettingsHelper::getNginxEnable()
{
	bool result = getRecordingEnable(NGINX_ENABLE, false);
	return result;
}

bool SettingsHelper::setExportDirectory(const QString &directory)
{
	bool result = false;
	QSettings settings;

	if(!directory.isEmpty())
	{
		settings.setValue(EXPORTING_DIRECTORY, directory);
		result = true;
	}

	return result;
}

QString SettingsHelper::getExportDirectory()
{
	QSettings settings;
	QString result = settings.value(EXPORTING_DIRECTORY, QCoreApplication::applicationDirPath()).toString();
	return result;
}

QString SettingsHelper::showExportingDirectory(QWidget *parent)
{
	QString defDir = getExportDirectory();
	QString dir = QFileDialog::getExistingDirectory(parent, tr("Open Directory"), defDir, QFileDialog::ShowDirsOnly);
	setExportDirectory(dir);
	return dir;
}

bool SettingsHelper::setIP(const QString &ip)
{
	bool result = false;
	QString fileName = getSettingsPath();
	QSettings settings(fileName, QSettings::NativeFormat);

	if(!ip.isEmpty())
	{
		settings.setValue(FFMPEG_IP, ip);
		result = true;
	}

	return result;
}

QString SettingsHelper::getIP()
{
	QString fileName = getSettingsPath();
	QSettings settings(fileName, QSettings::NativeFormat);
	QString defHost = NetworkUtil::getIPs().first();
	QString result = settings.value(FFMPEG_IP, defHost).toString();
	return result;
}
*/