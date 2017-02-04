// <copyright file="Resources.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

wstring Resources::GetString(_In_ DWORD nID) const
{
    LPCWSTR wsz = NULL;
    auto ch = ::LoadStringW(m_hInstance, nID, (LPWSTR)&wsz, 0);
    if (!ch)
    {
        throw win32_error();
    }

    return wstring(wsz, wsz + ch);
}

wstring Resources::FormatString(_In_ DWORD nID, ...) const
{
    va_list args;

    va_start(args, nID);
    auto wsz = FormatString(nID, args);
    va_end(args);

    return wsz;
}

wstring Resources::FormatString(_In_ DWORD nID, _In_ va_list args) const
{
    auto fmt = GetString(nID);
    auto ch = _vscwprintf_p(fmt.c_str(), args);

    wstring wsz;
    wsz.resize(++ch);

    _vswprintf_p(&wsz[0], ch, fmt.c_str(), args);

    return wsz;
}
