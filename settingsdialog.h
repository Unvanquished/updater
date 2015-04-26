#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QSettings>
#include <QFileDialog>

namespace Ui {
    class Dialog;
}

namespace Settings {
    const static QString INSTALL_PATH("settings/installPath");
    const static QString COMMAND_LINE("settings/commandLineParameters");
}

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

private slots:
    void saveSettings(void);
    void openDirectorySelector(void);
    void setInstallPath(void);

private:
    Ui::Dialog *ui;
    QSettings settings;
    QFileDialog fileDialog;
};

#endif
