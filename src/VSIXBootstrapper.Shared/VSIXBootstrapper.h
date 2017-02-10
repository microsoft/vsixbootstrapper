// <copyright file="CoInitializer.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

using std::experimental::filesystem::v1::path;

// Same generic error code VSIXInstaller.exe returns.
#define GENERIC_ERROR 3001
static const LPCWSTR g_wszFileName = L"VSIXInstaller.exe";

struct ErrorTraits
{
    static int __cdecl ErrorShowMessage(
        _In_opt_ LPCWSTR lpText,
        _In_opt_ LPCWSTR lpCaption,
        _In_ UINT uType)
    {
        return ::MessageBox(NULL, lpText, lpCaption, uType);
    }
};

struct IOTraits
{
    static bool __cdecl IOFileExists(_In_ LPCWSTR wszPath)
    {
        DWORD dwAttrib = ::GetFileAttributesW(wszPath);

        return dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
    }
};

template <class... Mixins>
struct Traits : public Mixins...
{
    typedef Traits<CoInitializerTraits, CommandLineTraits, ErrorTraits, IOTraits, ProcessTraits, RegistryKeyTraits, ResourcesTraits> Default;
};

template <class _Traits>
void ShowError(_In_ Resources<_Traits>& resources, _In_ bool quiet, _In_ DWORD nID, ...);

template <class _Traits>
std::wstring FromConfiguration() noexcept;

template <class _Traits>
std::wstring FromRegistry(_In_ LPCWSTR wszVersion) noexcept;

template <class _Traits = Traits<>::Default>
int Run(
    _In_ HINSTANCE hInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    using namespace std;

    Resources<_Traits> resources(hInstance);
    auto quiet = true;

    try
    {
        // Determine whether we should ever show UI.
        CommandLine<_Traits> args(lpCmdLine);
        quiet = args.IsQuiet() || SW_HIDE == nCmdShow;

        vector<function<wstring()>> lookups
        {
            FromConfiguration<_Traits>,
            bind(FromRegistry<_Traits>, L"14.0"),
            bind(FromRegistry<_Traits>, L"12.0"),
            bind(FromRegistry<_Traits>, L"11.0"),
            bind(FromRegistry<_Traits>, L"10.0"),
        };

        for (const auto& lookup : lookups)
        {
            path path = lookup();
            if (!path.empty() && _Traits::IOFileExists(path.append(g_wszFileName).c_str()))
            {
                Process<_Traits> p(nCmdShow, path.c_str(), lpCmdLine);
                p.Wait();

                return p.GetExitCode();
            }
        }

        // We didn't find it so return an error.
        ShowError<_Traits>(resources, quiet, IDS_NOTFOUND);
        return ERROR_FILE_NOT_FOUND;
    }
    catch (system_error& ex)
    {
        const auto code = ex.code().value();

        ShowError<_Traits>(resources, quiet, IDS_SYSTEM_ERROR, code, ex.what());
        return code;
    }
    catch (exception& ex)
    {
        ShowError<_Traits>(resources, quiet, IDS_ERROR, ex.what());
        return GENERIC_ERROR;
    }

    return GENERIC_ERROR;
}

template <class _Traits>
void ShowError(_In_ Resources<_Traits>& resources, _In_ bool quiet, _In_ DWORD nID, ...)
{
    if (!quiet)
    {
        va_list args;

        va_start(args, nID);
        const auto caption = resources.GetString(IDS_CAPTION);
        const auto message = resources.FormatString(nID, args);
        va_end(args);

        _Traits::ErrorShowMessage(message.c_str(), caption.c_str(), MB_OK | MB_ICONERROR);
    }
}

template <class _Traits>
std::wstring FromConfiguration() noexcept
{
    try
    {
        CoInitializer<_Traits> init;

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
                if (!path.empty() && _Traits::IOFileExists(path.append(g_wszFileName).c_str()))
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

    return std::wstring();
}

template <class _Traits>
std::wstring FromRegistry(_In_ LPCWSTR wszVersion) noexcept
{
    try
    {
        WCHAR wzPath[MAX_PATH] = {};
        if (0 > swprintf_s(wzPath, _countof(wzPath), L"SOFTWARE\\Microsoft\\VisualStudio\\%ls\\Setup\\VS", wszVersion))
        {
            throw win32_error(E_UNEXPECTED);
        }

        RegistryKey<_Traits> key(HKEY_LOCAL_MACHINE, wzPath);
        return key.GetString(L"EnvironmentDirectory");
    }
    catch (...)
    {
        // TODO: Consider tracing errors.
    }

    return std::wstring();
}
