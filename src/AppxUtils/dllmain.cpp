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
{
    InterlockedIncrement(&g_DllRefCount);
}

void __fastcall DllRelease()
{
    InterlockedDecrement(&g_DllRefCount);
}

STDAPI DllGetActivationFactory(HSTRING className, IInspectable** factory)
{
    if (wcsncmp(L"AppxUtils.AppxPackageFactory", WindowsGetStringRawBuffer(className, nullptr), 28) == 0)
    {
        ABI::AppxUtils::AppxPackageFactory* instance{ new ABI::AppxUtils::AppxPackageFactory{} };
        if (instance)
        {
            *factory = reinterpret_cast<IInspectable*>(instance);
            return S_OK;
        }
        else
        {
            return E_OUTOFMEMORY;
        }
    }
    else
    {
        return E_NOINTERFACE;
    }
}

STDAPI DllCanUnloadNow()
{
    return g_DllRefCount ? S_FALSE : S_OK;
}