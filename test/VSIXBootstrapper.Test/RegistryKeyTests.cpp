// <copyright file="RegistryKeyTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(RegistryKeyTests)
{
public:
    TEST_METHOD(RegistryKey_Constructor_Throws)
    {
        struct TestTraits
        {
            static LSTATUS __cdecl RegistryKeyOpen(
                _In_ HKEY hKey,
                _In_opt_ LPCWSTR lpSubKey,
                _In_opt_ DWORD ulOptions,
                _In_ REGSAM samDesired,
                _Out_ PHKEY phkResult
            )
            {
                Assert::AreEqual(L"ShouldNotExist", lpSubKey);
                Assert::AreEqual<DWORD>(KEY_QUERY_VALUE | KEY_WOW64_32KEY, samDesired);

                *phkResult = NULL;

                return ERROR_FILE_NOT_FOUND;
            }

            static BOOL __cdecl RegistryKeyClose(_In_ HANDLE hObject)
            {
                Assert::Fail(L"Unexpected");

                return TRUE;
            }
        };

        Assert::ExpectException<win32_error>([]() { RegistryKey<TestTraits>(HKEY_LOCAL_MACHINE, L"ShouldNotExist"); });
    }

    TEST_METHOD(RegistryKey_Value_Missing_Throws)
    {
        struct TestTraits
        {
            static LSTATUS __cdecl RegistryKeyOpen(
                _In_ HKEY hKey,
                _In_opt_ LPCWSTR lpSubKey,
                _In_opt_ DWORD ulOptions,
                _In_ REGSAM samDesired,
                _Out_ PHKEY phkResult
            )
            {
                Assert::AreEqual(L"ShouldNotExist", lpSubKey);
                Assert::AreEqual<DWORD>(KEY_QUERY_VALUE | KEY_WOW64_32KEY, samDesired);

                *phkResult = (HKEY)1;

                return ERROR_SUCCESS;
            }

            static LSTATUS __cdecl RegistryKeyQueryValue(
                _In_ HKEY hKey,
                _In_opt_ LPCWSTR lpValueName,
                _Reserved_ LPDWORD lpReserved,
                _Out_opt_ LPDWORD lpType,
                _Out_opt_ LPBYTE lpData,
                _Inout_opt_ LPDWORD lpcbData
            )
            {
                Assert::AreEqual(1UL, (DWORD)hKey);
                Assert::AreEqual(L"Value", lpValueName);

                return ERROR_FILE_NOT_FOUND;
            }

            static BOOL __cdecl RegistryKeyClose(_In_ HANDLE hObject)
            {
                Assert::AreEqual(1UL, (DWORD)hObject);

                return TRUE;
            }
        };

        RegistryKey<TestTraits> sut(HKEY_LOCAL_MACHINE, L"ShouldNotExist");

        Assert::ExpectException<win32_error>([&]() { sut.GetString(L"Value"); });
    }

    TEST_METHOD(RegistryKey_Value_Wrong_Type_Throws)
    {
        struct TestTraits
        {
            static LSTATUS __cdecl RegistryKeyOpen(
                _In_ HKEY hKey,
                _In_opt_ LPCWSTR lpSubKey,
                _In_opt_ DWORD ulOptions,
                _In_ REGSAM samDesired,
                _Out_ PHKEY phkResult
            )
            {
                Assert::AreEqual(L"ShouldNotExist", lpSubKey);
                Assert::AreEqual<DWORD>(KEY_QUERY_VALUE | KEY_WOW64_32KEY, samDesired);

                *phkResult = (HKEY)1;

                return ERROR_SUCCESS;
            }

            static LSTATUS __cdecl RegistryKeyQueryValue(
                _In_ HKEY hKey,
                _In_opt_ LPCWSTR lpValueName,
                _Reserved_ LPDWORD lpReserved,
                _Out_opt_ LPDWORD lpType,
                _Out_opt_ LPBYTE lpData,
                _Inout_opt_ LPDWORD lpcbData
            )
            {
                Assert::AreEqual(1UL, (DWORD)hKey);
                Assert::AreEqual(L"Value", lpValueName);

                Assert::IsNotNull(lpType);
                *lpType = REG_DWORD;

                Assert::IsNotNull(lpcbData);
                *lpcbData = 0;

                return ERROR_SUCCESS;
            }

            static BOOL __cdecl RegistryKeyClose(_In_ HANDLE hObject)
            {
                Assert::AreEqual(1UL, (DWORD)hObject);

                return TRUE;
            }
        };

        RegistryKey<TestTraits> sut(HKEY_LOCAL_MACHINE, L"ShouldNotExist");

        Assert::ExpectException<win32_error>([&]() { sut.GetString(L"Value"); });
    }

    TEST_METHOD(RegistryKey_QueryString)
    {
        struct TestTraits
        {
            static LSTATUS __cdecl RegistryKeyOpen(
                _In_ HKEY hKey,
                _In_opt_ LPCWSTR lpSubKey,
                _In_opt_ DWORD ulOptions,
                _In_ REGSAM samDesired,
                _Out_ PHKEY phkResult
            )
            {
                Assert::AreEqual(L"ShouldNotExist", lpSubKey);
                Assert::AreEqual<DWORD>(KEY_QUERY_VALUE | KEY_WOW64_32KEY, samDesired);

                *phkResult = (HKEY)1;

                return ERROR_SUCCESS;
            }

            static LSTATUS __cdecl RegistryKeyQueryValue(
                _In_ HKEY hKey,
                _In_opt_ LPCWSTR lpValueName,
                _Reserved_ LPDWORD lpReserved,
                _Out_opt_ LPDWORD lpType,
                _Out_opt_ LPBYTE lpData,
                _Inout_opt_ LPDWORD lpcbData
            )
            {
                const LPCWSTR wsz = L"Test";

                Assert::AreEqual(1UL, (DWORD)hKey);
                Assert::AreEqual(L"Value", lpValueName);

                if (lpType)
                {
                    *lpType = REG_SZ;
                }

                auto cch = wcslen(wsz);
                *lpcbData = ++cch * sizeof(WCHAR);

                if (lpData)
                {
                    wcscpy_s((LPWSTR)lpData, cch, wsz);
                }

                return ERROR_SUCCESS;
            }

            static BOOL __cdecl RegistryKeyClose(_In_ HANDLE hObject)
            {
                Assert::AreEqual(1UL, (DWORD)hObject);

                return TRUE;
            }
        };

        std::wstring value;
        {
            RegistryKey<TestTraits> sut(HKEY_LOCAL_MACHINE, L"ShouldNotExist");
            value = sut.GetString(L"Value");
        }

        Assert::AreEqual(L"Test", value.c_str());
    }
};
