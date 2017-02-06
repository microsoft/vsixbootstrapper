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
            static LPWSTR* __cdecl CommandLineParse(_In_ LPCWSTR lpCmdLine, _Out_ int *pNumsut)
            {
                Assert::Fail(L"Unexpected");
                return NULL;
            };

            static HLOCAL __cdecl CommandLineFree(_In_ HLOCAL hMem)
            {
                Assert::Fail(L"Unexpected");
                return NULL;
            };
        };

        CommandLine<TestTraits> sut(NULL);
        Assert::IsFalse(sut.IsQuiet());
    }

    TEST_METHOD(CommandLine_Parses_Slash_q)
    {
        {
            CommandLine<TestTraitsCounted> sut(L"ignored /q ignored");
            Assert::IsTrue(sut.IsQuiet());
        }

        Assert::AreEqual<byte>(2, s_count);
    }

    TEST_METHOD(CommandLine_Parses_Slash_Quiet)
    {
        {
            CommandLine<TestTraitsCounted> sut(L"ignored /Quiet ignored");
            Assert::IsTrue(sut.IsQuiet());
        }

        Assert::AreEqual<byte>(2, s_count);
    }

    TEST_METHOD(CommandLine_Parses_Dash_Q)
    {
        {
            CommandLine<TestTraitsCounted> sut(L"ignored -Q ignored");
            Assert::IsTrue(sut.IsQuiet());
        }

        Assert::AreEqual<byte>(2, s_count);
    }

    TEST_METHOD(CommandLine_Parses_Dash_quiet)
    {
        {
            CommandLine<TestTraitsCounted> sut(L"ignored -quiet ignored");
            Assert::IsTrue(sut.IsQuiet());
        }

        Assert::AreEqual<byte>(2, s_count);
    }

private:
    static byte s_count;

    struct TestTraitsCounted
    {
        static LPWSTR* __cdecl CommandLineParse(_In_ LPCWSTR lpCmdLine, _Out_ int *pNumsut)
        {
            s_count++;
            return CommandLineTraits::CommandLineParse(lpCmdLine, pNumsut);
        };

        static HLOCAL __cdecl CommandLineFree(_In_ HLOCAL hMem)
        {
            s_count++;
            return CommandLineTraits::CommandLineFree(hMem);
        };
    };
};

byte CommandLineTests::s_count = 0;
