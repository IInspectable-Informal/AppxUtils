#pragma once

namespace ABI::AppxUtils
{
    class AppxPackage final : public InspectableBase<BaseTrust,
        IAppxPackageCore, IAppxPackagePayloadFilesReader,
        IAppxPackage, IAppxPackage3, IAppxPackage4, IAppxPackage6, IAppxPackage10,
        IAppxPackageLegacy,
        IAppxPackageInterop,
        IAgileObject
    >
    {
    public:
        AppxPackage(IAppxPackageReader*& reader, const CRITICAL_SECTION& criticalSection) noexcept;

        HRESULT STDMETHODCALLTYPE get_Name(HSTRING* value);
        HRESULT STDMETHODCALLTYPE get_FamilyName(HSTRING* value);
        HRESULT STDMETHODCALLTYPE get_FullName(HSTRING* value);
        HRESULT STDMETHODCALLTYPE get_Publisher(HSTRING* value);
        HRESULT STDMETHODCALLTYPE get_Version(struct ABI::Windows::ApplicationModel::PackageVersion* value);
        HRESULT STDMETHODCALLTYPE get_Architecture(ABI::Windows::System::ProcessorArchitecture* value);
        HRESULT STDMETHODCALLTYPE get_PackageType(AppxPackageType* value);
        HRESULT STDMETHODCALLTYPE get_ResourceId(HSTRING* value);
        HRESULT STDMETHODCALLTYPE get_Logo(HSTRING* value);
        HRESULT STDMETHODCALLTYPE get_DisplayName(HSTRING* value);
        HRESULT STDMETHODCALLTYPE get_PublisherDisplayName(HSTRING* value);
        HRESULT STDMETHODCALLTYPE get_Description(HSTRING* value);
        HRESULT STDMETHODCALLTYPE get_Applications(ABI::Windows::Foundation::Collections::IVectorView<AppxPackageApplication*>** value);
        HRESULT STDMETHODCALLTYPE get_PackageDependencies(ABI::Windows::Foundation::Collections::IVectorView<struct AppxPackageDependency>** value);
        HRESULT STDMETHODCALLTYPE get_Resources(ABI::Windows::Foundation::Collections::IVectorView<struct AppxPackageResource>** value);
        HRESULT STDMETHODCALLTYPE get_DeviceCapabilities(ABI::Windows::Foundation::Collections::IVectorView<HSTRING>** value);

        HRESULT STDMETHODCALLTYPE GetManifestStream(ABI::Windows::Storage::Streams::IInputStream** result);

        //IAppxPackagePayloadFilesReader
        HRESULT STDMETHODCALLTYPE GetPayloadFiles(ABI::Windows::Foundation::Collections::IMapView<HSTRING, AppxPackagePayloadFile*>** result);

        //IAppxPackage
        HRESULT STDMETHODCALLTYPE get_TargetDeviceFamilies(ABI::Windows::Foundation::Collections::IVectorView<struct AppxPackageTargetDeviceFamily>** value);
        HRESULT STDMETHODCALLTYPE get_Capabilities(ABI::Windows::Foundation::Collections::IVectorView<HSTRING>** value);
        HRESULT STDMETHODCALLTYPE GetCapabilitiesByCapabilityClass(AppxPackageCapabilityClassType classType, ABI::Windows::Foundation::Collections::IVectorView<HSTRING>** result);

        //IAppxPackage3
        HRESULT STDMETHODCALLTYPE get_IsOptionalPackage(boolean* value);
        HRESULT STDMETHODCALLTYPE get_MainPackageName(HSTRING* value);

        //IAppxPackage4
        HRESULT STDMETHODCALLTYPE get_MainPackageDependencies(ABI::Windows::Foundation::Collections::IVectorView<struct AppxPackageMainPackageDependency>** value);

        //IAppxPackage6
        HRESULT STDMETHODCALLTYPE get_IsNonQualifiedResourcePackage(boolean* value);

        //IAppxPackage10
        HRESULT STDMETHODCALLTYPE get_DriverDependencies(ABI::Windows::Foundation::Collections::IVectorView<AppxPackageDriverDependency*>** value);
        HRESULT STDMETHODCALLTYPE get_OSPackageDependencies(ABI::Windows::Foundation::Collections::IVectorView<struct AppxPackageOSPackageDependency>** value);
        HRESULT STDMETHODCALLTYPE get_HostRuntimeDependencies(ABI::Windows::Foundation::Collections::IVectorView<struct AppxPackageHostRuntimeDependency>** value);

        //IAppxPackageLegacy
        HRESULT STDMETHODCALLTYPE get_MinVersionLegacy(struct ABI::Windows::ApplicationModel::PackageVersion* value);
        HRESULT STDMETHODCALLTYPE get_MaxVersionTestedLegacy(struct ABI::Windows::ApplicationModel::PackageVersion* value);
        HRESULT STDMETHODCALLTYPE get_CapabilitiesLegacy(AppxPackageCapabilitiesLegacy* value);

        //IAppxPackageInterop
        HRESULT STDMETHODCALLTYPE get_PackageReader(IAppxPackageReader** value);

        //IInspectable
        HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className);

        ~AppxPackage() noexcept;

    private:
        IAppxPackageReader* m_AppxPackageReader{ nullptr };

        HSTRING m_Name{ nullptr };
        HSTRING m_FamilyName{ nullptr };
        HSTRING m_FullName{ nullptr };
        HSTRING m_Publisher{ nullptr };
        BOOL m_HasVersion{ false };
        struct ABI::Windows::ApplicationModel::PackageVersion m_Version { 0, 0, 0, 0 };
        BOOL m_HasArchitecture{ false };
        ABI::Windows::System::ProcessorArchitecture m_Architecture{ ABI::Windows::System::ProcessorArchitecture_Unknown };
        BOOL m_HasPackageType{ false };
        AppxPackageType m_PackageType{ AppxPackageType::Main };
        HSTRING m_ResourceId{ nullptr };
        HSTRING m_Logo{ nullptr };
        HSTRING m_DisplayName{ nullptr };
        HSTRING m_PublisherDisplayName{ nullptr };
        HSTRING m_Description{ nullptr };
        ABI::AppxUtils::Internal::VectorView<AppxPackageApplication*>* m_Applications{ nullptr };
        ABI::AppxUtils::Internal::VectorView<struct AppxPackageDependency>* m_PackageDependencies{ nullptr };
        ABI::AppxUtils::Internal::VectorView<struct AppxPackageResource>* m_Resources{ nullptr };
        ABI::AppxUtils::Internal::VectorView<HSTRING>* m_DeviceCapabilities{ nullptr };
        ABI::Windows::Storage::Streams::IRandomAccessStream* m_ManifestStream{ nullptr };

        ABI::Windows::Foundation::Collections::IMapView<HSTRING, AppxPackagePayloadFile*>* m_PayloadFiles{ nullptr };

        ABI::AppxUtils::Internal::VectorView<struct AppxPackageTargetDeviceFamily>* m_TargetDeviceFamilies{ nullptr };
        ABI::AppxUtils::Internal::VectorView<HSTRING>* m_Capabilities{ nullptr };
        ABI::AppxUtils::Internal::VectorView<HSTRING>* m_GeneralCapabilities{ nullptr };
        ABI::AppxUtils::Internal::VectorView<HSTRING>* m_RestrictedCapabilities{ nullptr };
        ABI::AppxUtils::Internal::VectorView<HSTRING>* m_WindowsCapabilities{ nullptr };
        ABI::AppxUtils::Internal::VectorView<HSTRING>* m_AllCapabilities{ nullptr };
        ABI::AppxUtils::Internal::VectorView<HSTRING>* m_CustomCapabilities{ nullptr };

        BOOL m_HasIsOptionalPackage{ false };
        bool m_IsOptionalPackage{ false };
        HSTRING m_MainPackageName{ nullptr };

        ABI::AppxUtils::Internal::VectorView<struct AppxPackageMainPackageDependency>* m_MainPackageDependencies{ nullptr };

        BOOL m_HasIsNonQualifiedResourcePackage{ false };
        bool m_IsNonQualifiedResourcePackage{ false };
        
        ABI::AppxUtils::Internal::VectorView<AppxPackageDriverDependency*>* m_DriverDependencies{ nullptr };
        ABI::AppxUtils::Internal::VectorView<struct AppxPackageOSPackageDependency>* m_OSPackageDependencies{ nullptr };
        ABI::AppxUtils::Internal::VectorView<struct AppxPackageHostRuntimeDependency>* m_HostRuntimeDependencies{ nullptr };

        BOOL m_HasMinVersionLegacy{ false };
        struct ABI::Windows::ApplicationModel::PackageVersion m_MinVersionLegacy { 0, 0, 0, 0 };
        BOOL m_HasMaxVersionTestedLegacy{ false };
        struct ABI::Windows::ApplicationModel::PackageVersion m_MaxVersionTestedLegacy { 0, 0, 0, 0 };
        BOOL m_HasCapabilitiesLegacy{ false };
        AppxPackageCapabilitiesLegacy m_CapabilitiesLegacy{ AppxPackageCapabilitiesLegacy::NoCapability };

        CRITICAL_SECTION m_CriticalSection{};

        HRESULT STDMETHODCALLTYPE GetManifestReader(IAppxManifestReader*& manifestReader);
        HRESULT STDMETHODCALLTYPE GetPackageId(IAppxManifestPackageId*& pkgId);

        IAppxManifestReader* m_ManifestReader{ nullptr };
        IAppxManifestPackageId* m_PackageId{ nullptr };
    };
}
