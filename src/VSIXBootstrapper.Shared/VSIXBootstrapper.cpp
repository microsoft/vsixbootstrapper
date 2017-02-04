// <copyright file="Program.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace std::experimental::filesystem::v1;

// Same generic error code VSIXInstaller.exe returns.
#define GENERIC_ERROR 3001
static const LPCWSTR g_wszFileName = L"VSIXInstaller.exe";

struct RegDeleter
{
    void operator()(HKEY* hk)
    {
        if (hk && *hk)
        {
            ::CloseHandle(*hk);
        }
    }
};

int Error(_In_ Resources& resources, _In_ bool quiet, _In_ DWORD nError, _In_ DWORD nID, ...);
wstring FromConfiguration() noexcept;
wstring FromRegistry(_In_ LPCWSTR wszVersion) noexcept;

int Run(
    _In_ HINSTANCE hInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    Resources resources(hInstance);
    auto quiet = true;

    try
    {
        // Determine whether we should ever show UI.
        CommandLine<> args(lpCmdLine);
        quiet = args.IsQuiet() || SW_HIDE == nCmdShow;

        vector<function<wstring()>> lookups
        {
            FromConfiguration,
            bind(FromRegistry, L"14.0"),
            bind(FromRegistry, L"12.0"),
            bind(FromRegistry, L"11.0"),
            bind(FromRegistry, L"10.0"),
        };

        for (const auto& lookup : lookups)
        {
            path path = lookup();
            if (!path.empty() && exists(path.append(g_wszFileName)))
            {
                Process p(nCmdShow, path.c_str(), lpCmdLine);
                p.Wait();

                return p.GetExitCode();
            }
        }

        // We didn't find it so return an error.
        return Error(resources, quiet, ERROR_FILE_NOT_FOUND, IDS_NOTFOUND);
    }
    catch (system_error& ex)
    {
        const auto code = ex.code().value();
        return Error(resources, quiet, code, IDS_SYSTEM_ERROR, code, ex.what());
    }
    catch (exception& ex)
    {
        return Error(resources, quiet, GENERIC_READ, IDS_ERROR, ex.what());
    }

    return GENERIC_ERROR;
}

int Error(_In_ Resources& resources, _In_ bool quiet, _In_ DWORD nError, _In_ DWORD nID, ...)
{
    if (!quiet)
    {
        va_list args;

        va_start(args, nID);
        const auto caption = resources.GetString(IDS_CAPTION);
        const auto message = resources.FormatString(nID, args);
        va_end(args);

        ::MessageBoxW(NULL, message.c_str(), caption.c_str(), MB_OK | MB_ICONERROR);
    }

    return nError;
}

wstring FromConfiguration() noexcept
{
    try
    {
        CoInitializer<> init;

        ISetupConfiguration2Ptr query;
        IEnumSetupInstancesPtr e;
        ISetupInstance* pInstances[1];

        auto hr = query.CreateInstance(__uuidof(SetupConfiguration));
        if (FAILED(hr))
        {
            throw win32_error(hr);
        }

        hr = query->EnumAllInstances(&e);
        if (FAILED(hr))
        {
            throw win32_error(hr);
        }

        ULONG fetched = 0;
        hr = e->Next(1, pInstances, &fetched);
        while (SUCCEEDED(hr) && 1 == fetched)
        {
            auto* pInstance = pInstances[0];
            ISetupInstance2Ptr instance(pInstance);

            pInstance->Release();

            bstr_t enginePath;
            hr = instance->GetEnginePath(enginePath.GetAddress());
            if (SUCCEEDED(hr))
            {
                path path = (LPWSTR)enginePath;
                if (!path.empty() && exists(path.append(g_wszFileName)))
                {
                    return (LPWSTR)enginePath;
                }
            }

            hr = e->Next(1, pInstances, &fetched);
        }
    }
    catch (...)
    {
        // TODO: Consider tracing errors.
    }

    return wstring();
}

wstring FromRegistry(_In_ LPCWSTR wszVersion) noexcept
{
    try
    {
        WCHAR wzPath[MAX_PATH] = {};
        if (0 > swprintf_s(wzPath, _countof(wzPath), L"SOFTWARE\\Microsoft\\VisualStudio\\%ls\\Setup\\VS", wszVersion))
        {
            throw win32_error(E_UNEXPECTED);
        }

        RegistryKey key(HKEY_LOCAL_MACHINE, wzPath, KEY_QUERY_VALUE | KEY_WOW64_32KEY);
        return key.GetString(L"EnvironmentDirectory");
    }
    catch (...)
    {
        // TODO: Consider tracing errors.
    }

    return wstring();
}
