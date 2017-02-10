// <copyright file="ExceptionsTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(ExceptionsTests)
{
public:
    TEST_METHOD(win32_error_GetLastError)
    {
        ::SetLastError(E_FAIL);

        win32_error sut;
        Assert::AreEqual<int>(E_FAIL, sut.code().value());
    }

    TEST_METHOD(win32_error_custom)
    {
        ::SetLastError(ERROR_SUCCESS);

        win32_error sut(E_FAIL);
        Assert::AreEqual<int>(E_FAIL, sut.code().value());
    }
};
