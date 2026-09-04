#pragma once

namespace ABI::AppxUtils
{
    class AppxPackageFactory : public InspectableBase<BaseTrust,
        IAppxPackageFactory, IAppxBundleRuntimeClassFactory,
        IAgileObject
    >
    {
    public:
        AppxPackageFactory();

        HRESULT STDMETHODCALLTYPE GetAppxPackageFromStream(ABI::Windows::Storage::Streams::IRandomAccessStream* appxPackageStream, IAppxPackageCore** result);
        HRESULT STDMETHODCALLTYPE GetAppxBundleFromStream(ABI::Windows::Storage::Streams::IRandomAccessStream* appxBundleStream, IAppxBundleCore** result);

        //IInspectable
        HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className);

        ~AppxPackageFactory();

    private:
        IAppxFactory* m_AppxFactory{ nullptr };
        ::IAppxBundleFactory* m_AppxBundleFactory{ nullptr };

        INIT_ONCE m_AppxFactoryInitOnce{ INIT_ONCE_STATIC_INIT };
        INIT_ONCE m_AppxBundleFactoryInitOnce{ INIT_ONCE_STATIC_INIT };

        static BOOL WINAPI StaticAppxFactoryInit(INIT_ONCE* InitOnce, void* Parameter, void** Context);
        static BOOL WINAPI StaticAppxBundleFactoryInit(INIT_ONCE* InitOnce, void* Parameter, void** Context);
    };
}
