#include "Settings.h"
#include "./ui_Settings.h"
#include "SettingsHelper.h"
#include <QDateTime>
#include <QMetaEnum>
#include "AuthorizationHelper.h"

Settings::Settings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Settings)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

	connect(ui->checkBoxAutostart, &QCheckBox::checkStateChanged, this, &Settings::signalAutostart);
    connect(ui->checkBoxAutorunApp, &QCheckBox::checkStateChanged, this, &Settings::signalAutorunApp);
	connect(ui->toolButtonBrowse,  &QAbstractButton::clicked, this, &Settings::signalShowBrowseDialog);
    connect(ui->keySequenceEdit, &QKeySequenceEdit::keySequenceChanged, this, &Settings::signalKeySequenceChanged);
    //connect(ui->comboBoxDisplayTo, &QComboBox::currentTextChanged, this, &Settings::comboBoxModeToChanged);
	
    QMetaEnum metaEnum = QMetaEnum::fromType<Settings::ModeTo>();

    for (int i = 0; i < metaEnum.keyCount(); i++)
	{
        Settings::ModeTo mode = (Settings::ModeTo)metaEnum.value(i);
        const auto modeStr = QMetaEnum::fromType<Settings::ModeTo>().valueToKey(mode);
        ui->comboBoxDisplayDefault->addItem(modeStr, mode);
        ui->comboBoxDisplayTo->addItem(modeStr, mode);
	}

    loadSettings();
    setAutostartIcon();
}

Settings::~Settings()
{
    saveSettings();
    delete ui;
}

void Settings::setAutostart(bool autostart) {
    ui->checkBoxAutostart->setChecked(autostart);
}

void Settings::setRunApplicationPath(const QString &path) {
    if(!path.isEmpty()) {
        ui->lineEditApplicationPath->setText(path);
    }
}

void Settings::setAutorunApplication(bool autorun) {
    ui->checkBoxAutorunApp->setChecked(autorun);
}

void Settings::setKeySequence(const QString &keySequence) {
    ui->keySequenceEdit->setKeySequence(QKeySequence(keySequence));
}

void Settings::addToLog(const QString &log)
{
    QDateTime dt = QDateTime::currentDateTime();
    QString text = QString("%1 - %2\n").arg(dt.toString()).arg(log);
    ui->plainTextEditLog->moveCursor(QTextCursor::Start);
    ui->plainTextEditLog->insertPlainText(text);
}

void Settings::saveSettings()
{
    SettingsHelper settings;

    auto value = ui->comboBoxDisplayDefault->currentText();
	settings.setModeDefault(value);

    value = ui->comboBoxDisplayTo->currentText();
	settings.setModeTo(value);

	value = ui->comboBoxAudioDefault->currentText();
	settings.setAudioDefault(value);

	value = ui->comboBoxAudioTo->currentText();
	settings.setAudioTo(value);

    auto watchNewAudio = ui->checkBoxWatchNewAudio->isChecked();
    settings.setWatchNewAudio(watchNewAudio);

    settings.setWindowState(saveGeometry());
}

void Settings::loadSettings()
{
	SettingsHelper settings;

	auto value = settings.getModeDefault();
	ui->comboBoxDisplayDefault->setCurrentText(value);

    value = settings.getModeTo();
	ui->comboBoxDisplayTo->setCurrentText(value);

	//value = settings.getAudioDefault();
	//ui->comboBoxAudioDefault->setCurrentText(value);

	//value = settings.getAudioTo();
	//ui->comboBoxAudioTo->setCurrentText(value);

    auto watchNewAudio = settings.getWatchNewAudio();
    ui->checkBoxWatchNewAudio->setChecked(watchNewAudio);

    restoreGeometry(settings.getWindowState());
}

void Settings::comboBoxModeToChanged(const QString &itemText)
{
}

void Settings::updateAudioDevices(QMap<QString, QVariantMap> devices)
{
    for (auto it=devices.begin(); it != devices.end(); it++)
    {
		const QString& name = it.key();
		const QString& id = it.value()["id"].toString();
		const bool connected = it.value()["connected"].toBool();

        qDebug() << name << connected << id;
        if(connected) ui->comboBoxAudioDefault->addItem(name, id);
		ui->comboBoxAudioTo->addItem(name, id);
    }

    SettingsHelper settings;

	auto value = settings.getAudioDefault();
	ui->comboBoxAudioDefault->setCurrentText(value);

	value = settings.getAudioTo();
	ui->comboBoxAudioTo->setCurrentText(value);
}

bool Settings::watchNewAudio() const
{
    auto watchNewAudio = ui->checkBoxWatchNewAudio->isChecked();
    return watchNewAudio;
}

QString Settings::getAudioDefaultId() const
{
    QString id = ui->comboBoxAudioDefault->currentData().toString();
    return id;
}

QString Settings::getAudioToId() const
{
	QString id = ui->comboBoxAudioTo->currentData().toString();
	return id;
}

void Settings::setAutostartIcon()
{
	QCheckBox* box = ui->checkBoxAutostart;

	if (!AuthorizationHelper::hasAdminRights() && box)
	{
		QIcon icon = QApplication::style()->standardIcon(QStyle::SP_VistaShield);
        box->setIcon(icon);
	}
}