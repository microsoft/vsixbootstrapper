// <copyright file="ResourcesTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(ResourcesTests)
{
public:
    TEST_METHOD(Resources_Missing_ID_Throws)
    {
        struct TestTraits
        {
            #pragma warning(suppress: 6054) // Unexpected, so ignore that lpBuffer is not set
            static int ResourcesGetString(__in_opt HINSTANCE hInstance, __in UINT nID, __out_ecount_part(cchBufferMax, return +1) LPWSTR lpBuffer, __in int cchBufferMax)
            {
                return 0;
            };
        };

        Resources<TestTraits> sut(NULL);
        Assert::ExpectException<win32_error>([&]() { sut.GetString(0); });
    }

    TEST_METHOD(Resources_FormatString)
    {
        m_wsz = L"test: %1$hs = %2$d";

        Resources<TestStringTraits> sut(NULL);
        auto actual = sut.FormatString(0, "a", 1);

        Assert::AreEqual(L"test: a = 1", actual.c_str());
    }

    TEST_METHOD(Resources_FormatString_Throws)
    {
        m_wsz = L"test: %z";

        Resources<TestStringTraits> sut(NULL);
        Assert::ExpectException<win32_error>([&]() { sut.FormatString(0, "a"); });
    }

private:
    static LPWSTR m_wsz;

    struct TestStringTraits
    {
        #pragma warning(suppress: 6054) // Static analysis is failing to detect that m_wsz will be null-terminated
        static int ResourcesGetString(__in_opt HINSTANCE hInstance, __in UINT nID, __out_ecount_part(cchBufferMax, return +1) LPWSTR lpBuffer, __in int cchBufferMax)
        {
            ::memcpy(lpBuffer, &m_wsz, sizeof(LPWSTR));
            return wcslen(m_wsz);
        };
    };
};

LPWSTR ResourcesTests::m_wsz = L"test";