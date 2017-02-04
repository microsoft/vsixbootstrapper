// <copyright file="CommandLineTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(CommandLineTests)
{
public:
    TEST_METHOD_INITIALIZE(Initialize)
    {
        s_count = 0;
    }

    TEST_METHOD(CommandLine_Optional)
    {
        struct TestTraits
        {
            static LPWSTR* __cdecl Parse(_In_ LPCWSTR lpCmdLine, _Out_ int *pNumArgs)
            {
                Assert::Fail(L"Unexpected");
                return NULL;
            };

            static HLOCAL __cdecl Free(_In_ HLOCAL hMem)
            {
                Assert::Fail(L"Unexpected");
                return NULL;
            };
        };

        CommandLine<TestTraits> args(NULL);
        Assert::IsFalse(args.IsQuiet());
    }

    TEST_METHOD(CommandLine_Parses_Slash_q)
    {
        {
            CommandLine<TestTraitsCounted> args(L"ignored /q ignored");
            Assert::IsTrue(args.IsQuiet());
        }

        Assert::AreEqual<byte>(2, s_count);
    }

    TEST_METHOD(CommandLine_Parses_Slash_Quiet)
    {
        {
            CommandLine<TestTraitsCounted> args(L"ignored /Quiet ignored");
            Assert::IsTrue(args.IsQuiet());
        }

        Assert::AreEqual<byte>(2, s_count);
    }

    TEST_METHOD(CommandLine_Parses_Dash_Q)
    {
        {
            CommandLine<TestTraitsCounted> args(L"ignored -Q ignored");
            Assert::IsTrue(args.IsQuiet());
        }

        Assert::AreEqual<byte>(2, s_count);
    }

    TEST_METHOD(CommandLine_Parses_Dash_quiet)
    {
        {
            CommandLine<TestTraitsCounted> args(L"ignored -quiet ignored");
            Assert::IsTrue(args.IsQuiet());
        }

        Assert::AreEqual<byte>(2, s_count);
    }

private:
    static byte s_count;

    struct TestTraitsCounted
    {
        static LPWSTR* __cdecl Parse(_In_ LPCWSTR lpCmdLine, _Out_ int *pNumArgs)
        {
            s_count++;
            return CommandLineTraits::Parse(lpCmdLine, pNumArgs);
        };

        static HLOCAL __cdecl Free(_In_ HLOCAL hMem)
        {
            s_count++;
            return CommandLineTraits::Free(hMem);
        };
    };
};

byte CommandLineTests::s_count = 0;
