#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QSettings>
#include <QFileDialog>

namespace Ui {
    class Dialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

private slots:
    void saveSettings(void);

private:
    Ui::Dialog *ui;
    QSettings settings;
};

#endif
