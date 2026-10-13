// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once
//Win32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <combaseapi.h>
#include <unknwn.h>
#include <appxpackaging.h>
#include <shcore.h>
#include <heapapi.h>
#include <synchapi.h>

//WinRT
#include <hstring.h>
#include <winstring.h>
#include <inspectable.h>
#include <roapi.h>

//WinRT ABI
#include <Windows.Foundation.h>
#include <Windows.Foundation.Collections.h>
#include <Windows.ApplicationModel.h>
#include <Windows.Storage.h>
#include <Windows.Storage.Streams.h>
#include <Windows.System.h>

//Self
#include "dllmain.h"
#include "Generated Files/AppxUtils.g.h"
#include "Generated Files/AppxUtilsInterop.g.h"
#include "Infrastructures/GuidHelpers.hpp"
#include "Infrastructures/HeapManagement.h"
#include "Infrastructures/InspectableBase.hpp"
#include "Infrastructures/StaticHelpers.hpp"
#include "Infrastructures/FoundationStructComparer.hpp"
#include "Infrastructures/StructLifetimeFunctions.hpp"
#include "Infrastructures/StructLifetimeFunctionsSpecialized.h"
#include "Infrastructures/Reference.hpp"
#include "Infrastructures/VectorView.hpp"
#include "Infrastructures/AsyncInfoBase.hpp"
#include "Infrastructures/AsyncOperationBase.hpp"
#define E_CANCELED HRESULT_FROM_WIN32(ERROR_CANCELLED)
