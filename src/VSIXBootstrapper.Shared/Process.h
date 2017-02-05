// <copyright file="Process.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

struct ProcessTraits
{
    static BOOL __cdecl Create(
        _In_opt_ LPCWSTR lpApplicationName,
        _Inout_opt_ LPWSTR lpCommandLine,
        _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
        _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
        _In_ BOOL bInheritHandles,
        _In_ DWORD dwCreationFlags,
        _In_opt_ LPVOID lpEnvironment,
        _In_opt_ LPCWSTR lpCurrentDirectory,
        _In_ LPSTARTUPINFOW lpStartupInfo,
        _Out_ LPPROCESS_INFORMATION lpProcessInformation
    )
    {
        return ::CreateProcessW(
            lpApplicationName,
            lpCommandLine,
            lpProcessAttributes,
            lpThreadAttributes,
            bInheritHandles,
            dwCreationFlags,
            lpEnvironment,
            lpCurrentDirectory,
            lpStartupInfo,
            lpProcessInformation);
    }

    static DWORD __cdecl Wait(_In_ HANDLE hHandle, _In_ DWORD dwMilliseconds)
    {
        return ::WaitForSingleObject(hHandle, dwMilliseconds);
    }

    static BOOL __cdecl GetExitCode(_In_ HANDLE hProcess, _Out_ LPDWORD lpExitCode)
    {
        return ::GetExitCodeProcess(hProcess, lpExitCode);
    }

    static BOOL __cdecl Close(_In_ HANDLE hObject)
    {
        return ::CloseHandle(hObject);
    }
};

template <class _Traits = ProcessTraits>
class Process
{
public:
    Process(_In_ int nCmdShow, _In_ LPCWSTR wszPath, _In_ LPCWSTR wszCommandLine) :
        m_hProcess(NULL),
        m_hThread(NULL)
    {
        STARTUPINFOW si = { sizeof(STARTUPINFOW) };
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = nCmdShow;

        PROCESS_INFORMATION pi = {};

        std::wstring commandLine = std::wstring(L"\"") + wszPath + L"\" " + wszCommandLine;
        if (!_Traits::Create(NULL, (LPWSTR)commandLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
        {
            throw win32_error();
        }

        m_hProcess = pi.hProcess;
        m_hThread = pi.hThread;
    }

    ~Process()
    {
        if (m_hProcess)
        {
            _Traits::Close(m_hProcess);
        }

        if (m_hThread)
        {
            _Traits::Close(m_hThread);
        }
    }

    void Wait() const
    {
        ThrowIfInvalidHandle();

        _Traits::Wait(m_hProcess, INFINITE);
    }

    DWORD GetExitCode() const
    {
        ThrowIfInvalidHandle();

        DWORD dwExitCode = 0;
        if (!_Traits::GetExitCode(m_hProcess, &dwExitCode))
        {
            throw win32_error();
        }

        return dwExitCode;
    }

private:
    void ThrowIfInvalidHandle() const
    {
        if (!m_hProcess)
        {
            throw win32_error(ERROR_INVALID_HANDLE);
        }
    }

    HANDLE m_hProcess;
    HANDLE m_hThread;
};
