// <copyright file="CommandLine.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

struct CommandLineTraits
{
    static LPWSTR* __cdecl Parse(_In_ LPCWSTR lpCmdLine, _Out_ int *pNumArgs)
    {
        return ::CommandLineToArgvW(lpCmdLine, pNumArgs);
    }

    static HLOCAL __cdecl Free(_In_ HLOCAL hMem)
    {
        return ::LocalFree(hMem);
    }
};

template <class _Traits = CommandLineTraits>
class CommandLine
{
public:
    CommandLine(_In_opt_ LPWSTR wszCommandLine) :
        m_rgwszArgs(NULL),
        m_fQuiet(false)
    {
        if (!wszCommandLine)
        {
            return;
        }

        int cArgs = 0;

        m_rgwszArgs = _Traits::Parse(wszCommandLine, &cArgs);
        if (!m_rgwszArgs)
        {
            throw win32_error();
        }

        std::vector<std::wstring> args(m_rgwszArgs, m_rgwszArgs + cArgs);
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

    CommandLine(_In_ const CommandLine& obj) :
        m_fQuiet(obj.m_fQuiet)
    {
        // Original instance will free memory.
    }

    ~CommandLine()
    {
        if (m_rgwszArgs)
        {
            _Traits::Free(m_rgwszArgs);
        }
    }

    bool IsQuiet() const
    {
        return m_fQuiet;
    }

private:
    LPWSTR* m_rgwszArgs;
    bool m_fQuiet;
};
