#pragma once

#include <QObject>
#include <QStandardPaths>

class SettingsHelper: public QObject
{
    Q_OBJECT

public:
    explicit SettingsHelper(QObject *parent = nullptr);
    ~SettingsHelper();

    static QString showApplicationPath(QWidget *parent);
    static void setApplicationPath(const QString &script);
    static QString getApplicationPath(const QString &key);
    static QString getApplicationPath();

    static bool getAutostart();
    static void setAutostart(bool autostart);

    static bool getExitOnDone();
    static void setExitOnDone(bool exitOnDone);

    static void setAutorunApp(bool autorunApp);
    static bool getAutorunApp();

    static void setKeySequence(const QString &keySequence);
    static QString getKeySequence();

    /*
    static QString getRecordingDirectoryFfmpeg();
    static void setRecordingDirectory(const QString &key, const QString &directory);

	static QString getRecordingDuration();
	static void setRecordingDuration(const QString &duration);

    static void saveDBParams(const QString &hostname, const QString &port, const QString &username, const QString &password);
    static void loadDBParams(QString &hostname, QString &port, QString &username, QString &password);

    static QString showFfmpegExe(QWidget *parent);
    static void setRecordingFfmpegExe(const QString &ffmpegExe);
    static QString getRecordingFfmpegExe();

    static QString showRecordingDirectoryFfmpeg(QWidget *parent);

    static QString showNginxExe(QWidget *parent);
    static void setNginxExe(const QString &nginxExe);
    static QString getNginxExe();

	static void setRecordingEnable(const QString &key, bool enable);
	static bool getRecordingEnable(const QString &key, bool defArg);

    static void setRecordingEnable(bool enable);
    static bool getRecordingEnable();

	static void setRecordingFfmpegEnable(bool enable);
	static bool getRecordingFfmpegEnable();

    static void setNginxEnable(bool enable);
    static bool getNginxEnable();

    static bool setExportDirectory(const QString &directory);
    static QString getExportDirectory();
    static QString showExportingDirectory(QWidget *parent);

	static bool setIP(const QString &ip);
	static QString getIP();
    

protected:
    static QString getParams(const QString &key, const QString &value);
    static QString setParams(const QString &key, const QString &value);
    */
}; 
