// <copyright file="ProcessTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(ProcessTests)
{
public:
    TEST_METHOD(Process_Constructor_Throws)
    {
        struct TestTraits
        {
            static BOOL __cdecl Create(
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
                Assert::AreEqual(L"\"C:\\ShouldNotExist.exe\" ignored", lpCommandLine);

                return FALSE;
            }

            static BOOL __cdecl Close(_In_ HANDLE hObject)
            {
                Assert::Fail(L"Unexpected");

                return TRUE;
            }
        };

        Assert::ExpectException<win32_error>([]() { Process<TestTraits>(SW_NORMAL, L"C:\\ShouldNotExist.exe", L"ignored"); });
    }

    TEST_METHOD(Process_Wait)
    {
        struct TestTraits
        {
            static BOOL __cdecl Create(
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
                Assert::AreEqual<DWORD>(STARTF_USESHOWWINDOW, lpStartupInfo->dwFlags);
                Assert::AreEqual<DWORD>(SW_HIDE, lpStartupInfo->wShowWindow);
                Assert::AreEqual(L"\"C:\\ShouldNotExist.exe\" ignored", lpCommandLine);

                lpProcessInformation->hProcess = reinterpret_cast<HANDLE>(1);
                lpProcessInformation->hThread = reinterpret_cast<HANDLE>(1);

                return TRUE;
            }

            static DWORD __cdecl Wait(_In_ HANDLE hHandle, _In_ DWORD dwMilliseconds)
            {
                Assert::AreEqual((HANDLE)1, hHandle);

                return WAIT_OBJECT_0;
            }

            static BOOL __cdecl GetExitCode(_In_ HANDLE hProcess, _Out_ LPDWORD lpExitCode)
            {
                Assert::AreEqual((HANDLE)1, hProcess);

                *lpExitCode = ERROR_SUCCESS;

                return TRUE;
            }

            static BOOL __cdecl Close(_In_ HANDLE hObject)
            {
                Assert::AreEqual((HANDLE)1, hObject);

                return TRUE;
            }
        };

        DWORD dwExitCode = 0;
        {
            Process<TestTraits> sut(SW_HIDE, L"C:\\ShouldNotExist.exe", L"ignored");

            sut.Wait();
            dwExitCode = sut.GetExitCode();
        }

        Assert::AreEqual<DWORD>(ERROR_SUCCESS, dwExitCode);
    }

    TEST_METHOD(Process_Wait_Throws)
    {
        struct TestTraits
        {
            static BOOL __cdecl Create(
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
                Assert::AreEqual<DWORD>(STARTF_USESHOWWINDOW, lpStartupInfo->dwFlags);
                Assert::AreEqual<DWORD>(SW_HIDE, lpStartupInfo->wShowWindow);
                Assert::AreEqual(L"\"C:\\ShouldNotExist.exe\" ignored", lpCommandLine);

                lpProcessInformation->hProcess = NULL;
                lpProcessInformation->hThread = NULL;

                return TRUE;
            }

            static DWORD __cdecl Wait(_In_ HANDLE hHandle, _In_ DWORD dwMilliseconds)
            {
                Assert::Fail(L"Unexpected");

                return WAIT_OBJECT_0;
            }

            static BOOL __cdecl Close(_In_ HANDLE hObject)
            {
                Assert::Fail(L"Unexpected");

                return TRUE;
            }
        };

        Process<TestTraits> sut(SW_HIDE, L"C:\\ShouldNotExist.exe", L"ignored");

        Assert::ExpectException<win32_error>([&]() { sut.Wait(); });
    }
};
