// <copyright file="Resources.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

struct ResourcesTraits
{
    static int __cdecl ResourcesGetString(__in_opt HINSTANCE hInstance, __in UINT nID, __out_ecount_part(cchBufferMax, return +1) LPWSTR lpBuffer, __in int cchBufferMax)
    {
        return ::LoadStringW(hInstance, nID, lpBuffer, cchBufferMax);
    }
};

template <class _Traits = ResourcesTraits>
class Resources
{
public:
    Resources(_In_ HINSTANCE hInstance) noexcept :
        m_hInstance(hInstance)
    {
    }

    Resources(_In_ const Resources& obj) noexcept :
        m_hInstance(obj.m_hInstance)
    {
    }

    std::wstring GetString(_In_ DWORD nID) const
    {
        LPCWSTR wsz = NULL;
        auto ch = _Traits::ResourcesGetString(m_hInstance, nID, (LPWSTR)&wsz, 0);
        if (!ch)
        {
            throw win32_error();
        }

        return std::wstring(wsz, wsz + ch);
    }

    std::wstring FormatString(_In_ DWORD nID, ...) const
    {
        va_list args;

        va_start(args, nID);
        auto wsz = FormatString(nID, args);
        va_end(args);

        return wsz;
    }

    std::wstring FormatString(_In_ DWORD nID, _In_ va_list args) const
    {
        auto fmt = GetString(nID);

        ::_set_invalid_parameter_handler(OnInvalidParameter);
        auto ch = _vscwprintf_p(fmt.c_str(), args);

        if (0 > ch)
        {
            throw win32_error(ERROR_INVALID_PARAMETER);
        }

        std::wstring wsz;
        wsz.resize(++ch);

        if (0 > _vswprintf_p(&wsz[0], ch, fmt.c_str(), args))
        {
            throw win32_error(ERROR_INVALID_PARAMETER);
        }

        return wsz;
    }

private:
    static void __cdecl OnInvalidParameter(
        _In_ const wchar_t* expression,
        _In_ const wchar_t* function,
        _In_ const wchar_t* file,
        _In_ unsigned int line,
        _In_ uintptr_t pReserved
    )
    {
        throw win32_error(ERROR_INVALID_PARAMETER);
    }

    HINSTANCE m_hInstance;
};
