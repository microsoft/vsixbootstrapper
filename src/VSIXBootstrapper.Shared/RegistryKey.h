// <copyright file="RegistryKey.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class RegistryKey
{
public:
    RegistryKey(_In_ HKEY hk, _In_ LPCWSTR wszKey, _In_ REGSAM samDesired);
    ~RegistryKey();

    std::wstring GetString(_In_opt_ LPCWSTR wszValue) const;

private:
    RegistryKey(_In_ const RegistryKey&)
    {
    }

    HKEY m_hk;
};
