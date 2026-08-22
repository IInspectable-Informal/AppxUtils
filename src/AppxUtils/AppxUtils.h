#pragma once

namespace ABI::AppxUtils
{
    class AppxPackageFactory : public InspectableBase<BaseTrust, IAppxPackageFactory>
    {
    public:
        AppxPackageFactory();

        HRESULT STDMETHODCALLTYPE GetAppxPackageFromStream(ABI::Windows::Storage::Streams::IRandomAccessStream* appxPackageStream, IAppxPackageCore** result);

        //IInspectable
        HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className);

        ~AppxPackageFactory();

    private:
        IAppxFactory* m_AppxFactory{ nullptr };

        INIT_ONCE m_InitOnce{ INIT_ONCE_STATIC_INIT };

        static BOOL WINAPI StaticAppxFactoryInit(INIT_ONCE* InitOnce, void* Parameter, void** Context);
    };
}
