// <copyright file="CommandLine.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

class CommandLine
{
public:
    CommandLine(_In_ LPWSTR wszCommandLine);
    CommandLine(_In_ const CommandLine& obj) :
        m_fQuiet(obj.m_fQuiet)
    {
        // Original instance will free memory.
    }

    ~CommandLine();

    bool IsQuiet() const
    {
        return m_fQuiet;
    }

private:
    LPWSTR* m_rgwszArgs;
    bool m_fQuiet;
};
