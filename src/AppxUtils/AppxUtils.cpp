// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
#include "AppxUtils.h"
#include "AppxPackage.h"
#include "AppxBundle.h"
#include "helpers.hpp"
#include <stdio.h>

namespace ABI
{
	using namespace Windows::Foundation;
	using namespace Windows::Foundation::Collections;
	using namespace Windows::Storage::Streams;
}

namespace ABI::AppxUtils::Internal
{
	class AppxPackageFactoryGetAppxPackageFromStreamAsyncOp final : public AsyncOperation<AppxPackage*>
	{
	public:
		AppxPackageFactoryGetAppxPackageFromStreamAsyncOp(const DWORD appxPackageStream) noexcept :
			m_AppxPackageStream(appxPackageStream)
		{

		}

		HRESULT STDMETHODCALLTYPE LaunchAsyncTask()
		{
			this->AddRef();
			if (TrySubmitThreadpoolCallback(InitPackageCallback, this, nullptr))
			{ return S_OK; }
			else
			{
				this->Release();
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		//IAsyncInfo
		HRESULT STDMETHODCALLTYPE Close()
		{
			HRESULT hr{ S_OK };
			AcquireSRWLockExclusive(&m_Lock);
			if (static_cast<ABI::AsyncStatus>(m_Status) == ABI::AsyncStatus::Started)
			{ hr = E_ILLEGAL_METHOD_CALL; }
			ReleaseSRWLockExclusive(&m_Lock);
			return hr;
		}

		~AppxPackageFactoryGetAppxPackageFromStreamAsyncOp() noexcept = default;

	private:
		const DWORD m_AppxPackageStream{};

		static void CALLBACK InitPackageCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context)
		{
			auto& external{ *static_cast<AppxPackageFactoryGetAppxPackageFromStreamAsyncOp*>(Context) };
			HRESULT hr{ CoInitializeEx(nullptr, COINIT_MULTITHREADED) };
			IAppxPackageCore* result{ nullptr };
			if (SUCCEEDED(hr))
			{
				if (external.get_CanContinue())
				{
					IGlobalInterfaceTable* pGIT{ nullptr };
					hr = CoCreateInstance(CLSID_StdGlobalInterfaceTable, nullptr, CLSCTX_INPROC_SERVER, __uuidof(pGIT), to_void_pp(pGIT));
					if (SUCCEEDED(hr))
					{
						ABI::IRandomAccessStream* appxPackageStream{ nullptr };
						hr = pGIT->GetInterfaceFromGlobal(external.m_AppxPackageStream, __uuidof(appxPackageStream), to_void_pp(appxPackageStream));
						if (SUCCEEDED(hr))
						{
							IAppxFactory* factory{ nullptr };
							hr = CoCreateInstance(CLSID_AppxFactory, nullptr, CLSCTX_INPROC_SERVER, __uuidof(factory), to_void_pp(factory));
							if (SUCCEEDED(hr))
							{
								if (external.get_CanContinue())
								{
									IStream* stream{ nullptr };
									LARGE_INTEGER qpc1{};
									LARGE_INTEGER qpc2{};
									QueryPerformanceCounter(&qpc1);
									hr = CreateStreamOverRandomAccessStream(appxPackageStream, __uuidof(stream), to_void_pp(stream));
									QueryPerformanceCounter(&qpc2);
									printf("QPC: %lld\n", qpc1.QuadPart);
									printf("QPC: %lld\n", qpc2.QuadPart);
									if (SUCCEEDED(hr))
									{
										if (external.get_CanContinue())
										{
											IAppxPackageReader* reader{ nullptr };
											QueryPerformanceCounter(&qpc1);
											hr = factory->CreatePackageReader(stream, &reader);
											QueryPerformanceCounter(&qpc2);
											printf("QPC: %lld\n", qpc1.QuadPart);
											printf("QPC: %lld\n", qpc2.QuadPart);
											if (SUCCEEDED(hr))
											{
												const bool canContinue{ external.get_CanContinue()  };
												IAppxPackageCore* instance{ canContinue ? new AppxPackage{ reader } : nullptr };
												if (instance)
												{ result = instance; }
												else
												{
													reader->Release();
													hr = canContinue ? E_OUTOFMEMORY : E_CANCELED;
												}
											}
										}
										else
										{ hr = E_CANCELED; }
										stream->Release();
									}
								}
								else
								{ hr = E_CANCELED; }
								factory->Release();
							}
							appxPackageStream->Release();
						}
						pGIT->RevokeInterfaceFromGlobal(external.m_AppxPackageStream);
						pGIT->Release();
					}
				}
				else
				{ hr = E_CANCELED; }
				CoUninitialize();
			}
			LONG status{};
			AcquireSRWLockExclusive(&external.m_Lock);
			if (result)
			{
				external.m_Result = result;
				external.m_ErrorCode = S_OK;
				status = static_cast<LONG>(ABI::AsyncStatus::Completed);
			}
			else if (external.m_CanContinue)
			{
				external.m_ErrorCode = hr;
				status = static_cast<LONG>(ABI::AsyncStatus::Error);
			}
			else
			{
				external.m_ErrorCode = E_CANCELED;
				status = static_cast<LONG>(ABI::AsyncStatus::Canceled);
			}
			external.m_Status = status;
			auto* const handler{ external.m_Completed };
			ReleaseSRWLockExclusive(&external.m_Lock);
			if (handler)
			{ handler->Invoke(&external, static_cast<ABI::AsyncStatus>(status)); }
			external.Release();
		}
	};
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
				IStream* stream{ nullptr };
				LARGE_INTEGER qpc1{};
				LARGE_INTEGER qpc2{};
				QueryPerformanceCounter(&qpc1);
				hr = CreateStreamOverRandomAccessStream(appxPackageStream, __uuidof(stream), to_void_pp(stream));
				QueryPerformanceCounter(&qpc2);
				printf("QPC: %lld\n", qpc1.QuadPart);
				printf("QPC: %lld\n", qpc2.QuadPart);
				if (SUCCEEDED(hr))
				{
					IAppxPackageReader* reader{ nullptr };
					QueryPerformanceCounter(&qpc1);
					hr = m_AppxFactory->CreatePackageReader(stream, &reader);
					QueryPerformanceCounter(&qpc2);
					printf("QPC: %lld\n", qpc1.QuadPart);
					printf("QPC: %lld\n", qpc2.QuadPart);
					if (SUCCEEDED(hr))
					{
						IAppxPackageCore* instance{ new AppxPackage{ reader } };
						if (instance)
						{ *result = instance; }
						else
						{
							reader->Release();
							hr = E_OUTOFMEMORY;
						}
					}
					stream->Release();
				}
			}
			return hr;
		}
		else
		{ return E_INVALIDARG; }
	}

	HRESULT STDMETHODCALLTYPE AppxPackageFactory::GetAppxPackageFromStreamAsync(ABI::IRandomAccessStream* appxPackageStream, ABI::IAsyncOperation<AppxPackage*>** operation)
	{
		IGlobalInterfaceTable* pGIT{ nullptr };
		HRESULT hr{ CoCreateInstance(CLSID_StdGlobalInterfaceTable, nullptr, CLSCTX_INPROC_SERVER, __uuidof(pGIT), to_void_pp(pGIT)) };
		if (SUCCEEDED(hr))
		{
			DWORD cookie{};
			hr = pGIT->RegisterInterfaceInGlobal(appxPackageStream, __uuidof(appxPackageStream), &cookie);
			if (SUCCEEDED(hr))
			{
				auto* const instance{ new Internal::AppxPackageFactoryGetAppxPackageFromStreamAsyncOp{ cookie } };
				if (instance)
				{
					hr = instance->LaunchAsyncTask();
					if (SUCCEEDED(hr))
					{ *operation = instance; }
				}
				else
				{ hr = E_OUTOFMEMORY; }
				if (FAILED(hr))
				{ pGIT->RevokeInterfaceFromGlobal(cookie); }
			}
			pGIT->Release();
		}
		return hr;
	}

	HRESULT STDMETHODCALLTYPE AppxPackageFactory::GetAppxBundleFromStream(ABI::IRandomAccessStream* appxBundleStream, IAppxBundleCore** result)
	{
		if (appxBundleStream)
		{
			HRESULT hr{ S_OK };
			if (InitOnceExecuteOnce(&m_AppxBundleFactoryInitOnce, StaticAppxBundleFactoryInit, &m_AppxBundleFactory, reinterpret_cast<void**>(&hr)))
			{
				IStream* stream{ nullptr };
				LARGE_INTEGER qpc1{};
				LARGE_INTEGER qpc2{};
				QueryPerformanceCounter(&qpc1);
				hr = CreateStreamOverRandomAccessStream(appxBundleStream, __uuidof(stream), to_void_pp(stream));
				QueryPerformanceCounter(&qpc2);
				printf("QPC: %lld\n", qpc1.QuadPart);
				printf("QPC: %lld\n", qpc2.QuadPart);
				if (SUCCEEDED(hr))
				{
					IAppxBundleReader* reader{ nullptr };
					QueryPerformanceCounter(&qpc1);
					hr = m_AppxBundleFactory->CreateBundleReader(stream, &reader);
					QueryPerformanceCounter(&qpc2);
					printf("QPC: %lld\n", qpc1.QuadPart);
					printf("QPC: %lld\n", qpc2.QuadPart);
					if (SUCCEEDED(hr))
					{
						auto* instance{ new AppxBundle{ reader } };
						if (instance)
						{
							*result = instance;
							return S_OK;
						}
						else
						{
							reader->Release();
							stream->Release();
							hr = E_OUTOFMEMORY;
						}
					}
					else
					{ stream->Release(); }
				}
			}
			return hr;
		}
		else
		{ return E_INVALIDARG; }
	}

	//IActivationFactory
	HRESULT STDMETHODCALLTYPE AppxPackageFactory::ActivateInstance(IInspectable** instance)
	{ return E_NOTIMPL; }

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
