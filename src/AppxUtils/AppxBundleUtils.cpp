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
	class AppxPackageVectorView final : public VectorViewBase<ABI::AppxPackage*>
	{
	private:
		class InternalIterator final : public InternalIteratorBase
		{
		public:
			InternalIterator(AppxPackageElement* const list, const UINT32 size, AppxPackageVectorView* const vectorView) noexcept :
				InternalIteratorBase(size), m_Array(list), m_VectorView(vectorView)
			{
				vectorView->AddRef();
			}

			HRESULT STDMETHODCALLTYPE get_Current(T_ABI* current)
			{
				if (m_Current < m_Size)
				{
					auto* const element{ m_Array + m_Current };
					element->AddRef();
					*current = element;
					return S_OK;
				}
				else
				{ return E_BOUNDS; }
			}

			HRESULT STDMETHODCALLTYPE GetMany(UINT32 capacity, T_ABI* value, UINT32* actual)
			{
				UINT32 itemsGot{ 0 };
				for (; itemsGot < capacity && m_Current < m_Size; ++m_Current, ++itemsGot)
				{
					AppxPackageElement* const element{ m_Array + m_Current };
					element->AddRef();
					value[itemsGot] = element;
				}
				*actual = itemsGot;
				return S_OK;
			}

			//IInspectable
			HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className)
			{ return WindowsCreateString(L"Windows.Foundation.Collections.IIterator`1<AppxUtils.AppxPackage>", 65, className); }

			~InternalIterator()
			{
				m_VectorView->Release();
			}

		private:
			AppxPackageElement* const m_Array{ nullptr };
			AppxPackageVectorView* const m_VectorView{ nullptr };
		};

	public:
		AppxPackageVectorView(AppxPackageElement* const list, const UINT32 size) noexcept :
			m_Array(list), VectorViewBase(size)
		{

		}

		HRESULT STDMETHODCALLTYPE GetAt(UINT32 index, T_ABI* item)
		{
			if (index < m_Size)
			{
				auto* const element{ m_Array + index };
				element->AddRef();
				*item = element;
				return S_OK;
			}
			else
			{ return E_BOUNDS; }
		}

		HRESULT STDMETHODCALLTYPE IndexOf(T_ABI value, UINT32* index, boolean* found)
		{
			for (UINT32 i{ 0 }; i < m_Size; ++i)
			{
				if (static_cast<IAppxPackageCore*>(m_Array + i) == value)
				{
					*index = i;
					return S_OK;
				}
			}
			*index = 0;
			*found = false;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE GetMany(UINT32 startIndex, UINT32 capacity, T_ABI* value, UINT32* actual)
		{
			if (startIndex <= m_Size)
			{
				UINT32 itemsGot{ 0 };
				for (UINT32 i{ startIndex }; itemsGot < capacity && i < m_Size; ++i, ++itemsGot)
				{
					auto* const element{ m_Array + i };
					element->AddRef();
					value[itemsGot] = element;
				}
				*actual = itemsGot;
				return S_OK;
			}
			else
			{ return E_BOUNDS; }
		}

		//IIterable<AppxPackage>
		HRESULT STDMETHODCALLTYPE First(ABI::Windows::Foundation::Collections::IIterator<T_Logical>** first)
		{
			auto* const instance{ new InternalIterator{ m_Array, m_Size, this } };
			if (instance)
			{
				*first = instance;
				return S_OK;
			}
			else
			{ return E_OUTOFMEMORY; }
		}

		//IInspectable
		HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className)
		{ return WindowsCreateString(L"Windows.Foundation.Collections.IVectorView`1<AppxUtils.AppxPackage>", 67, className); }

		~AppxPackageVectorView() noexcept
		{
			for (UINT32 i{ m_Size }; i > 0;)
			{ m_Array[--i].Release(); }
			::operator delete[](m_Array);
		}

	private:
		AppxPackageElement* const m_Array{ nullptr };
	};

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
		AcquireSRWLockExclusive(&m_Lock);
		m_CanContinue = false;
		ReleaseSRWLockExclusive(&m_Lock);
		return S_OK;
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
											auto* const packages{ static_cast<AppxPackageElement*>(::operator new[](__aligned_size_of<AppxPackageElement> * count)) };
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
																	new (packages + completed) AppxPackageElement{ reader };
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
													auto* const instance{ canContinue ? new AppxPackageVectorView{ packages, count } : nullptr };
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
