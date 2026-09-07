// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
#include "dllmain.h"
#include "AppxUtils.h"

BOOL APIENTRY DllMain(HMODULE /* hModule */, DWORD ul_reason_for_call, LPVOID /* lpReserved */)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

static UINT64 g_DllRefCount{ 0 };

void __fastcall DllAddRef()
{ InterlockedIncrement(&g_DllRefCount); }

void __fastcall DllRelease()
{ InterlockedDecrement(&g_DllRefCount); }

constexpr bool __stdcall IsEqualWStr(const wchar_t* left, const wchar_t* right, const UINT32 maxLength)
{
    if (left == right)
    { return true; }
    else if (left && right)
    {
        for (UINT32 i{ 0 }; i < maxLength; ++i)
        {
            const auto& elementA{ left[i] };
            const auto& elementB{ right[i] };
            if (elementA == elementB)
            {
                if (!elementA)
                { return true; }
            }
            else
            { return false; }
        }
        return true;
    }
    else
    { return false; }
}

constexpr inline UINT32 max(const UINT32 a, const UINT32 b)
{ return a > b ? a : b; }

STDAPI DllGetActivationFactory(HSTRING className, IActivationFactory** factory)
{
    UINT32 count{ 255 };
    auto* classNameRaw{ WindowsGetStringRawBuffer(className, &count) };
    if (IsEqualWStr(L"AppxUtils.AppxPackageFactory", classNameRaw, max(count, 28)))
    {
        ABI::AppxUtils::AppxPackageFactory* instance{ new ABI::AppxUtils::AppxPackageFactory{} };
        if (instance)
        {
            *factory = instance;
            return S_OK;
        }
        else
        { return E_OUTOFMEMORY; }
    }
    else
    { return E_NOINTERFACE; }
}

STDAPI DllCanUnloadNow()
{ return InterlockedCompareExchange(&g_DllRefCount, 0, 0) ? S_FALSE : S_OK; }
