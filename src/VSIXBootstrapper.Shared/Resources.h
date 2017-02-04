// <copyright file="Resources.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

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

    std::wstring GetString(_In_ DWORD nID) const;
    std::wstring FormatString(_In_ DWORD nID, ...) const;
    std::wstring FormatString(_In_ DWORD nID, _In_ va_list args) const;

private:
    HINSTANCE m_hInstance;
};
