#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QDir>
#include <QErrorMessage>

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
    ui->commandLine->setText(settings.value("settings/commandLineParameters").toString());
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::saveSettings(void)
{
    QDir dir(ui->installPath->text());
    if (!dir.exists()) {
        if (!dir.mkpath(dir.path())) {
            QErrorMessage errorMessage;
            errorMessage.showMessage(dir.canonicalPath() + " does not exist and could not be created");
            errorMessage.exec();
            return;
        }
    }
    QString commandLineStr = ui->commandLine->text();
    if (!commandLineStr.contains("%%command%%")) {
        QErrorMessage errorMessage;
        errorMessage.showMessage("The command line args MUST contain the %%command%% string");
        errorMessage.exec();
        return;
    }
    settings.setValue("settings/commandLineParameters", commandLineStr);
    settings.setValue("settings/installPath", dir.canonicalPath());
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
