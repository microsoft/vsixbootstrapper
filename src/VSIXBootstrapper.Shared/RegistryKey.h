// <copyright file="RegistryKey.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

struct RegistryKeyTraits
{
    static LSTATUS __cdecl OpenKey(
        _In_ HKEY hKey,
        _In_opt_ LPCWSTR lpSubKey,
        _In_opt_ DWORD ulOptions,
        _In_ REGSAM samDesired,
        _Out_ PHKEY phkResult
    )
    {
        return ::RegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    }

    static LSTATUS __cdecl QueryValue(
        _In_ HKEY hKey,
        _In_opt_ LPCWSTR lpValueName,
        _Reserved_ LPDWORD lpReserved,
        _Out_opt_ LPDWORD lpType,
        _Out_opt_ LPBYTE lpData,
        _Inout_opt_ LPDWORD lpcbData
    )
    {
        return ::RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
    }

    static BOOL __cdecl Close(_In_ HANDLE hObject)
    {
        return ::CloseHandle(hObject);
    }
};

template <class _Traits = RegistryKeyTraits>
class RegistryKey
{
public:
    RegistryKey(_In_ HKEY hk, _In_ LPCWSTR wszKey, _In_ REGSAM samDesired = KEY_QUERY_VALUE | KEY_WOW64_32KEY)
    {
        auto err = _Traits::OpenKey(hk, wszKey, 0, samDesired, &m_hk);
        if (err)
        {
            throw win32_error(err);
        }
    }

    ~RegistryKey()
    {
        if (m_hk)
        {
            _Traits::Close(m_hk);
        }
    }

    std::wstring GetString(_In_opt_ LPCWSTR wszValue) const
    {
        DWORD cch = 0;
        DWORD dwType = 0;

        auto err = _Traits::QueryValue(m_hk, wszValue, NULL, &dwType, NULL, &cch);
        if (err)
        {
            throw win32_error(err);
        }

        if (REG_SZ != dwType)
        {
            throw win32_error(ERROR_INVALID_DATATYPE);
        }

        std::wstring value;
        value.resize((cch - sizeof(wchar_t)) / sizeof(wchar_t));

        err = _Traits::QueryValue(m_hk, wszValue, NULL, NULL, (LPBYTE)&value[0], &cch);
        if (err)
        {
            throw win32_error(err);
        }

        return value;
    }

private:
    RegistryKey(_In_ const RegistryKey&)
    {
    }

    HKEY m_hk;
};
