#pragma once

namespace ABI::AppxUtils
{
	class AppxBundle final : public InspectableBase<BaseTrust,
		IAppxBundleCore,
		ABI::Windows::Foundation::Collections::IVectorView<AppxPackage*>,
        ABI::Windows::Foundation::Collections::IIterable<AppxPackage*>,
        IAppxBundleInterop,
		IAgileObject
	>
	{
	public:
        AppxBundle(IAppxBundleReader*& reader, CRITICAL_SECTION* criticalSection) noexcept;

        HRESULT STDMETHODCALLTYPE get_Name(HSTRING* value);
        HRESULT STDMETHODCALLTYPE get_FamilyName(HSTRING* value);
        HRESULT STDMETHODCALLTYPE get_FullName(HSTRING* value);
        HRESULT STDMETHODCALLTYPE get_Publisher(HSTRING* value);
        HRESULT STDMETHODCALLTYPE get_Version(struct ABI::Windows::ApplicationModel::PackageVersion* value);
        HRESULT STDMETHODCALLTYPE get_Architecture(ABI::Windows::System::ProcessorArchitecture* value);
        HRESULT STDMETHODCALLTYPE get_ResourceId(HSTRING* value);

        HRESULT STDMETHODCALLTYPE GetManifestStream(ABI::Windows::Storage::Streams::IInputStream** result);

		//Windows.Foundation.Collections.IVectorView<AppxPackage>
        HRESULT STDMETHODCALLTYPE GetAt(UINT32 index, IAppxPackageCore** item);
        HRESULT STDMETHODCALLTYPE get_Size(UINT32* size);
        HRESULT STDMETHODCALLTYPE IndexOf(IAppxPackageCore* value, UINT32* index, boolean* found);
        HRESULT STDMETHODCALLTYPE GetMany(UINT32 startIndex, UINT32 capacity, IAppxPackageCore** value, UINT32* actual);

        //Windows.Foundation.Collections.IIterable<AppxPackage>
        HRESULT STDMETHODCALLTYPE First(ABI::Windows::Foundation::Collections::IIterator<AppxPackage*>** first);

        //IAppxBundleInterop
        HRESULT STDMETHODCALLTYPE get_BundleReader(IAppxBundleReader** value);

        //IInspectable
        HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className);

        ~AppxBundle() noexcept;

    private:
        HSTRING m_Name{ nullptr };
        HSTRING m_FamilyName{ nullptr };
        HSTRING m_FullName{ nullptr };
        HSTRING m_Publisher{ nullptr };
        short m_HasVersion{ false };
        short m_HasArchitecture{ false };
        struct ABI::Windows::ApplicationModel::PackageVersion m_Version{ 0, 0, 0, 0 };
        ABI::Windows::System::ProcessorArchitecture m_Architecture{ ABI::Windows::System::ProcessorArchitecture_Unknown };
        HSTRING m_ResourceId{ nullptr };
        ABI::Windows::Storage::Streams::IRandomAccessStream* m_ManifestStream{ nullptr };

        IAppxBundleReader* m_BundleReader{ nullptr };
        CRITICAL_SECTION* m_CriticalSection{ nullptr };
        IAppxBundleManifestReader* m_ManifestReader{ nullptr };
        IAppxManifestPackageId* m_BundleId{ nullptr };

        HRESULT STDMETHODCALLTYPE GetManifestReader(IAppxBundleManifestReader*& reader);
        HRESULT STDMETHODCALLTYPE GetBundleId(IAppxManifestPackageId*& bundleId);

        static BOOL WINAPI InitListStatic(INIT_ONCE* InitOnce, void* Parameter, void** Context);
    };
}
