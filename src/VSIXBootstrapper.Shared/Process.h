// <copyright file="Process.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class Process
{
public:
    Process(_In_ int nCmdShow, _In_ LPCWSTR wszPath, _In_ LPCWSTR wszCommandLine);
    ~Process();

    void Wait() const;
    DWORD GetExitCode() const;

private:
    void ThrowIfInvalidHandle() const;

    HANDLE m_hProcess;
    HANDLE m_hThread;
};
