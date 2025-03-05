#include "SettingsHelper.h"
#include <QSettings>
#include <QCoreApplication>
#include <QFileDialog>
#include <QTextStream>
#include "TaskSchedulerManager.h"
#include "AuthorizationHelper.h"

#define APPLICATION_PATH "Settings/Application path"
#define AUTOSTART_KEY qApp->applicationName()
#define AUTOSTART_PATH "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"

#define EXIT_ON_DONE	"Settings/Exit on done"
#define AUTORUN_APP		"Settings/Autorun app"
#define KEY_SEQUENCE	"Settings/Key sequence"
#define KEY_GEOMETRY	"Settings/Geometry"
#define KEY_MODE_DEFAULT "Settings/ModeDefault"
#define KEY_MODE_TO		"Settings/ModeTo"
#define KEY_WATCH_NEW_AUDIO	"Settings/WatchNewAudio"

#define KEY_AUDIO_DEFAULT "Settings/AudioDefault"
#define KEY_AUDIO_TO "Settings/AudioTo"

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

bool SettingsHelper::getAutostart() const {
	TaskSchedulerManager manager;
	bool result = manager.taskExists();
	return result;
}

void SettingsHelper::setAutostart(bool autostart) {
	QStringList arguments;
	arguments << (autostart ? "-i" : "-u");
	
	const QString program = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("autorun.exe");
	AuthorizationHelper::execute(program, arguments);
}

void SettingsHelper::setExitOnDone(bool exitOnDone) {
	QSettings settings;
	settings.setValue(EXIT_ON_DONE, exitOnDone);
}

bool SettingsHelper::getExitOnDone() const {
	QSettings settings;
	bool result = settings.value(EXIT_ON_DONE, false).toBool();
	return result;
}


void SettingsHelper::setAutorunApp(bool autorunApp) {
	QSettings settings;
	settings.setValue(AUTORUN_APP, autorunApp);
}

bool SettingsHelper::getAutorunApp() const  {
	QSettings settings;
	bool result = settings.value(AUTORUN_APP, false).toBool();
	return result;
}

void SettingsHelper::setKeySequence(const QString &keySequence) {
	QSettings settings;
	settings.setValue(KEY_SEQUENCE, keySequence);
}

QString SettingsHelper::getKeySequence() const  {
	QSettings settings;
	QString keySequence = settings.value(KEY_SEQUENCE).toString();

	return keySequence;
}

void SettingsHelper::setWindowState(const QByteArray &geometry) {
	QSettings settings;
	settings.setValue(KEY_GEOMETRY, geometry);
}

QByteArray SettingsHelper::getWindowState() const {
	QSettings settings;
	const QByteArray geometry = settings.value(KEY_GEOMETRY).toByteArray();
	return geometry;
}

void SettingsHelper::setModeDefault(const QString& modeTo) {
	QSettings settings;
	settings.setValue(KEY_MODE_DEFAULT, modeTo);
}

QString SettingsHelper::getModeDefault() const {
	QSettings settings;
	QString modeTo = settings.value(KEY_MODE_DEFAULT).toString();
	return modeTo;
}

void SettingsHelper::setModeTo(const QString& modeTo) {
	QSettings settings;
	settings.setValue(KEY_MODE_TO, modeTo);
}

QString SettingsHelper::getModeTo() const {
	QSettings settings;
	QString modeTo = settings.value(KEY_MODE_TO).toString();
	return modeTo;
}

void SettingsHelper::setAudioDefault(const QString& value)
{
	QSettings settings;
	settings.setValue(KEY_AUDIO_DEFAULT, value);
}

QString SettingsHelper::getAudioDefault() const
{
	QSettings settings;
	QString value = settings.value(KEY_AUDIO_DEFAULT).toString();
	return value;
}

void SettingsHelper::setAudioTo(const QString& value)
{
	QSettings settings;
	settings.setValue(KEY_AUDIO_TO, value);
}

QString SettingsHelper::getAudioTo() const
{
	QSettings settings;
	QString value = settings.value(KEY_AUDIO_TO).toString();
	return value;
}

void SettingsHelper::setWatchNewAudio(bool value)
{
	QSettings settings;
	settings.setValue(KEY_WATCH_NEW_AUDIO, value);
}

bool SettingsHelper::getWatchNewAudio() const
{
	QSettings settings;
	bool value = settings.value(KEY_WATCH_NEW_AUDIO).toBool();
	return value;
}
