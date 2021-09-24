// <copyright file="stdafx.h" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#pragma once

#include "targetver.h"

// Reference project header files
#include <stdafx.h>

// C++ unit test header files
#include "CppUnitTest.h"

// Project header files

// Disable Prefast warnings
#pragma warning(disable: 6101) // Returning uninitialied data. This is from test cases that fail if the code executes.
#pragma warning(disable: 6054) // Buffer may not be null-terminated.
#pragma warning(disable: 6385) // Reading extra data
#pragma warning(disable: 6386) // Potential buffer overrun.
#pragma warning(disable: 6387) // Potential input not matching expected.
