// <copyright file="CommandLine.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;

CommandLine::CommandLine(_In_ LPWSTR wszCommandLine) :
    m_rgwszArgs(NULL),
    m_fQuiet(false)
{
    if (!wszCommandLine)
    {
        return;
    }

    int cArgs = 0;

    m_rgwszArgs = ::CommandLineToArgvW(wszCommandLine, &cArgs);
    if (!m_rgwszArgs)
    {
        throw win32_error();
    }

    vector<wstring> args(m_rgwszArgs, m_rgwszArgs + cArgs);
    for (const auto& arg : args)
    {
        if (!arg.empty() && (L'-' == arg[0] || L'/' == arg[0]))
        {
            const auto name = &arg.c_str()[1];
            if (CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, L"q", -1, name, -1) ||
                CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, L"quiet", -1, name, -1))
            {
                m_fQuiet = true;
                break;
            }
        }
    }
}

CommandLine::~CommandLine()
{
    if (m_rgwszArgs)
    {
        ::LocalFree(m_rgwszArgs);
    }
}
