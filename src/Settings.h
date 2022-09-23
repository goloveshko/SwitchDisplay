#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Settings; }
QT_END_NAMESPACE

class Settings : public QDialog
{
    Q_OBJECT

public:
	enum ModeTo {
        Internal,
        External,
        Extend
	};
	Q_ENUM(ModeTo)

public:
    Settings(QWidget *parent = nullptr);
    ~Settings();

    void setAutostart(bool autostart);
    void setRunApplicationPath(const QString &path);
    void setAutorunApplication(bool autorun);
    void setKeySequence(const QString &keySequence);
    void addToLog(const QString& log);

protected:
    void saveSettings();
    void loadSettings();

protected slots:
    void comboBoxModeToChanged(const QString& itemText);

signals:
    void signalAutostart(int state);
    void signalAutorunApp(int state);
    void signalShowBrowseDialog();
    void signalKeySequenceChanged(const QKeySequence &keySequence);

private:
    Ui::Settings *ui;
};
#endif // SETTINGS_H
