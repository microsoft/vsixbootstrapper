// <copyright file="CoInitializer.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

struct CoInitializerTraits
{
    static HRESULT __cdecl CoInitialize(_In_opt_ LPVOID)
    {
        return ::CoInitialize(NULL);
    }

    static void __cdecl CoUninitialize()
    {
        return ::CoUninitialize();
    }
};

template <class _Traits = CoInitializerTraits>
class CoInitializer
{
public:
    CoInitializer()
    {
        auto hr = _Traits::CoInitialize(NULL);
        if (FAILED(hr))
        {
            throw win32_error(hr);
        }
    }

    ~CoInitializer()
    {
        _Traits::CoUninitialize();
    }

private:
    CoInitializer(const CoInitializer& obj)
    {
    }
};
