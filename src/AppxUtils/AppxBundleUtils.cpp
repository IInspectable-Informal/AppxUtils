// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
#include "AppxPackage.h"
#include "AppxBundleUtils.h"
#include "helpers.hpp"
#include <stdio.h>

namespace ABI
{
	using namespace Windows::Foundation;
	using namespace Windows::Foundation::Collections;
	using namespace Windows::ApplicationModel;
	using namespace Windows::Storage;
	using namespace Windows::Storage::Streams;
	using namespace Windows::System;
	using namespace AppxUtils;
}

namespace ABI::AppxUtils::Internal
{
#pragma region AppxBundleGetPackagesAsyncOp
	AppxBundleGetPackagesAsyncOp::AppxBundleGetPackagesAsyncOp(DWORD& bundleReader) noexcept :
		m_BundleReader(bundleReader)
	{
		//bundleReader->AddRef();
	}

#pragma region This class methods
	HRESULT STDMETHODCALLTYPE AppxBundleGetPackagesAsyncOp::LaunchAsyncTask()
	{
		this->AddRef();
		if (TrySubmitThreadpoolCallback(InitPackagesCallback, this, nullptr))
		{ return S_OK; }
		else
		{
			this->Release();
			return HRESULT_FROM_WIN32(GetLastError());
		}
	}
#pragma endregion

#pragma region IAsyncInfo
	HRESULT STDMETHODCALLTYPE AppxBundleGetPackagesAsyncOp::Close()
	{
		HRESULT hr{ S_OK };
		AcquireSRWLockExclusive(&m_Lock);
		const bool opening{ m_IsOpening != 0 };
		if (static_cast<ABI::AsyncStatus>(m_Status) == ABI::AsyncStatus::Started)
		{ hr = E_ILLEGAL_METHOD_CALL; }
		else
		{ m_IsOpening = false; }
		auto* const result{ m_Result };
		m_Result = nullptr;
		ReleaseSRWLockExclusive(&m_Lock);
		if (hr == S_OK && opening && result)
		{ result->Release(); }
		return hr;
	}
#pragma endregion

	//Destructor
	AppxBundleGetPackagesAsyncOp::~AppxBundleGetPackagesAsyncOp() noexcept
	{
		//m_BundleReader->Release();
	}

#pragma region Static members
	void CALLBACK AppxBundleGetPackagesAsyncOp::InitPackagesCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context)
	{
		auto& external{ *static_cast<AppxBundleGetPackagesAsyncOp*>(Context) };
		ABI::IVectorView<ABI::AppxPackage*>* result{ nullptr };
		HRESULT hr{ CoInitializeEx(nullptr, COINIT_MULTITHREADED) };
		if (SUCCEEDED(hr))
		{
			if (external.get_CanContinue())
			{
				IGlobalInterfaceTable* pGIT{ nullptr };
				hr = CoCreateInstance(CLSID_StdGlobalInterfaceTable, nullptr, CLSCTX_INPROC_SERVER, __uuidof(pGIT), to_void_pp(pGIT));
				if (SUCCEEDED(hr))
				{
					IAppxBundleReader* bundleReader{ nullptr };
					hr = pGIT->GetInterfaceFromGlobal(external.m_BundleReader, __uuidof(bundleReader), to_void_pp(bundleReader));
					if (SUCCEEDED(hr))
					{
						IAppxFactory* factory{ nullptr };
						hr = CoCreateInstance(CLSID_AppxFactory, nullptr, CLSCTX_INPROC_SERVER, __uuidof(factory), to_void_pp(factory));
						if (SUCCEEDED(hr))
						{
							if (external.get_CanContinue())
							{
								IAppxFilesEnumerator* enumerator{ nullptr };
								hr = bundleReader->GetPayloadPackages(&enumerator);
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
									hr = bundleReader->GetPayloadPackages(&enumerator);
									if (SUCCEEDED(hr))
									{
										if (external.get_CanContinue())
										{
											auto* const packages{ static_cast<AppxPackage*>(::operator new[](__aligned_size_of<AppxPackage> * count)) };
											if (packages)
											{
												enumerator->GetHasCurrent(&hasNext);
												UINT32 completed{ 0 };
												while (hasNext)
												{
													if (external.get_CanContinue())
													{
														IAppxFile* element{ nullptr };
														hr = enumerator->GetCurrent(&element);
														if (SUCCEEDED(hr))
														{
															IStream* stream{ nullptr };
															hr = element->GetStream(&stream);
															if (SUCCEEDED(hr))
															{
																IAppxPackageReader* reader{ nullptr };
																hr = factory->CreatePackageReader(stream, &reader);
																if (SUCCEEDED(hr))
																{
																	new (packages + completed) AppxPackage{ reader };
																	++completed;
																	enumerator->MoveNext(&hasNext);
																}
																stream->Release();
															}
															element->Release();
														}
													}
													else
													{ hr = E_CANCELED; }
													if (FAILED(hr))
													{
														for (UINT32 i{ completed }; i > 0;)
														{ packages[--i].Release(); }
														printf("failed\n");
														::operator delete[](packages);
														break;
													}
												}
												printf("test\n");
												if (SUCCEEDED(hr))
												{
													const bool canContinue{ external.get_CanContinue() };
													auto* const instance{ canContinue ? new VectorView<AppxPackage*>{ packages, count } : nullptr };
													if (instance)
													{ result = instance; }
													else
													{
														hr = canContinue ? E_OUTOFMEMORY : E_CANCELED;
														for (UINT32 i{ completed }; i > 0;)
														{ packages[--i].Release(); }
														::operator delete[](packages);
													}
												}
											}
											else
											{ hr = E_OUTOFMEMORY; }
										}
										else
										{ hr = E_CANCELED; }
										enumerator->Release();
									}
								}
							}
							else
							{ hr = E_CANCELED; }
							factory->Release();
						}
						bundleReader->Release();
					}
					pGIT->RevokeInterfaceFromGlobal(external.m_BundleReader);
					pGIT->Release();
				}
			}
			else
			{ hr = E_CANCELED; }
			CoUninitialize();
		}
		LONG newStatus{};
		AcquireSRWLockShared(&external.m_Lock);
		if (result)
		{
			external.m_Result = result;
			external.m_ErrorCode = S_OK;
			newStatus = static_cast<LONG>(ABI::AsyncStatus::Completed);
		}
		else if (external.m_CanContinue)
		{
			external.m_ErrorCode = hr;
			newStatus = static_cast<LONG>(ABI::AsyncStatus::Error);
		}
		else
		{
			external.m_ErrorCode = E_CANCELED;
			newStatus = static_cast<LONG>(ABI::AsyncStatus::Canceled);
		}
		external.m_Status = newStatus;
		auto* const local{ external.m_Completed };
		ReleaseSRWLockShared(&external.m_Lock);
		if (local)
		{ local->Invoke(&external, static_cast<ABI::AsyncStatus>(newStatus)); }
		external.Release();
	}
#pragma endregion
#pragma endregion
}
