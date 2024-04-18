/* Unvanquished Updater
 * Copyright (C) Unvanquished Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#include "initguid.h"
#include "shlobj.h"
#include "windows.h"
#include "winnls.h"
#include "shobjidl.h"
#include "objbase.h"
#include "objidl.h"
#include "shlguid.h"

#include "system.h"
#include "settings.h"
#include "quazip/quazip/JlCompress.h"
#include <QSettings>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QProcess>
#include <QMessageBox>

// ExecInExplorer.cpp
HRESULT ShellExecInExplorerProcess(PCWSTR pszFile, PCWSTR pszArgs);

namespace {

// Returns true if it is a nonempty string (REG_SZ)
bool registryKeyExists(HKEY root,
                       const QString& key,
                       const QString& name)
{
    DWORD sizeInBytes = 999999;
    LSTATUS error = RegGetValueW(
        root,
        key.toStdWString().c_str(),
        name.toStdWString().c_str(),
        RRF_RT_REG_SZ,
        nullptr, //(out) variant type
        nullptr, //output buffer
        &sizeInBytes);
    if (error != ERROR_SUCCESS) {
        if (error != ERROR_FILE_NOT_FOUND) {
            qDebug() << "Failed to check registry key" << key << "code" << error;
        }
        return false;
    }
    return sizeInBytes > sizeof(wchar_t);
}

void setRegistryKey(HKEY root,
                    const QString& key,
                    const QString& name,
                    const QString& value)
{
    std::wstring valueW = value.toStdWString();
    LSTATUS error = RegSetKeyValueW(
        root,
        key.toStdWString().c_str(),
        name.isEmpty() ? nullptr : name.toStdWString().c_str(),
        REG_SZ,
        valueW.c_str(),
        sizeof(wchar_t) * (valueW.size() + 1));
    if (error != ERROR_SUCCESS) {
        qDebug() << "Failed to set registry key" << key << "code" << error;
    }
}

bool runningAsAdmin() {
    switch (Settings().testWrite()) {
    case QSettings::NoError:
        return true;
    case QSettings::AccessError:
        return false;
    case QSettings::FormatError:
        break;
    }

    QMessageBox errorMessageBox;
    errorMessageBox.setText("Windows registry not working");
    errorMessageBox.exec();
    exit(1);
}

// Create a Shortcut. Code from https://msdn.microsoft.com/en-us/library/bb776891(VS.85).aspx
bool CreateLink(const QString& sourcePath, const QString& workingDir,
                const QString& linkPath, QString& linkName)
{
    qDebug() << "Shortcut location:" << linkPath;
    qDebug() << "Shortcut target:" << sourcePath;

    HRESULT hres;
    IShellLink* psl;


    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called.
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hres))
    {
        IPersistFile* ppf;

        // Set the path to the shortcut target and add the description.
        psl->SetPath(sourcePath.toStdWString().c_str());
        psl->SetWorkingDirectory(workingDir.toStdWString().c_str());
        psl->SetDescription(linkName.toStdWString().c_str());

        // Query IShellLink for the IPersistFile interface, used for saving the
        // shortcut in persistent storage.
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

        if (SUCCEEDED(hres))
        {
            // Save the link by calling IPersistFile::Save.
            hres = ppf->Save(linkPath.toStdWString().c_str(), TRUE);
            ppf->Release();
        }
        psl->Release();
    }
    return SUCCEEDED(hres);
}

bool GetStartMenuPath(const QString& installPath, QString* startPath) {
    auto startMenuTye = FOLDERID_Programs;
    if (installPath.contains("Program Files", Qt::CaseInsensitive)) {
        startMenuTye = FOLDERID_CommonPrograms;
    }
    PWSTR path = nullptr;
    HRESULT ret = SHGetKnownFolderPath(startMenuTye, 0, nullptr, &path);
    if (SUCCEEDED(ret)) {
        *startPath = QString::fromStdWString(std::wstring(path));
    } else {
        qDebug() << "SHGetKnownFolderPath failed with hresult" << ret;
    }
    CoTaskMemFree(path);
    return SUCCEEDED(ret);
}

}  // namespace
namespace Sys {
bool IsWow64()
{
    BOOL b;
    if (!IsWow64Process(GetCurrentProcess(), &b)) {
        qDebug() << "IsWow64Process broken";
    }
    return b;
}

QString archiveName()
{
    if (IsWow64()) {
        return "windows-amd64.zip";
    } else {
        return "windows-i686.zip";
    }
}

QString defaultInstallPath()
{
    static const char* PROGRAM_FILES_VAR = "programfiles";
    static const char* PROGRAM_FILES_WOW64_VAR = "PROGRAMW6432";
    QString installPath = qgetenv(IsWow64() ? PROGRAM_FILES_WOW64_VAR
                                            : PROGRAM_FILES_VAR);
    return installPath + "\\Unvanquished";
}

bool validateInstallPath(const QString&)
{
    // Running as administrator does not have file ownership implications like on Unix.
    return true;
}

bool installShortcuts()
{
    Settings settings;
    QString installPath = settings.installPath();
    qDebug() << "Install path:" << installPath;

    // Create unv:// protocol handler
    QString quotedInstallPath = '"' + installPath + '"';
    QString quotedExecutablePath = '"' + installPath + "\\daemon.exe\"";
    setRegistryKey(HKEY_CLASSES_ROOT, "unv", "", "URL: Unvanquished Protocol");
    setRegistryKey(HKEY_CLASSES_ROOT, "unv", "URL Protocol", "");
    setRegistryKey(HKEY_CLASSES_ROOT, "unv\\shell\\open\\command", "",
                   quotedExecutablePath + " -connect \"%1\"");

    // Create a start menu shortcut
    // By default, install it to the users's start menu, unless they are installing
    // the game globally.
    QString startPath;
    if (!GetStartMenuPath(installPath, &startPath)) {
        return false;
    }
    QDir dir(startPath);
    dir.mkdir("Unvanquished");
    dir.setPath(dir.path() + "\\Unvanquished");
    QString linkName = "Unvanquished";
    if (!CreateLink(installPath + "\\updater.exe", installPath, dir.path() + "\\Unvanquished.lnk", linkName)) {
        qDebug() << "Creating shortcut failed";
        return false;
    }
    return true;
}

bool installUpdater(const QString& installPath) {
    QFileInfo src(QCoreApplication::applicationFilePath());
    QFileInfo dest(installPath + QDir::separator() + "updater.exe");
    if (src == dest) {
        qDebug() << "Updater already in install location";
        return true;
    }
    if (dest.exists()) {
        qDebug() << "Deleting updater in install path";
        if (!QFile::remove(dest.filePath())) {
            return false;
        }
    }
    qDebug() << "Copying updater from" << src.absoluteFilePath();
    return QFile::copy(src.absoluteFilePath(), dest.filePath());
}

bool updateUpdater(const QString& updaterArchive)
{
    QString current = QCoreApplication::applicationFilePath();
    QString backup = current + ".bak";
    QFile backupUpdater(backup);
    if (backupUpdater.exists()) {
        if (!backupUpdater.remove()) {
            qDebug() << "Could not remove backup updater. Aborting autoupdate.";
            return false;
        }
    }
    if (!QFile::rename(current, backup)) {
        qDebug() << "Could not move " << current << " to " << backup;
        return false;
    }
    QDir destination(current);
    if (!destination.cdUp()) {
        qDebug() << "Unexpected destination";
        return false;
    }
    // Only expect a single executable.
    auto out = JlCompress::extractDir(updaterArchive, destination.absolutePath());
    if (out.size() < 1) {
        qDebug() << "Error extracting update.";
        return false;
    }
    if (out.size() != 1 || !out[0].endsWith(".exe", Qt::CaseInsensitive)) {
        qDebug() << "Invalid update archive.";
        return false;
    }

    if (out[0] != current) {
        if (!QFile::rename(out[0], current)) {
            qDebug() << "Error renaming new updater to previous file name.";
            return false;
        }
    }

    if (!QProcess::startDetached(current, QStringList())) {
        qDebug() << "Error starting " << current;
        return false;
    }
    QCoreApplication::quit();
    return true;
}

QString updaterArchiveName()
{
    return "UnvUpdaterWin.zip";
}

std::string getCertStore()
{
    return "";  // Not used on Windows.
}

// Settings are stored in the registry at (on 64-bit Windows)
// HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Unvanquished Development\Unvanquished Updater
// ≤v0.0.5 used HKEY_CURRENT_USER\Software\Unvanquished Development\Unvanquished Updater
QSettings* makePersistentSettings(QObject* parent)
{
    return new QSettings(QSettings::SystemScope, parent);
}

QString getGameCommand(const QString& installPath)
{
    return QuoteQProcessCommandArgument(installPath + QDir::separator() + "daemon.exe");
}

// Equivalent to selecting "High performance" for the program in "Graphics settings"
// This must be run while non-elevated because the admin user may be a different user
static void setGraphicsPreference()
{
    const QString key = "Software\\Microsoft\\DirectX\\UserGpuPreferences";
    const QString path = Settings().installPath() + QDir::separator() + "daemon.exe";

    if (registryKeyExists(HKEY_CURRENT_USER, key, path)) {
        qDebug() << "graphics registry setting already exists";
        return;
    }

    qDebug() << "writing graphics preference to registry";
    setRegistryKey(HKEY_CURRENT_USER, key, path, "GpuPreference=2;");
}

QString startGame(const QString& commandLine, bool failIfWindowsAdmin)
{
    if (!runningAsAdmin()) {
        qDebug() << "not admin, start game normally";
        setGraphicsPreference();
        return QProcess::startDetached(commandLine) ? "" : "startDetached failed";
    }

    if (failIfWindowsAdmin) {
        // De-elevation already failed. Bail out to make sure a restart loop is never possible
        return "Please launch Unvanquished from a non-administrator context.";
    }

    // Relaunch the updater without elevation so that the graphics preference can be set.
    std::wstring program = QCoreApplication::applicationFilePath().toStdWString();
    std::wstring args = L"--playnow";
    qDebug() << "restarting de-elevated: program =" << program << "args =" << args;
    HRESULT result = ShellExecInExplorerProcess(program.c_str(), args.c_str());
    qDebug() << "HRESULT:" << result;
    // It returns 1 "S_FALSE" (which is considered a success by SUCCEEDED) if the application
    // failed to start because it was given a nonexistent path. But returning success here seems
    // good anyway because Explorer creates its own dialog
    // box about the failure, and we don't want to pop two dialogs.
    // Strangely, ShellExecInExplorerProcess blocks until the user closes Explorer's message box in that case.
    if (SUCCEEDED(result)) {
        return "";
    }
    return QString("error %1 while relaunching updater as non-admin").arg(result);
}

// Care should be taken when using this function to avoid any possibility of an endless restart loop.
// RelaunchElevated is skipped when --update-updater-to or --update-game is used in order to avoid this.
ElevationResult RelaunchElevated(const QString& flags)
{
    if (runningAsAdmin()) {
        qDebug() << "Process already has administrator privileges";
        return ElevationResult::UNNEEDED;
    }
    std::wstring updaterPath = QCoreApplication::applicationFilePath().toStdWString();
    std::wstring parameters = flags.toStdWString();
    SHELLEXECUTEINFOW info{};
    info.cbSize = sizeof(info);
    // "Applications that exit immediately after calling ShellExecuteEx should specify this flag"
    info.fMask = SEE_MASK_NOASYNC;
    info.lpVerb = L"runas";
    info.lpFile = updaterPath.c_str();
    info.lpParameters = parameters.c_str();
    info.nShow = SW_SHOW;
    if (ShellExecuteExW(&info)) {
        qDebug() << "Successfully relaunched as administrator";
        return ElevationResult::RELAUNCHED;
    } else {
        qDebug() << "Launch as administrator failed. SE_ERR_xxx code" << info.hInstApp;
        return ElevationResult::FAILED;
    }
}

}  // namespace Sys
