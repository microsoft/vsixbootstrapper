// <copyright file="VSIXBootstrapperTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(VSIXBootstrapperTests)
{
public:
    TEST_METHOD_INITIALIZE(Initialize)
    {
        s_count = 0;
    }

    TEST_METHOD(VSIXBootstrapper_ShowError_Quiet)
    {
        struct TestErrorTraits
        {
            static int __cdecl ErrorShowMessage(
                _In_opt_ LPCWSTR lpText,
                _In_opt_ LPCWSTR lpCaption,
                _In_ UINT uType)
            {
                Assert::Fail(L"Unexpected");

                return 0;
            }
        };

        struct TestResourcesTraits
        {
            static int __cdecl ResourcesGetString(__in_opt HINSTANCE hInstance, __in UINT nID, __out_ecount_part(cchBufferMax, return +1) LPWSTR lpBuffer, __in int cchBufferMax)
            {
                Assert::Fail(L"Unexpected");

                return 0;
            }
        };

        typedef Traits<TestErrorTraits, TestResourcesTraits> TestTraits;

        Resources<TestTraits> resources(NULL);
        ShowError<TestTraits>(resources, true, 0);
    }

    TEST_METHOD(VSIXBootstrapper_ShowError)
    {
        struct TestErrorTraits
        {
            static int __cdecl ErrorShowMessage(
                _In_opt_ LPCWSTR lpText,
                _In_opt_ LPCWSTR lpCaption,
                _In_ UINT uType)
            {
                Assert::AreEqual(L"Text", lpText);
                Assert::AreEqual(L"Caption", lpCaption);
                Assert::AreEqual<DWORD>(MB_OK | MB_ICONERROR, uType);

                s_count++;

                return 0;
            }
        };

        struct TestResourcesTraits
        {
            static int __cdecl ResourcesGetString(__in_opt HINSTANCE hInstance, __in UINT nID, __out_ecount_part(cchBufferMax, return +1) LPWSTR lpBuffer, __in int cchBufferMax)
            {
                if (IDS_CAPTION == nID)
                {
                    const LPCWSTR wsz = L"Caption";

                    s_count++;

                    ::memcpy(lpBuffer, &wsz, sizeof(LPWSTR));
                    return wcslen(wsz);
                }
                else if (IDS_ERROR)
                {
                    const LPCWSTR wsz = L"Text";

                    s_count++;

                    ::memcpy(lpBuffer, &wsz, sizeof(LPWSTR));
                    return wcslen(wsz);
                }

                return 0;
            }
        };

        typedef Traits<TestErrorTraits, TestResourcesTraits> TestTraits;

        Resources<TestTraits> resources(NULL);
        ShowError<TestTraits>(resources, false, 0);

        Assert::AreEqual(3, s_count);
    }

    TEST_METHOD(VSIXBootstrapper_Run)
    {
        struct TestCoInitializerTraits
        {
            static HRESULT __cdecl CoInitialize(_In_ LPVOID)
            {
                s_count++;

                // Cause an exception to skip ISetupConfiguration.
                return E_FAIL;
            }

            static void __cdecl CoUninitialize()
            {
                Assert::Fail(L"Unexpected");
            }
        };

        struct TestIOTraits
        {
            static bool __cdecl IOFileExists(_In_ LPCWSTR wszPath)
            {
                return CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, L"C:\\ShouldNotExist\\VSIXInstaller.exe", -1, wszPath, -1);
            }
        };

        struct TestRegistryKeyTraits
        {
            static LSTATUS __cdecl RegistryKeyOpen(
                _In_ HKEY hKey,
                _In_opt_ LPCWSTR lpSubKey,
                _In_opt_ DWORD ulOptions,
                _In_ REGSAM samDesired,
                _Out_ PHKEY phkResult
            )
            {
                Assert::AreEqual((ULONG_PTR)HKEY_LOCAL_MACHINE, (ULONG_PTR)hKey);

                s_count++;

                if (CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, L"SOFTWARE\\Microsoft\\VisualStudio\\10.0\\Setup\\VS", -1, lpSubKey, -1))
                {
                    *phkResult = (HKEY)1;
                    return ERROR_SUCCESS;
                }

                *phkResult = NULL;
                return ERROR_FILE_NOT_FOUND;
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
                const LPCWSTR wsz = L"C:\\ShouldNotExist";

                Assert::AreEqual(1UL, (DWORD)hKey);
                Assert::AreEqual(L"EnvironmentDirectory", lpValueName);

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

        struct TestProcessTraits
        {
            static BOOL __cdecl ProcessCreate(
                _In_opt_ LPCWSTR lpApplicationName,
                _Inout_opt_ LPWSTR lpCommandLine,
                _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
                _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
                _In_ BOOL bInheritHandles,
                _In_ DWORD dwCreationFlags,
                _In_opt_ LPVOID lpEnvironment,
                _In_opt_ LPCWSTR lpCurrentDirectory,
                _In_ LPSTARTUPINFOW lpStartupInfo,
                _Out_ LPPROCESS_INFORMATION lpProcessInformation
            )
            {
                Assert::AreEqual<DWORD>(SW_NORMAL, lpStartupInfo->wShowWindow);
                Assert::AreEqual(L"\"C:\\ShouldNotExist\\VSIXInstaller.exe\" ignored", lpCommandLine);

                s_count++;

                lpProcessInformation->hProcess = (HANDLE)1;
                lpProcessInformation->hThread = (HANDLE)1;

                return TRUE;
            }

            static DWORD __cdecl ProcessWait(_In_ HANDLE hHandle, _In_ DWORD dwMilliseconds)
            {
                Assert::AreEqual((HANDLE)1, hHandle);

                s_count++;

                return WAIT_OBJECT_0;
            }

            static BOOL __cdecl ProcessGetExitCode(_In_ HANDLE hProcess, _Out_ LPDWORD lpExitCode)
            {
                Assert::AreEqual((HANDLE)1, hProcess);

                s_count++;

                *lpExitCode = ERROR_SUCCESS;

                return TRUE;
            }

            static BOOL __cdecl ProcessClose(_In_ HANDLE hObject)
            {
                Assert::AreEqual((HANDLE)1, hObject);

                return TRUE;
            }
        };

        typedef Traits<TestCoInitializerTraits, CommandLineTraits, ErrorTraits, TestIOTraits, TestProcessTraits, TestRegistryKeyTraits, ResourcesTraits> TestTraits;

        auto dwExitCode = Run<TestTraits>(NULL, L"ignored", SW_NORMAL);

        Assert::AreEqual<DWORD>(ERROR_SUCCESS, dwExitCode);
        Assert::AreEqual(8, s_count);
    }

    TEST_METHOD(VSIXBootstrapper_Run_Quiet)
    {
        struct TestCoInitializerTraits
        {
            static HRESULT __cdecl CoInitialize(_In_ LPVOID)
            {
                s_count++;

                // Cause an exception to skip ISetupConfiguration.
                return E_FAIL;
            }

            static void __cdecl CoUninitialize()
            {
                Assert::Fail(L"Unexpected");
            }
        };

        struct TestIOTraits
        {
            static bool __cdecl IOFileExists(_In_ LPCWSTR wszPath)
            {
                return CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, L"C:\\ShouldNotExist\\VSIXInstaller.exe", -1, wszPath, -1);
            }
        };

        struct TestRegistryKeyTraits
        {
            static LSTATUS __cdecl RegistryKeyOpen(
                _In_ HKEY hKey,
                _In_opt_ LPCWSTR lpSubKey,
                _In_opt_ DWORD ulOptions,
                _In_ REGSAM samDesired,
                _Out_ PHKEY phkResult
            )
            {
                Assert::AreEqual((ULONG_PTR)HKEY_LOCAL_MACHINE, (ULONG_PTR)hKey);

                s_count++;

                if (CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, L"SOFTWARE\\Microsoft\\VisualStudio\\10.0\\Setup\\VS", -1, lpSubKey, -1))
                {
                    *phkResult = (HKEY)1;
                    return ERROR_SUCCESS;
                }

                *phkResult = NULL;
                return ERROR_FILE_NOT_FOUND;
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
                const LPCWSTR wsz = L"C:\\ShouldNotExist";

                Assert::AreEqual(1UL, (DWORD)hKey);
                Assert::AreEqual(L"EnvironmentDirectory", lpValueName);

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

        struct TestProcessTraits
        {
            static BOOL __cdecl ProcessCreate(
                _In_opt_ LPCWSTR lpApplicationName,
                _Inout_opt_ LPWSTR lpCommandLine,
                _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
                _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
                _In_ BOOL bInheritHandles,
                _In_ DWORD dwCreationFlags,
                _In_opt_ LPVOID lpEnvironment,
                _In_opt_ LPCWSTR lpCurrentDirectory,
                _In_ LPSTARTUPINFOW lpStartupInfo,
                _Out_ LPPROCESS_INFORMATION lpProcessInformation
            )
            {
                Assert::AreEqual<DWORD>(SW_HIDE, lpStartupInfo->wShowWindow);
                Assert::AreEqual(L"\"C:\\ShouldNotExist\\VSIXInstaller.exe\" ignored", lpCommandLine);

                s_count++;

                lpProcessInformation->hProcess = (HANDLE)1;
                lpProcessInformation->hThread = (HANDLE)1;

                return TRUE;
            }

            static DWORD __cdecl ProcessWait(_In_ HANDLE hHandle, _In_ DWORD dwMilliseconds)
            {
                Assert::AreEqual((HANDLE)1, hHandle);

                s_count++;

                return WAIT_OBJECT_0;
            }

            static BOOL __cdecl ProcessGetExitCode(_In_ HANDLE hProcess, _Out_ LPDWORD lpExitCode)
            {
                Assert::AreEqual((HANDLE)1, hProcess);

                s_count++;

                *lpExitCode = ERROR_SUCCESS;

                return TRUE;
            }

            static BOOL __cdecl ProcessClose(_In_ HANDLE hObject)
            {
                Assert::AreEqual((HANDLE)1, hObject);

                return TRUE;
            }
        };

        typedef Traits<TestCoInitializerTraits, CommandLineTraits, ErrorTraits, TestIOTraits, TestProcessTraits, TestRegistryKeyTraits, ResourcesTraits> TestTraits;

        auto dwExitCode = Run<TestTraits>(NULL, L"ignored", SW_HIDE);

        Assert::AreEqual<DWORD>(ERROR_SUCCESS, dwExitCode);
        Assert::AreEqual(8, s_count);
    }

    TEST_METHOD(VSIXBootstrapper_Run_Not_Found)
    {
        struct TestCoInitializerTraits
        {
            static HRESULT __cdecl CoInitialize(_In_ LPVOID)
            {
                s_count++;

                // Cause an exception to skip ISetupConfiguration.
                return E_FAIL;
            }

            static void __cdecl CoUninitialize()
            {
                Assert::Fail(L"Unexpected");
            }
        };

        struct TestErrorTraits
        {
            static int __cdecl ErrorShowMessage(
                _In_opt_ LPCWSTR lpText,
                _In_opt_ LPCWSTR lpCaption,
                _In_ UINT uType)
            {
                Assert::AreEqual(L"Caption", lpCaption);
                Assert::AreEqual(L"Text", lpText);

                s_count++;

                return MB_OK;
            }
        };

        struct TestIOTraits
        {
            static bool __cdecl IOFileExists(_In_ LPCWSTR wszPath)
            {
                return CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, L"C:\\ShouldNotExist\\VSIXInstaller.exe", -1, wszPath, -1);
            }
        };

        struct TestRegistryKeyTraits
        {
            static LSTATUS __cdecl RegistryKeyOpen(
                _In_ HKEY hKey,
                _In_opt_ LPCWSTR lpSubKey,
                _In_opt_ DWORD ulOptions,
                _In_ REGSAM samDesired,
                _Out_ PHKEY phkResult
            )
            {
                Assert::AreEqual((ULONG_PTR)HKEY_LOCAL_MACHINE, (ULONG_PTR)hKey);

                s_count++;

                *phkResult = NULL;
                return ERROR_FILE_NOT_FOUND;
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
                Assert::Fail(L"Unexpected");

                return ERROR_FILE_NOT_FOUND;
            }

            static BOOL __cdecl RegistryKeyClose(_In_ HANDLE hObject)
            {
                Assert::Fail(L"Unexpected");

                return FALSE;
            }
        };

        struct TestResourcesTraits
        {
            static int ResourcesGetString(__in_opt HINSTANCE hInstance, __in UINT nID, __out_ecount_part(cchBufferMax, return +1) LPWSTR lpBuffer, __in int cchBufferMax)
            {
                if (IDS_CAPTION == nID)
                {
                    const LPCWSTR wsz = L"Caption";

                    ::memcpy(lpBuffer, &wsz, sizeof(LPWSTR));
                    return wcslen(wsz);
                }
                else if (IDS_NOTFOUND == nID)
                {
                    const LPCWSTR wsz = L"Text";

                    ::memcpy(lpBuffer, &wsz, sizeof(LPWSTR));
                    return wcslen(wsz);
                }

                Assert::Fail(L"Unexpected");

                return 0;
            };
        };

        typedef Traits<TestCoInitializerTraits, CommandLineTraits, TestErrorTraits, TestIOTraits, ProcessTraits, TestRegistryKeyTraits, TestResourcesTraits> TestTraits;

        auto dwExitCode = Run<TestTraits>(NULL, L"ignored", SW_NORMAL);

        Assert::AreEqual<DWORD>(ERROR_FILE_NOT_FOUND, dwExitCode);
        Assert::AreEqual(6, s_count);
    }

    TEST_METHOD(VSIXBootstrapper_Run_Not_Found_Quiet)
    {
        struct TestCoInitializerTraits
        {
            static HRESULT __cdecl CoInitialize(_In_ LPVOID)
            {
                s_count++;

                // Cause an exception to skip ISetupConfiguration.
                return E_FAIL;
            }

            static void __cdecl CoUninitialize()
            {
                Assert::Fail(L"Unexpected");
            }
        };

        struct TestErrorTraits
        {
            static int __cdecl ErrorShowMessage(
                _In_opt_ LPCWSTR lpText,
                _In_opt_ LPCWSTR lpCaption,
                _In_ UINT uType)
            {
                Assert::Fail(L"Unexpected");

                return MB_OK;
            }
        };

        struct TestIOTraits
        {
            static bool __cdecl IOFileExists(_In_ LPCWSTR wszPath)
            {
                return CSTR_EQUAL == ::CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, L"C:\\ShouldNotExist\\VSIXInstaller.exe", -1, wszPath, -1);
            }
        };

        struct TestRegistryKeyTraits
        {
            static LSTATUS __cdecl RegistryKeyOpen(
                _In_ HKEY hKey,
                _In_opt_ LPCWSTR lpSubKey,
                _In_opt_ DWORD ulOptions,
                _In_ REGSAM samDesired,
                _Out_ PHKEY phkResult
            )
            {
                Assert::AreEqual((ULONG_PTR)HKEY_LOCAL_MACHINE, (ULONG_PTR)hKey);

                s_count++;

                *phkResult = NULL;
                return ERROR_FILE_NOT_FOUND;
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
                Assert::Fail(L"Unexpected");

                return ERROR_FILE_NOT_FOUND;
            }

            static BOOL __cdecl RegistryKeyClose(_In_ HANDLE hObject)
            {
                Assert::Fail(L"Unexpected");

                return FALSE;
            }
        };

        struct TestResourcesTraits
        {
            static int ResourcesGetString(__in_opt HINSTANCE hInstance, __in UINT nID, __out_ecount_part(cchBufferMax, return +1) LPWSTR lpBuffer, __in int cchBufferMax)
            {
                if (IDS_CAPTION == nID)
                {
                    const LPCWSTR wsz = L"Caption";

                    ::memcpy(lpBuffer, &wsz, sizeof(LPWSTR));
                    return wcslen(wsz);
                }
                else if (IDS_NOTFOUND == nID)
                {
                    const LPCWSTR wsz = L"Text";

                    ::memcpy(lpBuffer, &wsz, sizeof(LPWSTR));
                    return wcslen(wsz);
                }

                Assert::Fail(L"Unexpected");

                return 0;
            };
        };

        typedef Traits<TestCoInitializerTraits, CommandLineTraits, TestErrorTraits, TestIOTraits, ProcessTraits, TestRegistryKeyTraits, TestResourcesTraits> TestTraits;

        auto dwExitCode = Run<TestTraits>(NULL, L"ignored", SW_HIDE);

        Assert::AreEqual<DWORD>(ERROR_FILE_NOT_FOUND, dwExitCode);
        Assert::AreEqual(5, s_count);
    }

private:
    static int s_count;
};

int VSIXBootstrapperTests::s_count = 0;
