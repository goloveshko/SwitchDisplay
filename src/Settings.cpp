#include "Settings.h"
#include "./ui_Settings.h"
#include "SettingsHelper.h"
#include <QDateTime>
#include <QMetaEnum>

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
    connect(ui->comboBoxModeTo, &QComboBox::currentTextChanged, this, &Settings::comboBoxModeToChanged);
	
    QMetaEnum metaEnum = QMetaEnum::fromType<Settings::ModeTo>();

    for (int i = 0; i < metaEnum.keyCount(); i++)
	{
        Settings::ModeTo mode = (Settings::ModeTo)metaEnum.value(i);
        const auto modeStr = QMetaEnum::fromType<Settings::ModeTo>().valueToKey(mode);
        ui->comboBoxModeTo->addItem(modeStr, mode);
	}

    loadSettings();
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

    auto itemText = ui->comboBoxModeTo->currentText();
	settings.setModeTo(itemText);

    settings.setWindowState(saveGeometry());
}

void Settings::loadSettings()
{
	SettingsHelper settings;

	auto modeStr = settings.getModeTo();
	ui->comboBoxModeTo->setCurrentText(modeStr);

    restoreGeometry(settings.getWindowState());
}

void Settings::comboBoxModeToChanged(const QString &itemText)
{
}
