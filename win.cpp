#include <windows.h>
#include "system.h"
#include "settings.h"
#include <QSettings>


namespace {
void setRegistryKey(const QString& key,
                    const QString& name,
                    const QString& value)
{
    QSettings registry(key, QSettings::NativeFormat);
    registry.setValue(name, value);
}
}  // namespace
namespace Sys {
bool IsWow64()
{
    BOOL bIsWow64 = FALSE;

    typedef BOOL (APIENTRY *LPFN_ISWOW64PROCESS)
    (HANDLE, PBOOL);

    LPFN_ISWOW64PROCESS fnIsWow64Process;

    HMODULE module = GetModuleHandle("kernel32");
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

QString archiveName(void)
{
    if (IsWow64()) {
        return "win64.zip";
    } else {
        return "win32.zip";
    }
}

QString defaultInstallPath(void)
{
    static const char* PROGRAM_FILES_VAR = "programfiles";
    static const char* PROGRAM_FILES_WOW64_VAR = "PROGRAMW6432";
    QString installPath = qgetenv(IsWow64() ? PROGRAM_FILES_WOW64_VAR
                                            : PROGRAM_FILES_VAR);
    return installPath + "\\Unvanquished";
}

QString executableName(void)
{
    return "daemon.exe";
}

bool install(void)
{
    Settings settings;
    QString installPath = settings.installPath();
    setRegistryKey("HKEY_CLASSES_ROOT\\unv", "Default", "URL: Unvanquished Protocol");
    setRegistryKey("HKEY_CLASSES_ROOT\\unv\\DefaultIcon", "Default",
                   installPath + "\\daemon.exe,1");
    setRegistryKey("HKEY_CLASSES_ROOT\\unv", "URL Protocol", "");
    setRegistryKey("HKEY_CLASSES_ROOT\\unv\\shell\\open\\command", "Default",
                   installPath + "\\daemon.exe -pakpath " + installPath +
                        " +connect \"%1\"");
    return true;
}

}  // namespace Sys
