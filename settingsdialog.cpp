#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::DirectoryOnly);
    connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(openDirectorySelector()));
    connect(&fileDialog, SIGNAL(accepted()), this, SLOT(setInstallPath()));
    ui->installPath->setText(settings.value("settings/installPath").toString());
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::saveSettings(void)
{
    settings.setValue("settings/installPath", ui->installPath->text());
}

void SettingsDialog::openDirectorySelector(void)
{
    fileDialog.exec();
}

void SettingsDialog::setInstallPath(void)
{
    QStringList files = fileDialog.selectedFiles();
    if (!files.empty()) {
        ui->installPath->setText(files[0]);
    }
}


