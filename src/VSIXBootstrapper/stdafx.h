// <copyright file="stdafx.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

#include "targetver.h"

// Windows header files
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

// COM support header files
#include <comutil.h>
#include <comdef.h>

// C runtime header files
#include <stdio.h>

// STL header files
#include <exception>
#include <filesystem>
#include <functional>
#include <string>
#include <system_error>
#include <vector>

// Reference project header files
#define MAXUINT UINT_MAX
#include <Setup.Configuration.h>

// Project header files
#include "Exceptions.h"
#include "CoInitializer.h"
#include "CommandLine.h"
#include "RegistryKey.h"
#include "Resource.h"
#include "Resources.h"

// Type definitions
_COM_SMARTPTR_TYPEDEF(ISetupConfiguration2, __uuidof(ISetupConfiguration2));
_COM_SMARTPTR_TYPEDEF(IEnumSetupInstances, __uuidof(IEnumSetupInstances));
_COM_SMARTPTR_TYPEDEF(ISetupInstance2, __uuidof(ISetupInstance2));
