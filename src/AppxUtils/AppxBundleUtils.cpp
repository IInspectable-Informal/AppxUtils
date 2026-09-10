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
	class AppxPackageVectorView final : public InspectableBase<BaseTrust,
		ABI::IVectorView<ABI::AppxPackage*>, ABI::IIterable<ABI::AppxPackage*>,
		IAgileObject
	>
	{
	private:
		class InternalIterator final : public InspectableBase<BaseTrust,
			ABI::IIterator<AppxPackage*>,
			IAgileObject
		>
		{
		public:
			InternalIterator(AppxPackageElement* const list, const UINT32 size, AppxPackageVectorView* const vectorView) noexcept : m_Size(size), m_Array(list), m_VectorView(vectorView)
			{
				vectorView->AddRef();
			}

			HRESULT STDMETHODCALLTYPE get_Current(IAppxPackageCore** current)
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

			HRESULT STDMETHODCALLTYPE get_HasCurrent(boolean* hasCurrent)
			{
				*hasCurrent = m_Current < m_Size;
				return S_OK;
			}

			HRESULT STDMETHODCALLTYPE MoveNext(boolean* hasCurrent)
			{
				if (m_Current <= m_Size)
				{
					++m_Current;
					*hasCurrent = m_Current < m_Size;
					return S_OK;
				}
				else
				{ return E_BOUNDS; }
			}

			HRESULT STDMETHODCALLTYPE GetMany(UINT32 capacity, IAppxPackageCore** value, UINT32* actual)
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
			const UINT32 m_Size{ 0 };
			UINT32 m_Current{ 0 };
			AppxPackageElement* const m_Array{ nullptr };
			AppxPackageVectorView* const m_VectorView{ nullptr };
		};

	public:
		AppxPackageVectorView(AppxPackageElement* const list, const UINT32 size, CRITICAL_SECTION* const criticalSections) noexcept :
			m_Array(list), m_Size(size), m_CriticalSections(criticalSections)
		{

		}

		HRESULT STDMETHODCALLTYPE GetAt(UINT32 index, IAppxPackageCore** item)
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

		HRESULT STDMETHODCALLTYPE get_Size(UINT32* size)
		{
			*size = m_Size;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE IndexOf(IAppxPackageCore* value, UINT32* index, boolean* found)
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

		HRESULT STDMETHODCALLTYPE GetMany(UINT32 startIndex, UINT32 capacity, IAppxPackageCore** value, UINT32* actual)
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
		HRESULT STDMETHODCALLTYPE First(ABI::Windows::Foundation::Collections::IIterator<AppxPackage*>** first)
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
			for (UINT32 i{ 0 }; i < m_Size; ++i)
			{ m_Array[i].Release(); }
			delete[] m_CriticalSections;
			delete[] m_Array;
		}

	private:
		const UINT32 m_Size{ 0 };
		AppxPackageElement* const m_Array{ nullptr };
		const CRITICAL_SECTION* const m_CriticalSections{ nullptr };
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

#pragma region IAsyncOperation<IVectorView<AppxPackage>>
	HRESULT STDMETHODCALLTYPE AppxBundleGetPackagesAsyncOp::put_Completed(TCompletedHandler handler)
	{
		printf("put_Completed\n");
		auto* const oldValue{ reinterpret_cast<TCompletedHandler>(InterlockedCompareExchangePointer(to_void_pp(m_Completed), nullptr, nullptr)) };
		if (InterlockedCompareExchangePointer(to_void_pp(m_Completed), handler, m_Completed) == oldValue)
		{
			if (oldValue)
			{ oldValue->Release(); }
			if (handler)
			{
				handler->AddRef();
				const auto status{ static_cast<ABI::AsyncStatus>(InterlockedCompareExchange(&m_Status, 0, 0)) };
				if (status != ABI::AsyncStatus::Started)
				{ handler->Invoke(this, status); }
			}
			return S_OK;
		}
		else
		{
			if (oldValue)
			{
				const auto status{ static_cast<ABI::AsyncStatus>(InterlockedCompareExchange(&m_Status, 0, 0)) };
				if (status != ABI::AsyncStatus::Started)
				{ oldValue->Invoke(this, status); }
			}
			return S_FALSE;
		}
	}

	HRESULT STDMETHODCALLTYPE AppxBundleGetPackagesAsyncOp::get_Completed(TCompletedHandler* handler)
	{
		printf("get_Completed\n");
		auto* local{ reinterpret_cast<TCompletedHandler>(InterlockedCompareExchangePointer(to_void_pp(m_Completed), nullptr, nullptr)) };
		if (local)
		{ local->AddRef(); }
		*handler = local;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxBundleGetPackagesAsyncOp::GetResults(TResult_ABI* results)
	{
		printf("GetResults\n");
		switch (static_cast<ABI::AsyncStatus>(InterlockedCompareExchange(&m_Status, 0, 0)))
		{
			case ABI::AsyncStatus::Completed:
			{
				auto* const result{ reinterpret_cast<TResult_ABI>(InterlockedCompareExchangePointer(to_void_pp(m_Result), nullptr, nullptr)) };
				if (result)
				{ result->AddRef(); }
				*results = result;
				return S_OK;
			} break;

			case ABI::AsyncStatus::Error:
				return InterlockedCompareExchange(&m_ErrorCode, S_OK, S_OK);

			case ABI::AsyncStatus::Canceled:
				return HRESULT_FROM_WIN32(ERROR_CANCELLED);

			default:
				return E_ILLEGAL_METHOD_CALL;
		}
	}
#pragma endregion

#pragma region IAsyncInfo
	HRESULT STDMETHODCALLTYPE AppxBundleGetPackagesAsyncOp::get_Id(UINT32* id)
	{
		printf("get_Id\n");
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE AppxBundleGetPackagesAsyncOp::get_Status(ABI::AsyncStatus* status)
	{
		printf("get_Status\n");
		*status = static_cast<ABI::AsyncStatus>(InterlockedCompareExchange(&m_Status, static_cast<LONG>(ABI::AsyncStatus::Started), static_cast<LONG>(ABI::AsyncStatus::Started)));
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxBundleGetPackagesAsyncOp::get_ErrorCode(HRESULT* errorCode)
	{
		printf("get_ErrorCode\n");
		*errorCode = InterlockedCompareExchange(&m_ErrorCode, S_OK, S_OK);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxBundleGetPackagesAsyncOp::Cancel()
	{
		printf("Cancel\n");
		InterlockedCompareExchange16(&m_CanContinue, false, m_CanContinue);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxBundleGetPackagesAsyncOp::Close()
	{
		printf("Close\n");
		return S_OK;
	}
#pragma endregion

#pragma region IInspectable
	HRESULT STDMETHODCALLTYPE AppxBundleGetPackagesAsyncOp::GetRuntimeClassName(HSTRING* className)
	{ return WindowsCreateString(L"Windows.Foundation.IAsyncOperation`1<Windows.Foundation.Collections.IVectorView`1<AppxUtils.AppxPackage>>", 105, className); }
#pragma endregion

	//Destructor
	AppxBundleGetPackagesAsyncOp::~AppxBundleGetPackagesAsyncOp() noexcept
	{
		if (m_Completed)
		{ m_Completed->Release(); }
		if (m_Result)
		{ m_Result->Release(); }
		//m_BundleReader->Release();
	}

#pragma region Static members
	void CALLBACK AppxBundleGetPackagesAsyncOp::InitPackagesCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context)
	{
		auto& external{ *reinterpret_cast<AppxBundleGetPackagesAsyncOp*>(Context) };
		bool createdVectorView{ false };
		HRESULT hr{ CoInitializeEx(nullptr, COINIT_MULTITHREADED) };
		if (SUCCEEDED(hr))
		{
			if (InterlockedCompareExchange16(&external.m_CanContinue, false, false))
			{
				IGlobalInterfaceTable* pGIT{ nullptr };
				hr = CoCreateInstance(CLSID_StdGlobalInterfaceTable, nullptr, CLSCTX_INPROC_SERVER, __uuidof(pGIT), to_void_pp(pGIT));
				if (SUCCEEDED(hr))
				{
					IAppxBundleReader* bundleReader{ nullptr };
					hr = pGIT->GetInterfaceFromGlobal(external.m_BundleReader, __uuidof(bundleReader), to_void_pp(bundleReader));;
					if (SUCCEEDED(hr))
					{
						IAppxFactory* factory{ nullptr };
						hr = CoCreateInstance(CLSID_AppxFactory, nullptr, CLSCTX_INPROC_SERVER, __uuidof(factory), to_void_pp(factory));
						if (SUCCEEDED(hr))
						{
							if (InterlockedCompareExchange16(&external.m_CanContinue, false, false))
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
										if (InterlockedCompareExchange16(&external.m_CanContinue, false, false))
										{
											auto* const packages{ reinterpret_cast<AppxPackageElement*>(new byte[__aligned_size_of<AppxPackageElement> *count]) };
											if (packages)
											{
												auto* const criticalSections{ new CRITICAL_SECTION[count] };
												if (criticalSections)
												{
													enumerator->GetHasCurrent(&hasNext);
													UINT32 completed{ 0 };
													while (hasNext)
													{
														if (InterlockedCompareExchange16(&external.m_CanContinue, false, false))
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
																		if (InitializeCriticalSectionEx(criticalSections + completed, 0, CRITICAL_SECTION_NO_DEBUG_INFO))
																		{
																			new (packages + completed) AppxPackageElement{ reader, criticalSections + completed };
																			++completed;
																			enumerator->MoveNext(&hasNext);
																		}
																		else
																		{
																			hr = HRESULT_FROM_WIN32(GetLastError());
																			reader->Release();
																		}
																	}
																	stream->Release();
																}
																element->Release();
															}
														}
														else
														{ hr = HRESULT_FROM_WIN32(ERROR_CANCELLED); }
														if (FAILED(hr))
														{
															for (UINT32 i{ 0 }; i < completed; ++i)
															{ packages[i].Release(); }
															printf("failed\n");
															delete[] criticalSections;
															delete[] packages;
															break;
														}
													}
													printf("test\n");
													if (SUCCEEDED(hr))
													{
														auto* const instance{ InterlockedCompareExchange16(&external.m_CanContinue, false, false) ? new AppxPackageVectorView{ packages, count, criticalSections } : nullptr };
														if (instance)
														{
															void* oldResult{ nullptr };
															do
															{
																oldResult = InterlockedCompareExchangePointer(to_void_pp(external.m_Result), nullptr, nullptr);
															} while (InterlockedCompareExchangePointer(to_void_pp(external.m_Result), instance, external.m_Result) != oldResult);
															createdVectorView = true;
														}
														else
														{
															hr = E_OUTOFMEMORY;
															for (UINT32 i{ 0 }; i < completed; ++i)
															{ packages[i].Release(); }
															delete[] criticalSections;
															delete[] packages;
														}
													}
												}
												else
												{
													hr = E_OUTOFMEMORY;
													delete[] packages;
												}
											}
											else
											{ hr = E_OUTOFMEMORY; }
										}
										enumerator->Release();
									}
								}
							}
							factory->Release();
						}
						bundleReader->Release();
					}
					pGIT->RevokeInterfaceFromGlobal(external.m_BundleReader);
					pGIT->Release();
				}
			}
			CoUninitialize();
		}
		LONG newStatus{};
		if (createdVectorView)
		{
			hr = S_OK;
			newStatus = static_cast<LONG>(ABI::AsyncStatus::Completed);
		}
		else
		{
			if (InterlockedCompareExchange16(&external.m_CanContinue, false, false))
			{ hr = HRESULT_FROM_WIN32(ERROR_CANCELLED); }
			newStatus = static_cast<LONG>(InterlockedCompareExchange16(&external.m_CanContinue, false, false) ? ABI::AsyncStatus::Error : ABI::AsyncStatus::Canceled);
		}
		HRESULT oldHR{};
		do
		{
			oldHR = InterlockedCompareExchange(&external.m_ErrorCode, 0, 0);
		} while (InterlockedCompareExchange(&external.m_ErrorCode, hr, external.m_ErrorCode) != oldHR);
		LONG oldStatus{};
		do
		{
			oldStatus = InterlockedCompareExchange(&external.m_Status, 0, 0);
		} while (InterlockedCompareExchange(&external.m_Status, newStatus, external.m_Status) != oldStatus);
		auto* const local{ reinterpret_cast<TCompletedHandler>(InterlockedCompareExchangePointer(to_void_pp(external.m_Completed), nullptr, nullptr)) };
		if (local)
		{ local->Invoke(&external, static_cast<ABI::AsyncStatus>(InterlockedCompareExchange(&external.m_Status, 0, 0))); }
		external.Release();
	}
#pragma endregion
#pragma endregion
}
