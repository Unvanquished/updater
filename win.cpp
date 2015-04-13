#include <windows.h>
#include "system.h"

namespace Sys {
bool IsWow64()
{
    BOOL bIsWow64 = FALSE;

    typedef BOOL (APIENTRY *LPFN_ISWOW64PROCESS)
    (HANDLE, PBOOL);

    LPFN_ISWOW64PROCESS fnIsWow64Process;

    HMODULE module = GetModuleHandle(_T("kernel32"));
    const char funcName[] = "IsWow64Process";
    fnIsWow64Process = (LPFN_ISWOW64PROCESS)
    GetProcAddress(module, funcName);

    if(NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),
            &bIsWow64))
            throw std::exception("Unknown error");
    }
    return bIsWow64 != FALSE;
}

QString getArchiveName(void)
{
    if (IsWow64()) {
        return "win64.zip";
    } else {
        return "win32.zip";
    }
}

QString getDefaultInstallPath(void)
{
    return "%%APPDATA%%\Unvanquished";
}
}
