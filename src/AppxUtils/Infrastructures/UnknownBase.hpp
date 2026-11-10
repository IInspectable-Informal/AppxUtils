// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "dllmain.h"
#include <stdio.h>

template<ABI::AppxUtils::Internal::ICOMInterfaceNotIUnknown... I>
class UnknownBase abstract : public I...
{
public:
    UnknownBase() noexcept
    {
        DllAddRef();
    }

    UnknownBase(const UnknownBase&) = delete;
    UnknownBase& operator=(const UnknownBase&) = delete;
    UnknownBase(UnknownBase&&) = delete;
    UnknownBase& operator=(UnknownBase&&) = delete;

    //IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override
    {
        if (ppvObject)
        {
            if (IsEqualGuid(riid, __uuidof(IUnknown)))
            {
                *ppvObject = reinterpret_cast<IUnknown*>(this);
                this->AddRef();
                return S_OK;
            }
            else if ((QIInternal<I>(riid, ppvObject) || ...))
            {
                this->AddRef();
                return S_OK;
            }
            else
            {
                *ppvObject = nullptr;
                return E_NOINTERFACE;
            }
        }
        else
        { return E_POINTER; }
    }

    ULONG STDMETHODCALLTYPE AddRef() override
    {
        const ULONG cRef{ InterlockedIncrement(&m_RefCount) };
        printf("++ COM Object (0x%016p) Ref Count: %d\n", this, cRef);
        return cRef;
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        const ULONG cRef{ InterlockedDecrement(&m_RefCount) };
        printf("-- COM Object (0x%016p) Ref Count: %d\n", this, cRef);
        if (!cRef)
        { delete this; }
        return cRef;
    }

    virtual ~UnknownBase() noexcept
    {
        DllRelease();
    }

protected:
    ULONG m_RefCount{ 1 };

    static constexpr ULONG s_IIdCount{ sizeof...(I) };
    static constexpr IID s_IIds[s_IIdCount]{ __uuidof(I)... };

private:
    template<typename I>
    bool STDMETHODCALLTYPE QIInternal(REFIID riid, void** ppvObject)
    {
        if (IsEqualGuid(riid, __uuidof(I)))
        {
            *ppvObject = static_cast<I*>(this);
            return true;
        }
        else
        { return false; }
    }
};
