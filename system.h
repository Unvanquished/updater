#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <string>

#include <QSettings>
#include <QString>

namespace Sys {
QString archiveName();
QString defaultInstallPath();
bool install();
bool updateUpdater(const QString& updaterArchive);
QString updaterArchiveName();
std::string getCertStore();
QSettings* makePersistentSettings(QObject* parent);
QString getGameCommand(const QString& installPath); // Substitution for %command%
bool startGame(const QString& commandLine);

inline QString QuoteQProcessCommandArgument(QString arg)
{
    arg.replace('"', "\"\"\"");
    return '"' + arg + '"';
}

}

#endif // SYSTEM_H_
