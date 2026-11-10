// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "dllmain.h"
#include <stdio.h>

template<TrustLevel level, typename... I>
class InspectableCommonBase : public I...
{
public:
    InspectableCommonBase() noexcept
    {
        DllAddRef();
    }

    InspectableCommonBase(const InspectableCommonBase&) = delete;
    InspectableCommonBase& operator=(const InspectableCommonBase&) = delete;
    InspectableCommonBase(InspectableCommonBase&&) = delete;
    InspectableCommonBase& operator=(InspectableCommonBase&&) = delete;

    //IInspectable
    HRESULT STDMETHODCALLTYPE GetIids(ULONG* iidCount, IID** iids)
    {
        IID* const buffer{ static_cast<IID*>(CoTaskMemAlloc(sizeof(IID) * s_IIdCount)) };
        if (buffer)
        {
            for (ULONG i = 0; i < s_IIdCount; ++i)
            { buffer[i] = s_IIds[i]; }
            *iidCount = s_IIdCount;
            *iids = buffer;
            return S_OK;
        }
        else
        { return E_OUTOFMEMORY; }
    }

    HRESULT STDMETHODCALLTYPE GetTrustLevel(TrustLevel* trustLevel)
    {
        *trustLevel = level;
        return S_OK;
    }

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
            else if (IsEqualGuid(riid, __uuidof(IInspectable)))
            {
                *ppvObject = reinterpret_cast<IInspectable*>(this);
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

    virtual ~InspectableCommonBase() noexcept
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

template<TrustLevel level, typename... I>
class InspectableBase abstract : public InspectableCommonBase<level, I...>
{
public:
    //IUnknown
    ULONG STDMETHODCALLTYPE Release() override
    {
        const ULONG cRef{ InterlockedDecrement(&this->m_RefCount) };
        printf("-- COM Object (0x%016p) Ref Count: %d\n", this, cRef);
        if (!cRef)
        { delete this; }
        return cRef;
    }

    virtual ~InspectableBase() noexcept = default;
};

template<TrustLevel level, typename... I>
class InspectableElementBase abstract : public InspectableCommonBase<level, I...>
{
public:
    //IUnknown
    ULONG STDMETHODCALLTYPE Release() override
    {
        const ULONG cRef{ InterlockedDecrement(&this->m_RefCount) };
        printf("-- COM Object (0x%016p) Ref Count: %d\n", this, cRef);
        if (!cRef)
        { this->~InspectableElementBase(); }
        return cRef;
    }

    virtual ~InspectableElementBase() noexcept = default;
};
