// <copyright file="RegistryKey.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

RegistryKey::RegistryKey(_In_ HKEY hk, _In_ LPCWSTR wszKey, _In_ REGSAM samDesired)
{
    auto err = ::RegOpenKeyExW(hk, wszKey, 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &m_hk);
    if (err)
    {
        throw win32_error(err);
    }
}

RegistryKey::~RegistryKey()
{
    if (m_hk)
    {
        ::CloseHandle(m_hk);
    }
}

wstring RegistryKey::GetString(_In_opt_ LPCWSTR wszValue) const
{
    DWORD cch = 0;
    DWORD dwType = 0;

    auto err = ::RegQueryValueExW(m_hk, wszValue, NULL, &dwType, NULL, &cch);
    if (err)
    {
        throw win32_error(err);
    }

    if (REG_SZ != dwType)
    {
        throw win32_error(ERROR_INVALID_DATATYPE);
    }

    wstring value;
    value.resize((cch - sizeof(wchar_t)) / sizeof(wchar_t));

    err = ::RegQueryValueExW(m_hk, wszValue, NULL, NULL, (LPBYTE)&value[0], &cch);
    if (err)
    {
        throw win32_error(err);
    }

    return value;
}
