#include "pch.h"
#include "AppxPackage.h"
#include "AppxPackagePayloadFile.h"
#include "helpers.hpp"

namespace ABI
{
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Collections;
    using namespace Windows::ApplicationModel;
    using namespace Windows::Data::Xml::Dom;
    using namespace Windows::Storage::Streams;
    using namespace Windows::System;
    using namespace AppxUtils::Internal;
}

namespace ABI::AppxUtils
{    
    constexpr inline ABI::PackageVersion UInt64ToPkgVer(UINT64 verNum)
    {
        ABI::PackageVersion ver{};
        ver.Major = (verNum >> 48) & 0xFFFF;
        ver.Minor = (verNum >> 32) & 0xFFFF;
        ver.Build = (verNum >> 16) & 0xFFFF;
        ver.Revision = verNum & 0xFFFF;
        return ver;
    }

    //AppxPackage
    //This member methods
    AppxPackage::AppxPackage(IAppxPackageReader*& reader, const CRITICAL_SECTION& criticalSection) noexcept : m_AppxPackageReader(reader), m_CriticalSection(criticalSection)
    {

    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_Name(HSTRING* value)
    {
        HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Name), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_Name)
            {
                IAppxManifestPackageId* pkgId{ nullptr };
                hr = GetPackageId(pkgId);
                if (SUCCEEDED(hr))
                {
                    LPWSTR name{ nullptr };
                    hr = pkgId->GetName(&name);
                    if (SUCCEEDED(hr))
                    {
                        hr = WindowsCreateString(name, WStringLength(name), &m_Name);
                        CoTaskMemFree(name);
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
            local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Name), nullptr, nullptr));
        }
        return WindowsDuplicateString(local, value);
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_FamilyName(HSTRING* value)
    {
        HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_FamilyName), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_FamilyName)
            {
                IAppxManifestPackageId* pkgId{ nullptr };
                hr = GetPackageId(pkgId);
                if (SUCCEEDED(hr))
                {
                    LPWSTR name{ nullptr };
                    hr = pkgId->GetPackageFamilyName(&name);
                    if (SUCCEEDED(hr))
                    {
                        hr = WindowsCreateString(name, WStringLength(name), &m_FamilyName);
                        CoTaskMemFree(name);
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
            local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_FamilyName), nullptr, nullptr));
        }
        return WindowsDuplicateString(local, value);
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_FullName(HSTRING* value)
    {
        HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_FullName), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_FullName)
            {
                IAppxManifestPackageId* pkgId{ nullptr };
                hr = GetPackageId(pkgId);
                if (SUCCEEDED(hr))
                {
                    LPWSTR name{ nullptr };
                    hr = pkgId->GetPackageFullName(&name);
                    if (SUCCEEDED(hr))
                    {
                        hr = WindowsCreateString(name, WStringLength(name), &m_FullName);
                        CoTaskMemFree(name);
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
            local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_FullName), nullptr, nullptr));
        }
        return WindowsDuplicateString(local, value);
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_Publisher(HSTRING* value)
    {
        HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Publisher), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_Publisher)
            {
                IAppxManifestPackageId* pkgId{ nullptr };
                hr = GetPackageId(pkgId);
                if (SUCCEEDED(hr))
                {
                    LPWSTR name{ nullptr };
                    hr = pkgId->GetPublisher(&name);
                    if (SUCCEEDED(hr))
                    {
                        hr = WindowsCreateString(name, WStringLength(name), &m_Publisher);
                        CoTaskMemFree(name);
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
            local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Publisher), nullptr, nullptr));
        }
        return WindowsDuplicateString(local, value);
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_Version(struct ABI::PackageVersion* value)
    {
        long local{ InterlockedCompareExchange(reinterpret_cast<long*>(&m_HasVersion), false, false) };
        if (local == false)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_HasVersion)
            {
                IAppxManifestPackageId* pkgId{ nullptr };
                hr = GetPackageId(pkgId);
                if (SUCCEEDED(hr))
                {
                    UINT64 ver{ 0 };
                    hr = pkgId->GetVersion(&ver);
                    if (SUCCEEDED(hr))
                    {
                        m_HasVersion = true;
                        m_Version.Major = (ver >> 48) & 0xFFFF;
                        m_Version.Minor = (ver >> 32) & 0xFFFF;
                        m_Version.Build = (ver >> 16) & 0xFFFF;
                        m_Version.Revision = ver & 0xFFFF;
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
        }

        *value = m_Version;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_Architecture(ABI::ProcessorArchitecture* value)
    {
        long local{ InterlockedCompareExchange(reinterpret_cast<long*>(&m_HasArchitecture), false, false) };
        if (local == false)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_HasArchitecture)
            {
                IAppxManifestPackageId* pkgId{ nullptr };
                hr = GetPackageId(pkgId);
                if (SUCCEEDED(hr))
                {
                    IAppxManifestPackageId2* pkgId2{ nullptr };
                    hr = pkgId->QueryInterface(__uuidof(pkgId2), to_void_pp(pkgId2));
                    if (SUCCEEDED(hr))
                    {
                        APPX_PACKAGE_ARCHITECTURE2 arch{ APPX_PACKAGE_ARCHITECTURE2_UNKNOWN };
                        hr = pkgId2->GetArchitecture2(&arch);
                        pkgId2->Release();
                        if (SUCCEEDED(hr))
                        {
                            m_HasArchitecture = true;
                            m_Architecture = static_cast<ABI::ProcessorArchitecture>(arch);
                        }
                    }
                    else
                    {
                        APPX_PACKAGE_ARCHITECTURE arch{ APPX_PACKAGE_ARCHITECTURE_X86 };
                        hr = pkgId->GetArchitecture(&arch);
                        if (SUCCEEDED(hr))
                        {
                            m_HasArchitecture = true;
                            m_Architecture = static_cast<ABI::ProcessorArchitecture>(arch);
                        }
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
        }

        *value = m_Architecture;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_PackageType(AppxPackageType* value)
    {
        long local{ InterlockedCompareExchange(reinterpret_cast<long*>(&m_HasPackageType), false, false) };
        if (local == false)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_HasPackageType)
            {
                IAppxManifestReader* manifest{ nullptr };
                hr = GetManifestReader(manifest);
                if (SUCCEEDED(hr))
                {
                    IAppxManifestProperties* props{ nullptr };
                    hr = manifest->GetProperties(&props);
                    if (SUCCEEDED(hr))
                    {
                        BOOL framework{ false };
                        BOOL resource{ false };
                        HRESULT hr1{ props->GetBoolValue(L"Framework", &framework) };
                        HRESULT hr2{ props->GetBoolValue(L"ResourcePackage", &resource) };
                        props->Release();
                        if (SUCCEEDED(hr2) || hr2 == E_INVALIDARG)
                        {
                            m_HasPackageType = true;
                            if (resource)
                            { m_PackageType = AppxPackageType::Resource; }
                        }
                        else if (SUCCEEDED(hr1) || hr1 == E_INVALIDARG)
                        {
                            m_HasPackageType = true;
                            if (framework)
                            { m_PackageType = AppxPackageType::Framework; }
                        }
                        else if (hr1 == E_INVALIDARG && hr2 == E_INVALIDARG)
                        { m_HasPackageType = true; }
                        else
                        { hr = hr1; }
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
        }

        *value = m_PackageType;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_ResourceId(HSTRING* value)
    {
        HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_ResourceId), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_ResourceId)
            {
                IAppxManifestPackageId* pkgId{ nullptr };
                hr = GetPackageId(pkgId);
                if (SUCCEEDED(hr))
                {
                    LPWSTR name{ nullptr };
                    hr = pkgId->GetResourceId(&name);
                    if (SUCCEEDED(hr))
                    {
                        if (name)
                        {
                            hr = WindowsCreateString(name, WStringLength(name), &m_ResourceId);
                            CoTaskMemFree(name);
                        }
                        else
                        { hr = WindowsCreateString(nullptr, 0, &m_ResourceId); }
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
            local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_ResourceId), nullptr, nullptr));
        }
        return WindowsDuplicateString(local, value);
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_Logo(HSTRING* value)
    {
        HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Logo), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_Logo)
            {
                IAppxManifestReader* manifest{ nullptr };
                hr = GetManifestReader(manifest);
                if (SUCCEEDED(hr))
                {
                    IAppxManifestProperties* props{ nullptr };
                    hr = manifest->GetProperties(&props);
                    if (SUCCEEDED(hr))
                    {
                        LPWSTR name{ nullptr };
                        hr = props->GetStringValue(L"Logo", &name);
                        props->Release();
                        if (SUCCEEDED(hr))
                        {
                            hr = WindowsCreateString(name, WStringLength(name), &m_Logo);
                            CoTaskMemFree(name);
                        }
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
            local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Logo), nullptr, nullptr));
        }
        return WindowsDuplicateString(local, value);
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_DisplayName(HSTRING* value)
    {
        HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_DisplayName), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_DisplayName)
            {
                IAppxManifestReader* manifest{ nullptr };
                hr = GetManifestReader(manifest);
                if (SUCCEEDED(hr))
                {
                    IAppxManifestProperties* props{ nullptr };
                    hr = manifest->GetProperties(&props);
                    if (SUCCEEDED(hr))
                    {
                        LPWSTR name{ nullptr };
                        hr = props->GetStringValue(L"DisplayName", &name);
                        props->Release();
                        if (SUCCEEDED(hr))
                        {
                            hr = WindowsCreateString(name, WStringLength(name), &m_DisplayName);
                            CoTaskMemFree(name);
                        }
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
            local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_DisplayName), nullptr, nullptr));
        }
        return WindowsDuplicateString(local, value);
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_PublisherDisplayName(HSTRING* value)
    {
        HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_PublisherDisplayName), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_PublisherDisplayName)
            {
                IAppxManifestReader* manifest{ nullptr };
                hr = GetManifestReader(manifest);
                if (SUCCEEDED(hr))
                {
                    IAppxManifestProperties* props{ nullptr };
                    hr = manifest->GetProperties(&props);
                    if (SUCCEEDED(hr))
                    {
                        LPWSTR name{ nullptr };
                        hr = props->GetStringValue(L"PublisherDisplayName", &name);
                        props->Release();
                        if (SUCCEEDED(hr))
                        {
                            hr = WindowsCreateString(name, WStringLength(name), &m_PublisherDisplayName);
                            CoTaskMemFree(name);
                        }
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
            local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_PublisherDisplayName), nullptr, nullptr));
        }
        return WindowsDuplicateString(local, value);
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_Description(HSTRING* value)
    {
        HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Description), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_Description)
            {
                IAppxManifestReader* manifest{ nullptr };
                hr = GetManifestReader(manifest);
                if (SUCCEEDED(hr))
                {
                    IAppxManifestProperties* props{ nullptr };
                    hr = manifest->GetProperties(&props);
                    if (SUCCEEDED(hr))
                    {
                        LPWSTR name{ nullptr };
                        hr = props->GetStringValue(L"Description", &name);
                        props->Release();
                        if (SUCCEEDED(hr) || hr == E_INVALIDARG)
                        {
                            if (name)
                            {
                                hr = WindowsCreateString(name, WStringLength(name), &m_Description);
                                CoTaskMemFree(name);
                            }
                            else
                            { hr = WindowsCreateString(nullptr, 0, &m_Description); }
                        }
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
            local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Description), nullptr, nullptr));
        }
        return WindowsDuplicateString(local, value);
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_PackageDependencies(ABI::IVectorView<struct AppxPackageDependency>** value)
    {
        auto local{ reinterpret_cast<VectorView<struct AppxPackageDependency>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_PackageDependencies), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_PackageDependencies)
            {
                IAppxManifestReader* reader{ nullptr };
                hr = GetManifestReader(reader);
                if (SUCCEEDED(hr))
                {
                    IAppxManifestPackageDependenciesEnumerator* enumerator{ nullptr };
                    hr = reader->GetPackageDependencies(&enumerator);
                    if (SUCCEEDED(hr))
                    {
                        UINT32 count{ 0 };
                        BOOL hasNext{ false };
                        enumerator->GetHasCurrent(&hasNext);
                        while (hasNext)
                        {
                            ++count;
                            enumerator->MoveNext(&hasNext);
                        }
                        enumerator->Release();
                        hr = reader->GetPackageDependencies(&enumerator);
                        if (SUCCEEDED(hr))
                        {
                            struct AppxPackageDependency* dependencies{ new struct AppxPackageDependency[count]{} };
                            if (dependencies)
                            {
                                enumerator->GetHasCurrent(&hasNext);
                                UINT32 completed{ 0 };
                                while (hasNext)
                                {
                                    IAppxManifestPackageDependency* dependency{ nullptr };
                                    hr = enumerator->GetCurrent(&dependency);
                                    if (SUCCEEDED(hr))
                                    {
                                        auto& dep{ dependencies[completed] };
                                        LPWSTR name{ nullptr };
                                        hr = dependency->GetName(&name);
                                        if (SUCCEEDED(hr))
                                        {
                                            LPWSTR publisher{ nullptr };
                                            hr = dependency->GetPublisher(&publisher);
                                            if (SUCCEEDED(hr))
                                            {
                                                UINT64 minVer{ 0 };
                                                hr = dependency->GetMinVersion(&minVer);
                                                if (SUCCEEDED(hr))
                                                {
                                                    hr = WindowsCreateString(name, WStringLength(name), &dep.Name);
                                                    if (SUCCEEDED(hr))
                                                    {
                                                        hr = WindowsCreateString(publisher, WStringLength(publisher), &dep.Publisher);
                                                        if (SUCCEEDED(hr))
                                                        {
                                                            dep.MinVersion = UInt64ToPkgVer(minVer);
                                                            dep.MaxMajorVersionTested = nullptr;
                                                            dep.Optional = nullptr;
                                                            {
                                                                IAppxManifestPackageDependency2* dependency2{ nullptr };
                                                                if (SUCCEEDED(dependency->QueryInterface(__uuidof(dependency2), to_void_pp(dependency2))))
                                                                {
                                                                    UINT16 maxMajorVersionTested{ 0 };
                                                                    if (SUCCEEDED(dependency2->GetMaxMajorVersionTested(&maxMajorVersionTested)))
                                                                    { Reference<UINT16>::CreateInstance(maxMajorVersionTested, dep.MaxMajorVersionTested); }
                                                                    dependency2->Release();
                                                                }
                                                                IAppxManifestPackageDependency3* dependency3{ nullptr };
                                                                if (SUCCEEDED(dependency->QueryInterface(__uuidof(dependency3), to_void_pp(dependency3))))
                                                                {
                                                                    BOOL isOptional{ false };
                                                                    if (SUCCEEDED(dependency3->GetIsOptional(&isOptional)))
                                                                    { Reference<bool>::CreateInstance(isOptional, dep.Optional); }
                                                                    dependency3->Release();
                                                                }
                                                            }
                                                            ++completed;
                                                            enumerator->MoveNext(&hasNext);
                                                        }
                                                    }
                                                }
                                                CoTaskMemFree(publisher);
                                            }
                                            CoTaskMemFree(name);
                                        }
                                        dependency->Release();
                                    }
                                    if (FAILED(hr))
                                    {
                                        for (UINT32 i = 0; i < completed; ++i)
                                        { StructLifetimeFunctions<struct AppxPackageDependency>::ReleaseStruct(dependencies[i]); }
                                        delete[] dependencies;
                                        break;
                                    }
                                }
                                if (SUCCEEDED(hr))
                                {
                                    m_PackageDependencies = new VectorView<struct AppxPackageDependency>(dependencies, count);
                                    if (!m_PackageDependencies)
                                    {
                                        for (UINT32 i = 0; i < completed; ++i)
                                        { StructLifetimeFunctions<struct AppxPackageDependency>::ReleaseStruct(dependencies[i]); }
                                        delete[] dependencies;
                                        hr = E_OUTOFMEMORY;
                                    }
                                }
                            }
                            else
                            { hr = E_OUTOFMEMORY; }
                            enumerator->Release();
                        }
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
            local = reinterpret_cast<VectorView<struct AppxPackageDependency>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_PackageDependencies), nullptr, nullptr));
        }

        local->AddRef();
        *value = local;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_Resources(ABI::IVectorView<struct AppxPackageResource>** value)
    {
        auto local{ reinterpret_cast<VectorView<struct AppxPackageResource>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Resources), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_Resources)
            {
                IAppxManifestReader* reader{ nullptr };
                hr = GetManifestReader(reader);
                if (SUCCEEDED(hr))
                {
                    struct AppxPackageResource* resources{ nullptr };
                    UINT32 count{ 0 };
                    UINT32 completed{ 0 };
                    IAppxManifestReader2* reader2{ nullptr };
                    hr = reader->QueryInterface(__uuidof(reader2), to_void_pp(reader2));
                    if (SUCCEEDED(hr))
                    {
                        IAppxManifestQualifiedResourcesEnumerator* enumerator{ nullptr };
                        hr = reader2->GetQualifiedResources(&enumerator);
                        if (SUCCEEDED(hr))
                        {
                            BOOL hasNext{ false };
                            enumerator->GetHasCurrent(&hasNext);
                            while (hasNext)
                            {
                                ++count;
                                enumerator->MoveNext(&hasNext);
                            }
                            enumerator->Release();
                            hr = reader2->GetQualifiedResources(&enumerator);
                            reader2->Release();
                            if (SUCCEEDED(hr))
                            {
                                resources = new struct AppxPackageResource[count]{};
                                if (resources)
                                {
                                    enumerator->GetHasCurrent(&hasNext);
                                    while (hasNext)
                                    {
                                        auto& res{ resources[completed] };
                                        IAppxManifestQualifiedResource* resource{ nullptr };
                                        hr = enumerator->GetCurrent(&resource);
                                        if (SUCCEEDED(hr))
                                        {
                                            HRESULT hr1{ S_OK };
                                            HRESULT hr2{ S_OK };
                                            HRESULT hr3{ S_OK };
                                            bool hasLanguage{ false };
                                            LPWSTR language{ nullptr };
                                            hr1 = resource->GetLanguage(&language);
                                            if (SUCCEEDED(hr1))
                                            {
                                                UINT32 count{ WStringLength(language) };
                                                hasLanguage = count;
                                                hr1 = WindowsCreateString(language, count, &res.Language);
                                                CoTaskMemFree(language);
                                            }
                                            hr2 = resource->GetScale(&res.Scale);
                                            hr3 = resource->GetDXFeatureLevel(reinterpret_cast<DX_FEATURE_LEVEL*>(&res.DirectXFeatureLevel));
                                            if (SUCCEEDED(hr1) || SUCCEEDED(hr2) || SUCCEEDED(hr3))
                                            {
                                                if (SUCCEEDED(hr1) && hasLanguage)
                                                { res.Type = AppxPackageResourceType::Language; }
                                                else if (SUCCEEDED(hr2) && res.Scale)
                                                { res.Type = AppxPackageResourceType::Scale; }
                                                else if (SUCCEEDED(hr3) && res.DirectXFeatureLevel != DirectXFeatureLevel::Unspecified)
                                                { res.Type = AppxPackageResourceType::DirectXFeatureLevel; }
                                                ++completed;
                                                enumerator->MoveNext(&hasNext);
                                            }
                                            else
                                            { hr = hr1 == E_OUTOFMEMORY ? E_OUTOFMEMORY : HRESULT_FROM_WIN32(ERROR_RESOURCE_NOT_AVAILABLE); }
                                        }
                                        if (FAILED(hr))
                                        { break; }
                                    }
                                }
                                else
                                { hr = E_OUTOFMEMORY; }
                                enumerator->Release();
                            }
                        }
                        else
                        { reader2->Release(); }
                    }
                    else
                    {
                        IAppxManifestResourcesEnumerator* enumerator{ nullptr };
                        hr = reader->GetResources(&enumerator);
                        if (SUCCEEDED(hr))
                        {
                            BOOL hasNext{ false };
                            enumerator->GetHasCurrent(&hasNext);
                            while (hasNext)
                            {
                                ++count;
                                enumerator->MoveNext(&hasNext);
                            }
                            enumerator->Release();
                            hr = reader->GetResources(&enumerator);
                            if (SUCCEEDED(hr))
                            {
                                resources = new struct AppxPackageResource[count]{};
                                if (resources)
                                {
                                    enumerator->GetHasCurrent(&hasNext);
                                    while (hasNext)
                                    {
                                        auto& resource{ resources[completed] };
                                        LPWSTR language{ nullptr };
                                        hr = enumerator->GetCurrent(&language);
                                        if (SUCCEEDED(hr))
                                        {
                                            hr = WindowsCreateString(language, WStringLength(language), &resource.Language);
                                            CoTaskMemFree(language);
                                            if (SUCCEEDED(hr))
                                            {
                                                resource.Type = AppxPackageResourceType::Language;
                                                ++completed;
                                                enumerator->MoveNext(&hasNext);
                                            }
                                        }
                                        if (FAILED(hr))
                                        { break; }
                                    }
                                }
                                else
                                { hr = E_OUTOFMEMORY; }
                                enumerator->Release();
                            }
                        }
                    }
                    if (SUCCEEDED(hr))
                    {
                        m_Resources = new VectorView<struct AppxPackageResource>(resources, count);
                        if (!m_Resources)
                        {
                            for (UINT32 i = 0; i < count; ++i)
                            { WindowsDeleteString(resources[i].Language); }
                            delete[] resources;
                            hr = E_OUTOFMEMORY;
                        }
                    }
                    else if (resources)
                    {
                        for (UINT32 i = 0; i < completed; ++i)
                        { WindowsDeleteString(resources[i].Language); }
                        delete[] resources;
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
            local = reinterpret_cast<VectorView<struct AppxPackageResource>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Resources), nullptr, nullptr));
        }

        local->AddRef();
        *value = local;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_DeviceCapabilities(ABI::IVectorView<HSTRING>** value)
    {
        auto local{ reinterpret_cast<VectorView<HSTRING>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_DeviceCapabilities), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_DeviceCapabilities)
            {
                IAppxManifestReader* reader{ nullptr };
                hr = GetManifestReader(reader);
                if (SUCCEEDED(hr))
                {
                    IAppxManifestDeviceCapabilitiesEnumerator* enumerator{ nullptr };
                    hr = reader->GetDeviceCapabilities(&enumerator);
                    if (SUCCEEDED(hr))
                    {
                        UINT32 count{ 0 };
                        BOOL hasNext{ false };
                        enumerator->GetHasCurrent(&hasNext);
                        while (hasNext)
                        {
                            ++count;
                            enumerator->MoveNext(&hasNext);
                        }
                        enumerator->Release();
                        hr = reader->GetDeviceCapabilities(&enumerator);
                        if (SUCCEEDED(hr))
                        {
                            HSTRING* capabilities{ new HSTRING[count]{} };
                            if (capabilities)
                            {
                                enumerator->GetHasCurrent(&hasNext);
                                UINT32 completed{ 0 };
                                while (hasNext)
                                {
                                    LPWSTR name{ nullptr };
                                    hr = enumerator->GetCurrent(&name);
                                    if (SUCCEEDED(hr))
                                    {
                                        hr = WindowsCreateString(name, WStringLength(name), capabilities + completed);
                                        CoTaskMemFree(name);
                                        if (SUCCEEDED(hr))
                                        {
                                            ++completed;
                                            enumerator->MoveNext(&hasNext);
                                        }
                                    }
                                    if (FAILED(hr))
                                    {
                                        for (UINT32 i = 0; i < completed; ++i)
                                        { WindowsDeleteString(capabilities[i]); }
                                        delete[] capabilities;
                                        break;
                                    }
                                }
                                if (SUCCEEDED(hr))
                                {
                                    m_DeviceCapabilities = new ABI::VectorView<HSTRING>(capabilities, count);
                                    if (!m_DeviceCapabilities)
                                    {
                                        for (UINT32 i = 0; i < completed; ++i)
                                        { WindowsDeleteString(capabilities[i]); }
                                        delete[] capabilities;
                                        hr = E_OUTOFMEMORY;
                                    }
                                }
                            }
                            else
                            { hr = E_OUTOFMEMORY; }
                            enumerator->Release();
                        }
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
            local = reinterpret_cast<VectorView<HSTRING>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_DeviceCapabilities), nullptr, nullptr));
        }

        local->AddRef();
        *value = local;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::GetManifestStream(ABI::IInputStream** result)
    {
        HRESULT hr{ S_OK };
        auto local{ reinterpret_cast<ABI::IRandomAccessStream*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_ManifestStream), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            if (!m_ManifestStream)
            {
                IAppxManifestReader* reader{ nullptr };
                hr = GetManifestReader(reader);
                if (SUCCEEDED(hr))
                {
                    IStream* stream{ nullptr };
                    hr = reader->GetStream(&stream);
                    if (SUCCEEDED(hr))
                    {
                        hr = CreateRandomAccessStreamOverStream(stream, BSOS_DEFAULT, __uuidof(m_ManifestStream), to_void_pp(m_ManifestStream));
                        stream->Release();
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
            local = reinterpret_cast<ABI::IRandomAccessStream*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_ManifestStream), nullptr, nullptr));
        }

        ABI::IRandomAccessStream* clonedStream{ nullptr };
        hr = local->CloneStream(&clonedStream);
        if (SUCCEEDED(hr))
        {
            hr = clonedStream->QueryInterface(__uuidof(*result), to_void_pp(*result));
            clonedStream->Release();
        }
        return hr;
    }

    //IAppxPackagePayloadFilesReader
    HRESULT STDMETHODCALLTYPE AppxPackage::GetPayloadFiles(ABI::IMapView<HSTRING, AppxPackagePayloadFile*>** result)
    {
        auto local{ reinterpret_cast<ABI::IMapView<HSTRING, AppxPackagePayloadFile*>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_PayloadFiles), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_PayloadFiles)
            {
                hr = E_NOTIMPL;
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
            local = reinterpret_cast<ABI::IMapView<HSTRING, AppxPackagePayloadFile*>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_PayloadFiles), nullptr, nullptr));
        }

        local->AddRef();
        *result = local;
        return S_OK;
    }

    //IAppxPackage
    HRESULT STDMETHODCALLTYPE AppxPackage::get_TargetDeviceFamilies(ABI::IVectorView<struct AppxPackageTargetDeviceFamily>** value)
    {
        auto local{ reinterpret_cast<VectorView<struct AppxPackageTargetDeviceFamily>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_TargetDeviceFamilies), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_TargetDeviceFamilies)
            {
                IAppxManifestReader* reader{ nullptr };
                hr = GetManifestReader(reader);
                if (SUCCEEDED(hr))
                {
                    IAppxManifestReader3* reader3{ nullptr };
                    hr = reader->QueryInterface(__uuidof(reader3), to_void_pp(reader3));
                    if (SUCCEEDED(hr))
                    {
                        IAppxManifestTargetDeviceFamiliesEnumerator* enumerator{ nullptr };
                        hr = reader3->GetTargetDeviceFamilies(&enumerator);
                        if (SUCCEEDED(hr))
                        {
                            UINT32 count{ 0 };
                            BOOL hasNext{ false };
                            enumerator->GetHasCurrent(&hasNext);
                            while (hasNext)
                            {
                                ++count;
                                enumerator->MoveNext(&hasNext);
                            }
                            enumerator->Release();
                            hr = reader3->GetTargetDeviceFamilies(&enumerator);
                            if (SUCCEEDED(hr))
                            {
                                struct AppxPackageTargetDeviceFamily* families{ new struct AppxPackageTargetDeviceFamily[count]{} };
                                if (families)
                                {
                                    enumerator->GetHasCurrent(&hasNext);
                                    UINT32 completed{ 0 };
                                    while (hasNext)
                                    {
                                        IAppxManifestTargetDeviceFamily* element{ nullptr };
                                        hr = enumerator->GetCurrent(&element);
                                        if (SUCCEEDED(hr))
                                        {
                                            auto& family{ families[completed] };
                                            LPWSTR name{ nullptr };
                                            hr = element->GetName(&name);
                                            if (SUCCEEDED(hr))
                                            {
                                                hr = WindowsCreateString(name, WStringLength(name), &family.Name);
                                                CoTaskMemFree(name);
                                                if (SUCCEEDED(hr))
                                                {
                                                    UINT64 minVer{};
                                                    hr = element->GetMinVersion(&minVer);
                                                    if (SUCCEEDED(hr))
                                                    {
                                                        UINT64 maxVerTested{};
                                                        hr = element->GetMaxVersionTested(&maxVerTested);
                                                        if (SUCCEEDED(hr))
                                                        {
                                                            family.MinVersion = UInt64ToPkgVer(minVer);
                                                            family.MaxVersionTested = UInt64ToPkgVer(maxVerTested);
                                                            ++completed;
                                                            enumerator->MoveNext(&hasNext);
                                                        }
                                                    }

                                                    if (FAILED(hr))
                                                    { WindowsDeleteString(family.Name); }
                                                }
                                            }
                                            element->Release();
                                        }

                                        if (FAILED(hr))
                                        {
                                            for (UINT32 i{ 0 }; i < completed; ++i)
                                            { WindowsDeleteString(families[i].Name); }
                                            delete[] families;
                                            break;
                                        }
                                    }
                                    if (SUCCEEDED(hr))
                                    {
                                        m_TargetDeviceFamilies = new VectorView<struct AppxPackageTargetDeviceFamily>(families, count);
                                        if (!m_TargetDeviceFamilies)
                                        {
                                            for (UINT32 i{ 0 }; i < completed; ++i)
                                            { WindowsDeleteString(families[i].Name); }
                                            delete[] families;
                                            hr = E_OUTOFMEMORY;
                                        }
                                    }
                                }
                                else
                                { hr = E_OUTOFMEMORY; }
                            }
                        }
                        reader3->Release();
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
            local = reinterpret_cast<VectorView<struct AppxPackageTargetDeviceFamily>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_TargetDeviceFamilies), nullptr, nullptr));
        }

        local->AddRef();
        *value = local;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_Capabilities(ABI::IVectorView<HSTRING>** value)
    { return this->GetCapabilitiesByCapabilityClass(AppxPackageCapabilityClassType::Default, value); }

    HRESULT STDMETHODCALLTYPE AppxPackage::GetCapabilitiesByCapabilityClass(AppxPackageCapabilityClassType classType, ABI::IVectorView<HSTRING>** result)
    {
        VectorView<HSTRING>* local{ nullptr };
        switch (classType)
        {
            case AppxPackageCapabilityClassType::Default:
                local = reinterpret_cast<VectorView<HSTRING>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Capabilities), nullptr, nullptr));
                break;

            case AppxPackageCapabilityClassType::General:
                local = reinterpret_cast<VectorView<HSTRING>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_GeneralCapabilities), nullptr, nullptr));
                break;

            case AppxPackageCapabilityClassType::Restricted:
                local = reinterpret_cast<VectorView<HSTRING>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_RestrictedCapabilities), nullptr, nullptr));
                break;

            case AppxPackageCapabilityClassType::Windows:
                local = reinterpret_cast<VectorView<HSTRING>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_WindowsCapabilities), nullptr, nullptr));
                break;

            case AppxPackageCapabilityClassType::All:
                local = reinterpret_cast<VectorView<HSTRING>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_AllCapabilities), nullptr, nullptr));
                break;

            case AppxPackageCapabilityClassType::Custom:
                local = reinterpret_cast<VectorView<HSTRING>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_CustomCapabilities), nullptr, nullptr));
                break;

            default:
                return E_INVALIDARG;
        }

        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            switch (classType)
            {
                case AppxPackageCapabilityClassType::Default:
                    local = reinterpret_cast<VectorView<HSTRING>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Capabilities), nullptr, nullptr));
                    break;

                case AppxPackageCapabilityClassType::General:
                    local = reinterpret_cast<VectorView<HSTRING>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_GeneralCapabilities), nullptr, nullptr));
                   break;

                case AppxPackageCapabilityClassType::Restricted:
                    local = reinterpret_cast<VectorView<HSTRING>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_RestrictedCapabilities), nullptr, nullptr));
                    break;

                case AppxPackageCapabilityClassType::Windows:
                    local = reinterpret_cast<VectorView<HSTRING>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_WindowsCapabilities), nullptr, nullptr));
                    break;

                case AppxPackageCapabilityClassType::All:
                    local = reinterpret_cast<VectorView<HSTRING>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_AllCapabilities), nullptr, nullptr));
                    break;

                case AppxPackageCapabilityClassType::Custom:
                    local = reinterpret_cast<VectorView<HSTRING>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_CustomCapabilities), nullptr, nullptr));
                    break;
            }
            if (local == nullptr)
            {
                IAppxManifestReader* reader{ nullptr };
                hr = GetManifestReader(reader);
                if (SUCCEEDED(hr))
                {
                    IAppxManifestReader3* reader3{ nullptr };
                    hr = reader->QueryInterface(__uuidof(reader3), to_void_pp(reader3));
                    if (SUCCEEDED(hr))
                    {
                        IAppxManifestCapabilitiesEnumerator* enumerator{ nullptr };
                        hr = reader3->GetCapabilitiesByCapabilityClass(static_cast<APPX_CAPABILITY_CLASS_TYPE>(classType), &enumerator);
                        if (SUCCEEDED(hr))
                        {
                            UINT32 count{ 0 };
                            BOOL hasNext{ false };
                            enumerator->GetHasCurrent(&hasNext);
                            while (hasNext)
                            {
                                ++count;
                                enumerator->MoveNext(&hasNext);
                            }
                            enumerator->Release();
                            hr = reader3->GetCapabilitiesByCapabilityClass(static_cast<APPX_CAPABILITY_CLASS_TYPE>(classType), &enumerator);
                            if (SUCCEEDED(hr))
                            {
                                HSTRING* capabilities{ new HSTRING[count]{} };
                                if (capabilities)
                                {
                                    enumerator->GetHasCurrent(&hasNext);
                                    UINT32 completed{ 0 };
                                    while (hasNext)
                                    {
                                        LPWSTR element{ nullptr };
                                        hr = enumerator->GetCurrent(&element);
                                        if (SUCCEEDED(hr))
                                        {
                                            hr = WindowsCreateString(element, WStringLength(element), capabilities + completed);
                                            CoTaskMemFree(element);
                                            if (SUCCEEDED(hr))
                                            {
                                                ++completed;
                                                enumerator->MoveNext(&hasNext);
                                            }
                                        }
                                        if (FAILED(hr))
                                        {
                                            for (UINT32 i{ 0 }; i < completed; ++i)
                                            { WindowsDeleteString(capabilities[i]); }
                                            delete[] capabilities;
                                            break;
                                        }
                                    }
                                    if (SUCCEEDED(hr))
                                    {
                                        local = new VectorView<HSTRING>(capabilities, count);
                                        if (local)
                                        {
                                            switch (classType)
                                            {
                                                case AppxPackageCapabilityClassType::Default:
                                                    m_Capabilities = local;
                                                    break;

                                                case AppxPackageCapabilityClassType::General:
                                                    m_GeneralCapabilities = local;
                                                    break;

                                                case AppxPackageCapabilityClassType::Restricted:
                                                    m_RestrictedCapabilities = local;
                                                    break;

                                                case AppxPackageCapabilityClassType::Windows:
                                                    m_WindowsCapabilities = local;
                                                    break;

                                                case AppxPackageCapabilityClassType::All:
                                                    m_AllCapabilities = local;
                                                    break;

                                                case AppxPackageCapabilityClassType::Custom:
                                                    m_CustomCapabilities = local;
                                                    break;
                                            }
                                        }
                                        else
                                        {
                                            hr = E_OUTOFMEMORY;
                                            for (UINT32 i{ 0 }; i < completed; ++i)
                                            { WindowsDeleteString(capabilities[i]); }
                                            delete[] capabilities;
                                        }
                                    }
                                }
                                else
                                { hr = E_OUTOFMEMORY; }
                                enumerator->Release();
                            }
                        }
                        reader3->Release();
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
        }

        local->AddRef();
        *result = local;
        return S_OK;
    }

    //IAppxPackage3
    HRESULT STDMETHODCALLTYPE AppxPackage::get_IsOptionalPackage(boolean* value)
    {
        long local{ InterlockedCompareExchange(reinterpret_cast<long*>(&m_HasIsOptionalPackage), false, false) };
        if (local == false)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_HasIsOptionalPackage)
            {
                IAppxManifestReader* reader{ nullptr };
                hr = GetManifestReader(reader);
                if (SUCCEEDED(hr))
                {
                    IAppxManifestReader4* reader4{ nullptr };
                    hr = reader->QueryInterface(__uuidof(reader4), to_void_pp(reader4));
                    if (SUCCEEDED(hr))
                    {
                        IAppxManifestOptionalPackageInfo* info{ nullptr };
                        hr = reader4->GetOptionalPackageInfo(&info);
                        if (SUCCEEDED(hr))
                        {
                            BOOL isOptional{ false };
                            hr = info->GetIsOptionalPackage(&isOptional);
                            if (SUCCEEDED(hr))
                            {
                                m_IsOptionalPackage = isOptional;
                                m_HasIsOptionalPackage = true;
                            }
                            info->Release();
                        }
                        reader4->Release();
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
        }

        *value = m_IsOptionalPackage;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_MainPackageName(HSTRING* value)
    {
        HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_MainPackageName), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_MainPackageName)
            {
                IAppxManifestReader* reader{ nullptr };
                hr = GetManifestReader(reader);
                if (SUCCEEDED(hr))
                {
                    IAppxManifestReader4* reader4{ nullptr };
                    hr = reader->QueryInterface(__uuidof(reader4), to_void_pp(reader4));
                    if (SUCCEEDED(hr))
                    {
                        IAppxManifestOptionalPackageInfo* info{ nullptr };
                        hr = reader4->GetOptionalPackageInfo(&info);
                        if (SUCCEEDED(hr))
                        {
                            LPWSTR mainPkgName{ nullptr };
                            hr = info->GetMainPackageName(&mainPkgName);
                            if (SUCCEEDED(hr))
                            {
                                hr = WindowsCreateString(mainPkgName, WStringLength(mainPkgName), &m_MainPackageName);
                                CoTaskMemFree(mainPkgName);
                            }
                            info->Release();
                        }
                        reader4->Release();
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
            local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_MainPackageName), nullptr, nullptr));
        }

        return WindowsDuplicateString(local, value);
    }

    //IAppxPackage4
    HRESULT STDMETHODCALLTYPE AppxPackage::get_MainPackageDependencies(ABI::IVectorView<struct AppxPackageMainPackageDependency>** value)
    {
        auto local{ reinterpret_cast<VectorView<struct AppxPackageMainPackageDependency>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_MainPackageDependencies), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_MainPackageDependencies)
            {
                IAppxManifestReader* reader{ nullptr };
                hr = GetManifestReader(reader);
                if (SUCCEEDED(hr))
                {
                    IAppxManifestReader5* reader5{ nullptr };
                    hr = reader->QueryInterface(__uuidof(reader5), to_void_pp(reader5));
                    if (SUCCEEDED(hr))
                    {
                        IAppxManifestMainPackageDependenciesEnumerator* enumerator{ nullptr };
                        hr = reader5->GetMainPackageDependencies(&enumerator);
                        if (SUCCEEDED(hr))
                        {
                            UINT32 count{ 0 };
                            BOOL hasNext{ false };
                            enumerator->GetHasCurrent(&hasNext);
                            while (hasNext)
                            {
                                ++count;
                                enumerator->MoveNext(&hasNext);
                            }
                            enumerator->Release();
                            hr = reader5->GetMainPackageDependencies(&enumerator);
                            if (SUCCEEDED(hr))
                            {
                                struct AppxPackageMainPackageDependency* dependency{ new AppxPackageMainPackageDependency[count]{} };
                                if (dependency)
                                {
                                    enumerator->GetHasCurrent(&hasNext);
                                    UINT32 completed{ 0 };
                                    while (hasNext)
                                    {
                                        IAppxManifestMainPackageDependency* element{ nullptr };
                                        hr = enumerator->GetCurrent(&element);
                                        if (SUCCEEDED(hr))
                                        {
                                            auto& dep{ dependency[completed] };
                                            LPWSTR name{ nullptr };
                                            hr = element->GetName(&name);
                                            if (SUCCEEDED(hr))
                                            {
                                                LPWSTR pkgFamilyName{ nullptr };
                                                hr = element->GetPackageFamilyName(&pkgFamilyName);
                                                if (SUCCEEDED(hr))
                                                {
                                                    LPWSTR publisher{ nullptr };
                                                    hr = element->GetPublisher(&publisher);
                                                    if (SUCCEEDED(hr))
                                                    {
                                                        hr = WindowsCreateString(name, WStringLength(name), &dep.Name);
                                                        if (SUCCEEDED(hr))
                                                        {
                                                            hr = WindowsCreateString(pkgFamilyName, WStringLength(pkgFamilyName), &dep.FamilyName);
                                                            if (SUCCEEDED(hr))
                                                            {
                                                                hr = WindowsCreateString(publisher, WStringLength(publisher), &dep.Publisher);
                                                                if (SUCCEEDED(hr))
                                                                {
                                                                    ++completed;
                                                                    enumerator->MoveNext(&hasNext);
                                                                }
                                                                else
                                                                {
                                                                    WindowsDeleteString(dep.FamilyName);
                                                                    WindowsDeleteString(dep.Name);
                                                                }
                                                            }
                                                            else
                                                            { WindowsDeleteString(dep.Name); }
                                                        }
                                                        CoTaskMemFree(publisher);
                                                    }
                                                    CoTaskMemFree(pkgFamilyName);
                                                }
                                                CoTaskMemFree(name);
                                            }
                                            element->Release();
                                        }
                                        if (FAILED(hr))
                                        {
                                            for (UINT32 i{ 0 }; i < completed; ++i)
                                            { StructLifetimeFunctions<struct AppxPackageMainPackageDependency>::ReleaseStruct(dependency[i]); }
                                            delete[] dependency;
                                            break;
                                        }
                                    }
                                    if (SUCCEEDED(hr))
                                    {
                                        m_MainPackageDependencies = new VectorView<struct AppxPackageMainPackageDependency>(dependency, count);
                                        if (!m_MainPackageDependencies)
                                        {
                                            hr = E_OUTOFMEMORY;
                                            for (UINT32 i{ 0 }; i < completed; ++i)
                                            { StructLifetimeFunctions<struct AppxPackageMainPackageDependency>::ReleaseStruct(dependency[i]); }
                                            delete[] dependency;
                                        }
                                    }
                                }
                                else
                                { hr = E_OUTOFMEMORY; }
                                enumerator->Release();
                            }
                        }
                        reader5->Release();
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return S_OK; }
            local = reinterpret_cast<VectorView<struct AppxPackageMainPackageDependency>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_MainPackageDependencies), nullptr, nullptr));
        }

        local->AddRef();
        *value = local;
        return S_OK;
    }

    //IAppxPackage6
    HRESULT STDMETHODCALLTYPE AppxPackage::get_IsNonQualifiedResourcePackage(boolean* value)
    {
        long local{ InterlockedCompareExchange(reinterpret_cast<long*>(&m_HasIsNonQualifiedResourcePackage), false, false) };
        if (local == false)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_HasIsNonQualifiedResourcePackage)
            {
                IAppxManifestReader* reader{ nullptr };
                hr = GetManifestReader(reader);
                if (SUCCEEDED(hr))
                {
                    IAppxManifestReader6* reader6{ nullptr };
                    hr = reader->QueryInterface(__uuidof(reader6), to_void_pp(reader6));
                    if (SUCCEEDED(hr))
                    {
                        BOOL isNonQualifiedResourcePackage{ false };
                        hr = reader6->GetIsNonQualifiedResourcePackage(&isNonQualifiedResourcePackage);
                        if (SUCCEEDED(hr))
                        {
                            m_IsNonQualifiedResourcePackage = isNonQualifiedResourcePackage;
                            m_HasIsNonQualifiedResourcePackage = true;
                        }
                        reader6->Release();
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
        }

        *value = m_IsNonQualifiedResourcePackage;
        return S_OK;
    }

    //IAppxPackage10
    HRESULT STDMETHODCALLTYPE AppxPackage::get_DriverDependencies(ABI::IVectorView<AppxPackageDriverDependency*>** value)
    {
        return E_NOTIMPL;
    }
    
    HRESULT STDMETHODCALLTYPE AppxPackage::get_OSPackageDependencies(ABI::IVectorView<struct AppxPackageOSPackageDependency>** value)
    {
        auto local{ reinterpret_cast<VectorView<struct AppxPackageOSPackageDependency>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_OSPackageDependencies), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_OSPackageDependencies)
            {
                IAppxManifestReader* reader{ nullptr };
                hr = GetManifestReader(reader);
                if (SUCCEEDED(hr))
                {
                    IAppxManifestReader7* reader7{ nullptr };
                    hr = reader->QueryInterface(__uuidof(reader7), to_void_pp(reader7));
                    if (SUCCEEDED(hr))
                    {
                        IAppxManifestOSPackageDependenciesEnumerator* enumerator{ nullptr };
                        hr = reader7->GetOSPackageDependencies(&enumerator);
                        if (SUCCEEDED(hr))
                        {
                            UINT32 count{ 0 };
                            BOOL hasNext{ false };
                            enumerator->GetHasCurrent(&hasNext);
                            while (hasNext)
                            {
                                ++count;
                                enumerator->MoveNext(&hasNext);
                            }
                            enumerator->Release();
                            hr = reader7->GetOSPackageDependencies(&enumerator);
                            if (SUCCEEDED(hr))
                            {
                                struct AppxPackageOSPackageDependency* dependencies{ new AppxPackageOSPackageDependency[count]{} };
                                if (dependencies)
                                {
                                    enumerator->GetHasCurrent(&hasNext);
                                    UINT32 completed{ 0 };
                                    while (hasNext)
                                    {
                                        IAppxManifestOSPackageDependency* element{ nullptr };
                                        hr = enumerator->GetCurrent(&element);
                                        if (SUCCEEDED(hr))
                                        {
                                            auto& dep{ dependencies[completed] };
                                            LPWSTR name{ nullptr };
                                            hr = element->GetName(&name);
                                            if (SUCCEEDED(hr))
                                            {
                                                hr = WindowsCreateString(name, WStringLength(name), &dep.Name);
                                                CoTaskMemFree(name);
                                                if (SUCCEEDED(hr))
                                                {
                                                    UINT64 ver{};
                                                    hr = element->GetVersion(&ver);
                                                    if (SUCCEEDED(hr))
                                                    {
                                                        dep.Version = UInt64ToPkgVer(ver);
                                                        ++completed;
                                                        enumerator->MoveNext(&hasNext);
                                                    }
                                                    else
                                                    { WindowsDeleteString(dep.Name); }
                                                }
                                            }
                                            element->Release();
                                        }
                                        if (FAILED(hr))
                                        {
                                            for (UINT32 i{ 0 }; i < completed; ++i)
                                            { WindowsDeleteString(dependencies[i].Name); }
                                            delete[] dependencies;
                                            break;
                                        }
                                    }
                                    if (SUCCEEDED(hr))
                                    {
                                        m_OSPackageDependencies = new VectorView<struct AppxPackageOSPackageDependency>(dependencies, count);
                                        if (!m_OSPackageDependencies)
                                        {
                                            hr = E_OUTOFMEMORY;
                                            for (UINT32 i{ 0 }; i < completed; ++i)
                                            { WindowsDeleteString(dependencies[i].Name); }
                                            delete[] dependencies;
                                        }
                                    }
                                }
                                else
                                { hr = E_OUTOFMEMORY; }
                                enumerator->Release();
                            }
                        }
                        reader7->Release();
                    }
                }
            }
            local = m_OSPackageDependencies;
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
        }
        
        local->AddRef();
        *value = local;
        return S_OK;
    }
    
    HRESULT STDMETHODCALLTYPE AppxPackage::get_HostRuntimeDependencies(ABI::IVectorView<struct AppxPackageHostRuntimeDependency>** value)
    {
        auto local{ reinterpret_cast<VectorView<struct AppxPackageOSPackageDependency>*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_HostRuntimeDependencies), nullptr, nullptr)) };
        if (local == nullptr)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_HostRuntimeDependencies)
            {
                IAppxManifestReader* reader{ nullptr };
                hr = GetManifestReader(reader);
                if (SUCCEEDED(hr))
                {
                    IAppxManifestReader7* reader7{ nullptr };
                    hr = reader->QueryInterface(__uuidof(reader7), to_void_pp(reader7));
                    if (SUCCEEDED(hr))
                    {
                        IAppxManifestHostRuntimeDependenciesEnumerator* enumerator{ nullptr };
                        hr = reader7->GetHostRuntimeDependencies(&enumerator);
                        if (SUCCEEDED(hr))
                        {
                            UINT32 count{ 0 };
                            BOOL hasNext{ false };
                            enumerator->GetHasCurrent(&hasNext);
                            while (hasNext)
                            {
                                ++count;
                                enumerator->MoveNext(&hasNext);
                            }
                            enumerator->Release();
                            hr = reader7->GetHostRuntimeDependencies(&enumerator);
                            if (SUCCEEDED(hr))
                            {
                                struct AppxPackageHostRuntimeDependency* dependencies{ new AppxPackageHostRuntimeDependency[count]{} };
                                if (dependencies)
                                {
                                    enumerator->GetHasCurrent(&hasNext);
                                    UINT32 completed{ 0 };
                                    while (hasNext)
                                    {
                                        IAppxManifestHostRuntimeDependency* element{ nullptr };
                                        hr = enumerator->GetCurrent(&element);
                                        if (SUCCEEDED(hr))
                                        {
                                            auto& dep{ dependencies[completed] };
                                            LPWSTR name{ nullptr };
                                            hr = element->GetName(&name);
                                            if (SUCCEEDED(hr))
                                            {
                                                LPWSTR publisher{ nullptr };
                                                hr = element->GetPublisher(&publisher);
                                                if (SUCCEEDED(hr))
                                                {
                                                    hr = WindowsCreateString(name, WStringLength(name), &dep.Name);
                                                    if (SUCCEEDED(hr))
                                                    {
                                                        hr = WindowsCreateString(publisher, WStringLength(publisher), &dep.Publisher);
                                                        if (SUCCEEDED(hr))
                                                        {
                                                            UINT64 minVer{};
                                                            hr = element->GetMinVersion(&minVer);
                                                            if (SUCCEEDED(hr))
                                                            {
                                                                dep.MinVersion = UInt64ToPkgVer(minVer);
                                                                ++completed;
                                                                enumerator->MoveNext(&hasNext);
                                                            }
                                                            else
                                                            {
                                                                WindowsDeleteString(dep.Publisher);
                                                                WindowsDeleteString(dep.Name);
                                                            }
                                                        }
                                                        else
                                                        { WindowsDeleteString(dep.Name); }
                                                    }
                                                    CoTaskMemFree(publisher);
                                                }
                                                CoTaskMemFree(name);
                                            }
                                            element->Release();
                                        }
                                        if (FAILED(hr))
                                        {
                                            for (UINT32 i{ 0 }; i < completed; ++i)
                                            { StructLifetimeFunctions<struct AppxPackageHostRuntimeDependency>::ReleaseStruct(dependencies[i]); }
                                            delete[] dependencies;
                                            break;
                                        }
                                    }
                                    if (SUCCEEDED(hr))
                                    {
                                        m_HostRuntimeDependencies = new VectorView<struct AppxPackageHostRuntimeDependency>(dependencies, count);
                                        if (!m_HostRuntimeDependencies)
                                        {
                                            hr = E_OUTOFMEMORY;
                                            for (UINT32 i{ 0 }; i < completed; ++i)
                                            { StructLifetimeFunctions<struct AppxPackageHostRuntimeDependency>::ReleaseStruct(dependencies[i]); }
                                            delete[] dependencies;
                                        }
                                    }
                                }
                                else
                                { hr = E_OUTOFMEMORY; }
                                enumerator->Release();
                            }
                        }
                        reader7->Release();
                    }
                }
            }
            local = m_HostRuntimeDependencies;
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
        }
        
        local->AddRef();
        *value = local;
        return S_OK;
    }

    //IAppxPackageLegacy
    HRESULT STDMETHODCALLTYPE AppxPackage::get_MinVersionLegacy(struct ABI::PackageVersion* value)
    {
        long local{ InterlockedCompareExchange(reinterpret_cast<long*>(&m_HasMinVersionLegacy), false, false) };
        if (local == false)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_HasMinVersionLegacy)
            {
                IAppxManifestReader* reader{ nullptr };
                hr = GetManifestReader(reader);
                if (SUCCEEDED(hr))
                {
                    UINT64 osMinVersion{};
                    hr = reader->GetPrerequisite(L"OSMinVersion", &osMinVersion);
                    if (SUCCEEDED(hr))
                    {
                        m_MinVersionLegacy = UInt64ToPkgVer(osMinVersion);
                        m_HasMinVersionLegacy = true;
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
        }

        *value = m_MinVersionLegacy;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_MaxVersionTestedLegacy(struct ABI::PackageVersion* value)
    {
        long local{ InterlockedCompareExchange(reinterpret_cast<long*>(&m_HasMaxVersionTestedLegacy), false, false) };
        if (local == false)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_HasMaxVersionTestedLegacy)
            {
                IAppxManifestReader* reader{ nullptr };
                hr = GetManifestReader(reader);
                if (SUCCEEDED(hr))
                {
                    UINT64 osMaxVersionTested{};
                    hr = reader->GetPrerequisite(L"OSMaxVersionTested", &osMaxVersionTested);
                    if (SUCCEEDED(hr))
                    {
                        m_MaxVersionTestedLegacy = UInt64ToPkgVer(osMaxVersionTested);
                        m_HasMaxVersionTestedLegacy = true;
                    }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
        }

        *value = m_MaxVersionTestedLegacy;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_CapabilitiesLegacy(AppxPackageCapabilitiesLegacy* value)
    {
        long local{ InterlockedCompareExchange(reinterpret_cast<long*>(&m_HasCapabilitiesLegacy), false, false) };
        if (local == false)
        {
            EnterCriticalSection(&m_CriticalSection);
            HRESULT hr{ S_OK };
            if (!m_HasCapabilitiesLegacy)
            {
                IAppxManifestReader* reader{ nullptr };
                hr = GetManifestReader(reader);
                if (SUCCEEDED(hr))
                {
                    hr = reader->GetCapabilities(reinterpret_cast<APPX_CAPABILITIES*>(&m_CapabilitiesLegacy));
                    if (SUCCEEDED(hr))
                    { m_HasCapabilitiesLegacy = true; }
                }
            }
            LeaveCriticalSection(&m_CriticalSection);
            if (FAILED(hr))
            { return hr; }
        }

        *value = m_CapabilitiesLegacy;
        return S_OK;
    }

    //Private methods
    HRESULT STDMETHODCALLTYPE AppxPackage::GetManifestReader(IAppxManifestReader*& manifestReader)
    {
        if (!m_ManifestReader)
        {
            HRESULT hr{ m_AppxPackageReader->GetManifest(&m_ManifestReader) };
            if (FAILED(hr))
            { return hr; }
        }
        manifestReader = m_ManifestReader;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::GetPackageId(IAppxManifestPackageId*& pkgId)
    {
        if (!m_PackageId)
        {
            IAppxManifestReader* reader{ nullptr };
            HRESULT hr{ m_AppxPackageReader->GetManifest(&reader) };
            if (SUCCEEDED(hr))
            {
                hr = reader->GetPackageId(&m_PackageId);
                reader->Release();
            }

            if (FAILED(hr))
            { return hr; }
        }
        pkgId = m_PackageId;
        return S_OK;
    }

    //IAppxPackageInterop
    HRESULT STDMETHODCALLTYPE AppxPackage::get_PackageReader(IAppxPackageReader** value)
    {
        m_AppxPackageReader->AddRef();
        *value = m_AppxPackageReader;
        return S_OK;
    }

    //IInspectable
    HRESULT STDMETHODCALLTYPE AppxPackage::GetRuntimeClassName(HSTRING* className)
    { return WindowsCreateString(L"AppxUtils.AppxPackage", 21, className); }

    //Destructor
    AppxPackage::~AppxPackage() noexcept
    {
        if (m_ManifestReader)
        { m_ManifestReader->Release(); }
        if (m_PackageId)
        { m_PackageId->Release(); }
        m_AppxPackageReader->Release();
        if (m_Name)
        { WindowsDeleteString(m_Name); }
        if (m_FamilyName)
        { WindowsDeleteString(m_FamilyName); }
        if (m_FullName)
        { WindowsDeleteString(m_FullName); }
        if (m_Publisher)
        { WindowsDeleteString(m_Publisher); }
        if (m_ResourceId)
        { WindowsDeleteString(m_ResourceId); }
        if (m_Logo)
        { WindowsDeleteString(m_Logo); }
        if (m_DisplayName)
        { WindowsDeleteString(m_DisplayName); }
        if (m_PublisherDisplayName)
        { WindowsDeleteString(m_PublisherDisplayName); }
        if (m_Description)
        { WindowsDeleteString(m_Description); }
        if (m_PackageDependencies)
        { m_PackageDependencies->Release(); }
        if (m_Resources)
        { m_Resources->Release(); }
        if (m_DeviceCapabilities)
        { m_DeviceCapabilities->Release(); }
        if (m_ManifestStream)
        {
            ABI::IClosable* closable{ nullptr };
            m_ManifestStream->QueryInterface(__uuidof(closable), to_void_pp(closable));
            closable->Close();
            closable->Release();
            m_ManifestStream->Release();
        }

        if (m_PayloadFiles)
        { m_PayloadFiles->Release(); }

        if (m_TargetDeviceFamilies)
        { m_TargetDeviceFamilies->Release(); }
        if (m_Capabilities)
        { m_Capabilities->Release(); }
        if (m_GeneralCapabilities)
        { m_GeneralCapabilities->Release(); }
        if (m_RestrictedCapabilities)
        { m_RestrictedCapabilities->Release(); }
        if (m_WindowsCapabilities)
        { m_WindowsCapabilities->Release(); }
        if (m_AllCapabilities)
        { m_AllCapabilities->Release(); }
        if (m_CustomCapabilities)
        { m_CustomCapabilities->Release(); }

        if (m_MainPackageName)
        { WindowsDeleteString(m_MainPackageName); }

        if (m_MainPackageDependencies)
        { m_MainPackageDependencies->Release(); }
        
        if (m_OSPackageDependencies)
        { m_OSPackageDependencies->Release(); }
        if (m_HostRuntimeDependencies)
        { m_HostRuntimeDependencies->Release(); }

        DeleteCriticalSection(&m_CriticalSection);
    }
}
