#pragma once
#include "dllmain.h"

template<TrustLevel level, typename... I>
class InspectableCommonBase abstract : public I...
{
public:
    InspectableCommonBase()
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
        IID* buffer{ reinterpret_cast<IID*>(CoTaskMemAlloc(sizeof(IID) * s_IIdCount)) };
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
            *ppvObject = nullptr;
            if (IsEqualGuid(riid, __uuidof(IUnknown)))
            { *ppvObject = reinterpret_cast<IUnknown*>(this); }
            else if (IsEqualGuid(riid, __uuidof(IInspectable)))
            { *ppvObject = reinterpret_cast<IInspectable*>(this); }
            else
            {
                for (ULONG i = 0; i < s_IIdCount; ++i)
                {
                    if (IsEqualGuid(riid, s_IIds[i]))
                    { *ppvObject = m_Interfaces[i]; break; }
                }
            }
            if (*ppvObject)
            {
                this->AddRef();
                return S_OK;
            }
            else
            { return E_NOINTERFACE; }
        }
        else
        { return E_POINTER; }
    }

    ULONG STDMETHODCALLTYPE AddRef() override
    { return InterlockedIncrement(&m_RefCount); }

    virtual ~InspectableCommonBase()
    {
        DllRelease();
    }

protected:
    ULONG m_RefCount{ 1 };

    static constexpr ULONG s_IIdCount{ sizeof...(I) };
    static constexpr IID s_IIds[s_IIdCount]{ __uuidof(I)... };

    void* const m_Interfaces[s_IIdCount]{ static_cast<I*>(this)... };
};

template<TrustLevel level, typename... I>
class InspectableBase abstract : public InspectableCommonBase<level, I...>
{
public:
    //IUnknown
    ULONG STDMETHODCALLTYPE Release() override
    {
        ULONG cRef{ InterlockedDecrement(&m_RefCount) };
        if (!cRef)
        { delete this; }
        return cRef;
    }
};

template<TrustLevel level, typename... I>
class InspectableElementBase abstract : public InspectableCommonBase<level, I...>
{
public:
    //IUnknown
    ULONG STDMETHODCALLTYPE Release() override
    {
        ULONG cRef{ InterlockedDecrement(&m_RefCount) };
        if (!cRef)
        { this->~InspectableElementBase(); }
        return cRef;
    }
};
