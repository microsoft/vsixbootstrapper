// <copyright file="Process.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

Process::Process(_In_ int nCmdShow, _In_ LPCWSTR wszPath, _In_ LPCWSTR wszCommandLine) :
    m_hProcess(NULL),
    m_hThread(NULL)
{
    STARTUPINFOW si = { sizeof(STARTUPINFOW) };
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = nCmdShow;

    PROCESS_INFORMATION pi = {};

    wstring commandLine = wstring(L"\"") + wszPath + L"\"" + wszCommandLine;
    if (!::CreateProcessW(NULL, (LPWSTR)commandLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        throw win32_error();
    }

    m_hProcess = pi.hProcess;
    m_hThread = pi.hThread;
}

Process::~Process()
{
    ::CloseHandle(m_hProcess);
    ::CloseHandle(m_hThread);
}

void Process::Wait() const
{
    ThrowIfInvalidHandle();

    ::WaitForSingleObject(m_hProcess, INFINITE);
}

DWORD Process::GetExitCode() const
{
    ThrowIfInvalidHandle();

    DWORD dwExitCode = 0;
    if (!::GetExitCodeProcess(m_hProcess, &dwExitCode))
    {
        throw win32_error();
    }

    return dwExitCode;
}

void Process::ThrowIfInvalidHandle() const
{
    if (!m_hProcess)
    {
        throw win32_error(ERROR_INVALID_HANDLE);
    }
}
