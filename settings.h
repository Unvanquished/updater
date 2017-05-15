#ifndef SETTINGS_H
#define SETTINGS_H
#include <QObject>
#include <QSettings>
#include <QString>

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString installPath READ installPath WRITE setInstallPath NOTIFY installPathChanged)
    Q_PROPERTY(QString commandLine READ commandLine WRITE setCommandLine NOTIFY commandLineChanged)
    Q_PROPERTY(QString currentVersion READ currentVersion WRITE setCurrentVersion NOTIFY currentVersionChanged)
    Q_PROPERTY(bool installFinished READ installFinished WRITE setInstallFinished NOTIFY installFinishedChanged)
public:
    const QString INSTALL_PATH = "settings/installPath";
    const QString COMMAND_LINE = "settings/commandLineParameters";
    const QString CURRENT_VERSION = "settings/currentVersion";
    const QString INSTALL_FINISHED = "settings/installFinished";

    QString installPath() const;
    QString commandLine() const;
    QString currentVersion() const;
    bool installFinished() const;

    void setInstallPath(const QString& installPath);
    void setCommandLine(const QString& commandLine);
    void setCurrentVersion(const QString& currentVersion);
    void setInstallFinished(bool installFinished);

signals:
    void installPathChanged(QString installPath);
    void commandLineChanged(QString commandLine);
    void currentVersionChanged(QString currentVersion);
    void installFinishedChanged(bool installFinished);

private:
    QSettings settings_;
};


#endif // SETTINGS_H
