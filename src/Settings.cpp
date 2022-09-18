#include "Settings.h"
#include "./ui_Settings.h"
#include <QSettings>

Settings::Settings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Settings)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

	connect(ui->checkBoxAutostart, &QCheckBox::stateChanged, this, &Settings::signalAutostart);
    connect(ui->checkBoxAutorunApp, &QCheckBox::stateChanged, this, &Settings::signalAutorunApp);
	connect(ui->toolButtonBrowse,  &QAbstractButton::clicked, this, &Settings::signalShowBrowseDialog);
    connect(ui->keySequenceEdit, &QKeySequenceEdit::keySequenceChanged, this, &Settings::signalKeySequenceChanged);

    readWindowState();
}

Settings::~Settings()
{
    writeWindowState();
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

void Settings::writeWindowState()
{
	QSettings settings;
	settings.setValue("Geometry", saveGeometry());
}

void Settings::readWindowState()
{
	QSettings settings;
	restoreGeometry(settings.value("Geometry").toByteArray());
}
