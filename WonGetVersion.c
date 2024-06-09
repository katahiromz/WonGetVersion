#include <windows.h>
#include <shlwapi.h>
#include "WonGetVersion.h"

#ifndef WONGETVERSION_REGKEY
    #define WONGETVERSION_REGKEY L"SOFTWARE\\Katayama Hirofumi MZ\\vista2xp"
#endif

static DWORD
GetDwordValue(LPCWSTR pszName, DWORD dwDefaultValue)
{
    WCHAR szModule[MAX_PATH], szSubKey[MAX_PATH];
    HKEY hKey;
    LONG error;
    DWORD dwValue, cbValue;

    GetModuleFileNameW(NULL, szModule, _countof(szModule));

    lstrcpynW(szSubKey, WONGETVERSION_REGKEY, _countof(szSubKey));
    PathAppendW(szSubKey, PathFindFileNameW(szModule));

    error = RegOpenKeyExW(HKEY_CURRENT_USER, szSubKey, 0, KEY_READ, &hKey);
    if (error)
    {
        lstrcpynW(szSubKey, WONGETVERSION_REGKEY, _countof(szSubKey));
        error = RegOpenKeyExW(HKEY_CURRENT_USER, szSubKey, 0, KEY_READ, &hKey);
    }
    if (error)
        return dwDefaultValue;

    cbValue = sizeof(dwValue);
    dwValue = dwDefaultValue;
    error = RegQueryValueExW(hKey, pszName, NULL, NULL, (BYTE*)&dwValue, &cbValue);
    if (error)
    {
        RegCloseKey(hKey);
        return dwDefaultValue;
    }

    return dwValue;
}

static void
GetSzValue(LPCWSTR pszName, LPWSTR pszValue, DWORD cchValue, LPCWSTR pszDefValue)
{
    WCHAR szModule[MAX_PATH], szSubKey[MAX_PATH];
    HKEY hKey;
    LONG error;
    DWORD dwValue, cbValue;

    GetModuleFileNameW(NULL, szModule, _countof(szModule));

    lstrcpynW(szSubKey, WONGETVERSION_REGKEY, _countof(szSubKey));
    PathAppendW(szSubKey, PathFindFileNameW(szModule));

    error = RegOpenKeyExW(HKEY_CURRENT_USER, szSubKey, 0, KEY_READ, &hKey);
    if (error)
    {
        lstrcpynW(szSubKey, WONGETVERSION_REGKEY, _countof(szSubKey));
        error = RegOpenKeyExW(HKEY_CURRENT_USER, szSubKey, 0, KEY_READ, &hKey);
    }
    if (error)
    {
        lstrcpynW(pszValue, pszDefValue, cchValue);
        return;
    }

    cbValue = cchValue * sizeof(WCHAR);
    error = RegQueryValueExW(hKey, pszName, NULL, NULL, (BYTE*)pszValue, &cbValue);
    RegCloseKey(hKey);
    if (error)
        lstrcpynW(pszValue, pszDefValue, cchValue);
}

DWORD WINAPI WonGetVersion(void)
{
    return GetDwordValue(L"GetVersion", 0x23F00206);
}

DWORD WINAPI WonGetVersionExA(LPOSVERSIONINFOA osver)
{
    WCHAR szCSDVersion[64];

    if (!osver ||
        osver->dwOSVersionInfoSize != sizeof(OSVERSIONINFOA) ||
        osver->dwOSVersionInfoSize != sizeof(OSVERSIONINFOEXA))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    osver->dwMajorVersion = GetDwordValue(L"dwMajorVersion", 6);
    osver->dwMinorVersion = GetDwordValue(L"dwMinorVersion", 2);
    osver->dwBuildNumber = GetDwordValue(L"dwBuildNumber", 9200);
    osver->dwPlatformId = GetDwordValue(L"dwPlatformId", 2);

    GetSzValue(L"szCSDVersion", szCSDVersion, _countof(szCSDVersion), L"");
    WideCharToMultiByte(CP_ACP, 0, szCSDVersion, -1, osver->szCSDVersion,
                        _countof(osver->szCSDVersion), NULL, NULL);

    if (osver->dwOSVersionInfoSize == sizeof(OSVERSIONINFOEXA))
    {
        LPOSVERSIONINFOEXA osverx = (LPOSVERSIONINFOEXA)osver;
        osverx->wServicePackMajor = 0x0000;
        osverx->wServicePackMinor = 0x0000;
        osverx->wSuiteMask = 0x0300;
        osverx->wProductType = 1;
        osverx->wReserved = 0;
    }

    return TRUE;
}

DWORD WINAPI WonGetVersionExW(LPOSVERSIONINFOW osver)
{
    if (!osver ||
        osver->dwOSVersionInfoSize != sizeof(OSVERSIONINFOW) ||
        osver->dwOSVersionInfoSize != sizeof(OSVERSIONINFOEXW))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    osver->dwMajorVersion = GetDwordValue(L"dwMajorVersion", 6);
    osver->dwMinorVersion = GetDwordValue(L"dwMinorVersion", 2);
    osver->dwBuildNumber = GetDwordValue(L"dwBuildNumber", 9200);
    osver->dwPlatformId = GetDwordValue(L"dwPlatformId", 2);

    GetSzValue(L"szCSDVersion", osver->szCSDVersion, _countof(osver->szCSDVersion), L"");

    if (osver->dwOSVersionInfoSize == sizeof(OSVERSIONINFOEXA))
    {
        LPOSVERSIONINFOEXW osverx = (LPOSVERSIONINFOEXW)osver;
        osverx->wServicePackMajor = 0x0000;
        osverx->wServicePackMinor = 0x0000;
        osverx->wSuiteMask = 0x0300;
        osverx->wProductType = 1;
        osverx->wReserved = 0;
    }

    return TRUE;
}
