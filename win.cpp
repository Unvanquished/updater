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

// ExecInExplorer.cpp
HRESULT ShellExecInExplorerProcess(PCWSTR pszFile, PCWSTR pszArgs);

namespace {
void setRegistryKey(const QString& key,
                    const QString& name,
                    const QString& value)
{
    QSettings registry(key, QSettings::NativeFormat);
    registry.setValue(name, value);
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

void SplitFirstArg(const std::wstring& commandLine, std::wstring* program, std::wstring* args)
{
    bool quoting = false;
    for (int i = 0; i < commandLine.size(); i++) {
        wchar_t c = commandLine.at(i);
        if (c == L'"') {
            quoting = !quoting;
        } else if (!quoting && (c == L' ' || c == L'\t')) {
            *program = commandLine.substr(0, i);
            *args = commandLine.substr(i + 1);
            return;
        }
    }
    *program = commandLine;
    *args = L"";
}

}  // namespace
namespace Sys {
bool IsWow64()
{
    BOOL bIsWow64 = FALSE;

    typedef BOOL (APIENTRY *LPFN_ISWOW64PROCESS)
    (HANDLE, PBOOL);

    LPFN_ISWOW64PROCESS fnIsWow64Process;

    HMODULE module = GetModuleHandle(L"kernel32");
    const char funcName[] = "IsWow64Process";
    fnIsWow64Process = (LPFN_ISWOW64PROCESS)
    GetProcAddress(module, funcName);

    if(NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),
            &bIsWow64))
            return false;
    }
    return bIsWow64 != FALSE;
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
    setRegistryKey("HKEY_CLASSES_ROOT\\unv", "Default", "URL: Unvanquished Protocol");
    setRegistryKey("HKEY_CLASSES_ROOT\\unv", "URL Protocol", "");
    setRegistryKey("HKEY_CLASSES_ROOT\\unv\\shell\\open\\command", "Default",
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

bool startGame(const QString& commandLine)
{
    if (Settings().testWrite() == QSettings::AccessError) {
        qDebug() << "not admin, start game normally";
        return QProcess::startDetached(commandLine);
    }
    std::wstring program, args;
    SplitFirstArg(commandLine.toStdWString(), &program, &args);
    qDebug() << "startGame de-elevated: program =" << program << "args =" << args;
    HRESULT result = ShellExecInExplorerProcess(program.c_str(), args.c_str());
    qDebug() << "startGame HRESULT:" << result;
    // It returns 1 "S_FALSE" (which is considered a success by SUCCEEDED) if the application
    // failed to start because it was given a nonexistent path. But returning success here seems
    // good anyway because Explorer creates its own dialog
    // box about the failure, and we don't want to pop two dialogs.
    // Strangely, ShellExecInExplorerProcess blocks until the user closes Explorer's message box in that case.
    return SUCCEEDED(result);
}

// Care should be taken when using this function to avoid any possibility of an endless restart loop.
// RelaunchElevated is skipped when --update-updater-to or --update-game is used in order to avoid this.
ElevationResult RelaunchElevated(const QString& flags)
{
    if (Settings().testWrite() == QSettings::NoError) {
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
