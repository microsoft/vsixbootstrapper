// <copyright file="CoInitializerTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(CoInitializerTests)
{
public:
    TEST_METHOD(CoInitializer_Constructor_Throws)
    {
        struct TestTraits
        {
            static HRESULT __cdecl Initialize(_In_ LPVOID)
            {
                return E_FAIL;
            }

            static void __cdecl Uninitialize()
            {
                Assert::Fail(L"Unexpected");
            }
        };

        Assert::ExpectException<win32_error>([]() { CoInitializer<TestTraits>(); });
    }

    TEST_METHOD(CoInitializer_Unitializes)
    {
        static byte count = 0;
        struct TestTraits
        {
            static HRESULT __stdcall Initialize(_In_ LPVOID)
            {
                count++;
                return S_OK;
            }

            static void __stdcall Uninitialize()
            {
                count++;
            }
        };

        // CoInitializer must be in separate scope to test.
        {
            CoInitializer<TestTraits>();
        }

        Assert::AreEqual<byte>(2, count);
    }
};
