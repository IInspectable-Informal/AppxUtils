#include "pch.h"
#include "AppxPackage.h"
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
    constexpr inline UINT32 WStringLength(LPCWSTR wstr)
    {
        UINT32 count{ 0 };
        if (wstr)
        {
            while (*(wstr + count))
            { ++count; }
        }
        return count;
    }
    
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
        return WindowsDuplicateString(m_Name, value);
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
        return WindowsDuplicateString(m_FullName, value);
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
        return WindowsDuplicateString(m_Publisher, value);
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_Version(struct ABI::PackageVersion* value)
    {
        if (!m_HasVersion)
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
        if (!m_HasArchitecture)
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
        if (!m_HasPackageType)
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
        if (!m_ResourceId)
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
        }
        return WindowsDuplicateString(m_ResourceId, value);
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_Logo(HSTRING* value)
    {
        if (!m_Logo)
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
        }
        return WindowsDuplicateString(m_Logo, value);
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_DisplayName(HSTRING* value)
    {
        if (!m_DisplayName)
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
        }
        return WindowsDuplicateString(m_DisplayName, value);
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_PublisherDisplayName(HSTRING* value)
    {
        if (!m_PublisherDisplayName)
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
        }
        return WindowsDuplicateString(m_PublisherDisplayName, value);
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_Description(HSTRING* value)
    {
        if (!m_Description)
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
        }
        return WindowsDuplicateString(m_Description, value);
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_PackageDependencies(ABI::IVectorView<struct AppxPackageDependency>** value)
    {
        if (!m_PackageDependencies)
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
                            struct AppxPackageDependency* dependencies{ new struct AppxPackageDependency[count]{ nullptr } };
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
        }

        m_PackageDependencies->AddRef();
        *value = m_PackageDependencies;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_Resources(ABI::IVectorView<struct AppxPackageResource>** value)
    {
        if (!m_Resources)
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
                                            LPWSTR language{ nullptr };
                                            hr1 = resource->GetLanguage(&language);
                                            if (SUCCEEDED(hr1))
                                            {
                                                HSTRING languageHstr{ nullptr };
                                                hr1 = WindowsCreateString(language, WStringLength(language), &res.Language);
                                                CoTaskMemFree(language);
                                            }
                                            hr2 = resource->GetScale(&res.Scale);
                                            hr3 = resource->GetDXFeatureLevel(reinterpret_cast<DX_FEATURE_LEVEL*>(&res.DirectXFeatureLevel));
                                            if (SUCCEEDED(hr1) || SUCCEEDED(hr2) || SUCCEEDED(hr3))
                                            {
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
        }

        m_Resources->AddRef();
        *value = m_Resources;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_DeviceCapabilities(ABI::IVectorView<HSTRING>** value)
    {
        if (!m_DeviceCapabilities)
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
                            HSTRING* capabilities{ new HSTRING[count]{ nullptr } };
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
        }

        m_DeviceCapabilities->AddRef();
        *value = m_DeviceCapabilities;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::GetManifestStream(ABI::IInputStream** result)
    {
        HRESULT hr{ S_OK };
        if (!m_ManifestStream)
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
        }

        ABI::IRandomAccessStream* clonedStream{ nullptr };
        hr = m_ManifestStream->CloneStream(&clonedStream);
        if (SUCCEEDED(hr))
        {
            hr = clonedStream->QueryInterface(__uuidof(*result), to_void_pp(*result));
            clonedStream->Release();
            return hr;
        }
        else
        { return hr; }
    }

    //IAppxPackage
    HRESULT STDMETHODCALLTYPE AppxPackage::get_TargetDeviceFamilies(ABI::IVectorView<struct AppxPackageTargetDeviceFamily>** value)
    {
        if (!m_TargetDeviceFamilies)
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
        }

        m_TargetDeviceFamilies->AddRef();
        *value = m_TargetDeviceFamilies;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AppxPackage::get_Capabilities(ABI::IVectorView<HSTRING>** value)
    { return this->GetCapabilitiesByCapabilityClass(AppxPackageCapabilityClassType::Default, value); }

    HRESULT STDMETHODCALLTYPE AppxPackage::GetCapabilitiesByCapabilityClass(AppxPackageCapabilityClassType classType, ABI::IVectorView<HSTRING>** result)
    {
        return E_NOTIMPL;
    }

    //IAppxPackageLegacy
    HRESULT STDMETHODCALLTYPE AppxPackage::get_MinVersionLegacy(struct ABI::PackageVersion* value)
    {
        if (!m_HasMinVersionLegacy)
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
        if (!m_HasMaxVersionTestedLegacy)
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
        if (!m_HasCapabilitiesLegacy)
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

        if (m_TargetDeviceFamilies)
        { m_TargetDeviceFamilies->Release(); }
        if (m_Capabilities)
        { m_Capabilities->Release(); }

        DeleteCriticalSection(&m_CriticalSection);
    }
}
