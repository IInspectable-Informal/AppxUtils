#pragma once
#include "targetver.h"

//Win32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <combaseapi.h>
#include <unknwn.h>
#include <appxpackaging.h>
#include <shcore.h>

//WinRT
#include <hstring.h>
#include <winstring.h>
#include <inspectable.h>
#include <roapi.h>

//WinRT ABI
#include <Windows.Foundation.h>
#include <Windows.Foundation.Collections.h>
#include <Windows.ApplicationModel.h>
#include <Windows.Data.Xml.Dom.h>
#include <Windows.Storage.h>
#include <Windows.Storage.Streams.h>
#include <Windows.System.h>

//Self
#include "Generated Files\AppxUtils.g.h"
#include "dllmain.h"
#include "InspectableBase.hpp"
#include "StaticHelpers.hpp"
#include "GuidHelpers.hpp"
#include "FoundationStructComparer.hpp"
#include "StructLifetimeFunctions.hpp"
#include "StructLifetimeFunctionsSpecialized.h"
#include "Reference.hpp"
#include "VectorView.hpp"
