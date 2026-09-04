#include "pch.h"
#include "AppxUtils.h"
#include "AppxPackage.h"
#include "AppxBundle.h"
#include "helpers.hpp"

namespace ABI
{
	using namespace Windows::Foundation;
	using namespace Windows::Foundation::Collections;
	using namespace Windows::Storage::Streams;
}

namespace ABI::AppxUtils
{
	//AppxPackageFactory
	//This member methods
	AppxPackageFactory::AppxPackageFactory()
	{

	}

	HRESULT STDMETHODCALLTYPE AppxPackageFactory::GetAppxPackageFromStream(ABI::IRandomAccessStream* appxPackageStream, IAppxPackageCore** result)
	{
		if (appxPackageStream)
		{
			HRESULT hr{ S_OK };
			if (InitOnceExecuteOnce(&m_AppxFactoryInitOnce, StaticAppxFactoryInit, &m_AppxFactory, reinterpret_cast<void**>(&hr)))
			{
				CRITICAL_SECTION* section{ new CRITICAL_SECTION{} };
				if (section)
				{
					if (InitializeCriticalSectionEx(section, 0, CRITICAL_SECTION_NO_DEBUG_INFO))
					{
						IStream* stream{ nullptr };
						hr = CreateStreamOverRandomAccessStream(appxPackageStream, __uuidof(stream), to_void_pp(stream));
						if (SUCCEEDED(hr))
						{
							IAppxPackageReader* reader{ nullptr };
							hr = m_AppxFactory->CreatePackageReader(stream, &reader);
							if (SUCCEEDED(hr))
							{
								IAppxPackageCore* instance{ new AppxPackage{ reader, section } };
								if (instance)
								{
									*result = instance;
									return S_OK;
								}
								else
								{
									reader->Release();
									DeleteCriticalSection(section);
									delete section;
									stream->Release();
									hr = E_OUTOFMEMORY;
								}
							}
							else
							{
								DeleteCriticalSection(section);
								delete section;
								stream->Release();
							}
						}
						else
						{
							DeleteCriticalSection(section);
							delete section;
						}
					}
					else
					{
						hr = HRESULT_FROM_WIN32(GetLastError());
						delete section;
					}
				}
				else
				{ hr = E_OUTOFMEMORY; }
			}
			else
			{ hr = HRESULT_FROM_WIN32(GetLastError()); }
			return hr;
		}
		else
		{ return E_INVALIDARG; }
	}

	HRESULT STDMETHODCALLTYPE AppxPackageFactory::GetAppxBundleFromStream(ABI::IRandomAccessStream* appxBundleStream, IAppxBundleCore** result)
	{
		if (appxBundleStream)
		{
			HRESULT hr{ S_OK };
			if (InitOnceExecuteOnce(&m_AppxBundleFactoryInitOnce, StaticAppxFactoryInit, &m_AppxBundleFactory, reinterpret_cast<void**>(&hr)))
			{
				CRITICAL_SECTION* section{ new CRITICAL_SECTION{} };
				if (section)
				{
					if (InitializeCriticalSectionEx(section, 0, CRITICAL_SECTION_NO_DEBUG_INFO))
					{
						IStream* stream{ nullptr };
						hr = CreateStreamOverRandomAccessStream(appxBundleStream, __uuidof(stream), to_void_pp(stream));
						if (SUCCEEDED(hr))
						{
							IAppxBundleReader* reader{ nullptr };
							hr = m_AppxBundleFactory->CreateBundleReader(stream, &reader);
							if (SUCCEEDED(hr))
							{
								auto* instance{ new AppxBundle{ reader, section } };
								if (instance)
								{
									*result = instance;
									return S_OK;
								}
								else
								{
									reader->Release();
									DeleteCriticalSection(section);
									delete section;
									stream->Release();
									hr = E_OUTOFMEMORY;
								}
							}
							else
							{
								DeleteCriticalSection(section);
								delete section;
								stream->Release();
							}
						}
						else
						{
							DeleteCriticalSection(section);
							delete section;
						}
					}
					else
					{
						hr = HRESULT_FROM_WIN32(GetLastError());
						delete section;
					}
				}
				else
				{ hr = E_OUTOFMEMORY; }
			}
			else
			{ hr = HRESULT_FROM_WIN32(GetLastError()); }
			return hr;
		}
		else
		{ return E_INVALIDARG; }
	}

	//IInspectable
	HRESULT STDMETHODCALLTYPE AppxPackageFactory::GetRuntimeClassName(HSTRING* className)
	{
		return E_ILLEGAL_METHOD_CALL;
	}

	//Destructor
	AppxPackageFactory::~AppxPackageFactory()
	{
		if (m_AppxFactory)
		{ m_AppxFactory->Release(); }
		if (m_AppxBundleFactory)
		{ m_AppxBundleFactory->Release(); }
	}

	//Static members
	BOOL WINAPI AppxPackageFactory::StaticAppxFactoryInit(INIT_ONCE* InitOnce, void* Parameter, void** Context)
	{
		HRESULT& hr{ *reinterpret_cast<HRESULT*>(Context) };
		hr = CoCreateInstance(CLSID_AppxFactory, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IAppxFactory), reinterpret_cast<void**>(Parameter));
		return SUCCEEDED(hr);
	}

	BOOL WINAPI AppxPackageFactory::StaticAppxBundleFactoryInit(INIT_ONCE* InitOnce, void* Parameter, void** Context)
	{
		HRESULT& hr{ *reinterpret_cast<HRESULT*>(Context) };
		hr = CoCreateInstance(CLSID_AppxBundleFactory, nullptr, CLSCTX_INPROC_SERVER, __uuidof(::IAppxBundleFactory), reinterpret_cast<void**>(Parameter));
		return SUCCEEDED(hr);
	}
}
