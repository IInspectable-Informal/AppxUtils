// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
#include "AppxPackage.h"
#include "AppxBundle.h"
#include "helpers.hpp"

namespace ABI
{
	using namespace Windows::Foundation;
	using namespace Windows::Foundation::Collections;
	using namespace Windows::ApplicationModel;
	using namespace Windows::Storage;
	using namespace Windows::Storage::Streams;
	using namespace Windows::System;
	using namespace AppxUtils::Internal;
}

namespace ABI::AppxUtils
{
#pragma region Free members
	static INIT_ONCE g_InitOnce{ INIT_ONCE_STATIC_INIT };
	static UINT64 g_FactoryRefCount{ 0 };
	static IAppxFactory* g_AppxPackageFactory{ nullptr };

	BOOL WINAPI InitAppxPackageFactory(INIT_ONCE* InitOnce, void* Parameter, void** Context)
	{
		auto& hr{ *reinterpret_cast<HRESULT*>(Context) };
		hr = CoCreateInstance(CLSID_AppxFactory, nullptr, CLSCTX_INPROC_SERVER, __uuidof(g_AppxPackageFactory), to_void_pp(g_AppxPackageFactory));
		return SUCCEEDED(hr);
	}
#pragma endregion

	class AppxPackagesIterator final : public InspectableBase<BaseTrust,
		ABI::IIterator<AppxPackage*>,
		IAgileObject
	>
	{
	public:
		AppxPackagesIterator(AppxPackageElement* list, const UINT32 size, AppxBundle* bundle) noexcept : m_Size(size), m_Array(list), m_Bundle(bundle)
		{ bundle->AddRef(); }

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

		~AppxPackagesIterator()
		{ m_Bundle->Release(); }

	private:
		const UINT32 m_Size{ 0 };
		UINT32 m_Current{ 0 };
		AppxPackageElement* const m_Array{ nullptr };
		AppxBundle* const m_Bundle{ nullptr };
	};

#pragma region AppxBundle
	AppxBundle::AppxBundle(IAppxBundleReader*& reader, CRITICAL_SECTION* criticalSection) noexcept :
		m_BundleReader(reader), m_CriticalSection(criticalSection)
	{

	}

#pragma region IAppxBundleCore
	HRESULT STDMETHODCALLTYPE AppxBundle::get_Name(HSTRING* value)
	{
		HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Name), nullptr, nullptr)) };
		if (local == nullptr)
		{
			EnterCriticalSection(m_CriticalSection);
			HRESULT hr{ S_OK };
			if (!m_Name)
			{
				IAppxManifestPackageId* pkgId{ nullptr };
				hr = GetBundleId(pkgId);
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
			local = m_Name;
			LeaveCriticalSection(m_CriticalSection);
			if (FAILED(hr))
			{ return hr; }
		}
		return WindowsDuplicateString(local, value);
	}

	HRESULT STDMETHODCALLTYPE AppxBundle::get_FamilyName(HSTRING* value)
	{
		HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_FamilyName), nullptr, nullptr)) };
		if (local == nullptr)
		{
			EnterCriticalSection(m_CriticalSection);
			HRESULT hr{ S_OK };
			if (!m_FamilyName)
			{
				IAppxManifestPackageId* pkgId{ nullptr };
				hr = GetBundleId(pkgId);
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
			local = m_FamilyName;
			LeaveCriticalSection(m_CriticalSection);
			if (FAILED(hr))
			{ return hr; }
		}
		return WindowsDuplicateString(local, value);
	}

	HRESULT STDMETHODCALLTYPE AppxBundle::get_FullName(HSTRING* value)
	{
		HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_FullName), nullptr, nullptr)) };
		if (local == nullptr)
		{
			EnterCriticalSection(m_CriticalSection);
			HRESULT hr{ S_OK };
			if (!m_FullName)
			{
				IAppxManifestPackageId* pkgId{ nullptr };
				hr = GetBundleId(pkgId);
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
			local = m_FullName;
			LeaveCriticalSection(m_CriticalSection);
			if (FAILED(hr))
			{ return hr; }
		}
		return WindowsDuplicateString(local, value);
	}

	HRESULT STDMETHODCALLTYPE AppxBundle::get_Publisher(HSTRING* value)
	{
		HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Publisher), nullptr, nullptr)) };
		if (local == nullptr)
		{
			EnterCriticalSection(m_CriticalSection);
			HRESULT hr{ S_OK };
			if (!m_Publisher)
			{
				IAppxManifestPackageId* pkgId{ nullptr };
				hr = GetBundleId(pkgId);
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
			local = m_Publisher;
			LeaveCriticalSection(m_CriticalSection);
			if (FAILED(hr))
			{ return hr; }
		}
		return WindowsDuplicateString(local, value);
	}

	HRESULT STDMETHODCALLTYPE AppxBundle::get_Version(struct ABI::PackageVersion* value)
	{
		short local{ InterlockedCompareExchange16(&m_HasVersion, false, false) };
		if (local == false)
		{
			EnterCriticalSection(m_CriticalSection);
			HRESULT hr{ S_OK };
			if (!m_HasVersion)
			{
				IAppxManifestPackageId* pkgId{ nullptr };
				hr = GetBundleId(pkgId);
				if (SUCCEEDED(hr))
				{
					UINT64 ver{ 0 };
					hr = pkgId->GetVersion(&ver);
					if (SUCCEEDED(hr))
					{
						m_HasVersion = true;
						m_Version = UInt64ToPkgVer(ver);
					}
				}
			}
			LeaveCriticalSection(m_CriticalSection);
			if (FAILED(hr))
			{ return hr; }
		}
		*value = m_Version;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxBundle::get_Architecture(ABI::ProcessorArchitecture* value)
	{
		short local{ InterlockedCompareExchange16(&m_HasArchitecture, false, false) };
		if (local == false)
		{
			EnterCriticalSection(m_CriticalSection);
			HRESULT hr{ S_OK };
			if (!m_HasArchitecture)
			{
				IAppxManifestPackageId* pkgId{ nullptr };
				hr = GetBundleId(pkgId);
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
			LeaveCriticalSection(m_CriticalSection);
			if (FAILED(hr))
			{ return hr; }
		}
		*value = m_Architecture;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxBundle::get_ResourceId(HSTRING* value)
	{
		HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_ResourceId), nullptr, nullptr)) };
		if (local == nullptr)
		{
			EnterCriticalSection(m_CriticalSection);
			HRESULT hr{ S_OK };
			if (!m_ResourceId)
			{
				IAppxManifestPackageId* pkgId{ nullptr };
				hr = GetBundleId(pkgId);
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
			local = m_ResourceId;
			LeaveCriticalSection(m_CriticalSection);
			if (FAILED(hr))
			{ return hr; }
		}
		return WindowsDuplicateString(local, value);
	}

	HRESULT STDMETHODCALLTYPE AppxBundle::GetManifestStream(ABI::IInputStream** result)
	{
		HRESULT hr{ S_OK };
		auto local{ reinterpret_cast<ABI::IRandomAccessStream*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_ManifestStream), nullptr, nullptr)) };
		if (local == nullptr)
		{
			EnterCriticalSection(m_CriticalSection);
			if (!m_ManifestStream)
			{
				IAppxBundleManifestReader* reader{ nullptr };
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
			local = m_ManifestStream;
			LeaveCriticalSection(m_CriticalSection);
			if (FAILED(hr))
			{ return hr; }
		}
		ABI::IRandomAccessStream* clonedStream{ nullptr };
		hr = local->CloneStream(&clonedStream);
		if (SUCCEEDED(hr))
		{
			hr = clonedStream->QueryInterface(__uuidof(*result), to_void_pp(*result));
			clonedStream->Release();
		}
		return hr;
	}
#pragma endregion

#pragma region Windows.Foundation.Collections.IVectorView<AppxPackage>
	HRESULT STDMETHODCALLTYPE AppxBundle::GetAt(UINT32 index, IAppxPackageCore** item)
	{
		HRESULT hr{ S_OK };
		if (InitOnceExecuteOnce(&m_InitOnce, InitListStatic, this, reinterpret_cast<void**>(&hr)))
		{
			if (index < m_Size)
			{
				auto* element{ m_AppxPackages + index };
				element->AddRef();
				*item = element;
				return S_OK;
			}
			else
			{ return E_BOUNDS; }
		}
		else
		{ return hr; }
	}

	HRESULT STDMETHODCALLTYPE AppxBundle::get_Size(UINT32* size)
	{
		HRESULT hr{ S_OK };
		if (InitOnceExecuteOnce(&m_InitOnce, InitListStatic, this, reinterpret_cast<void**>(&hr)))
		{
			*size = m_Size;
			return S_OK;
		}
		else
		{ return hr; }
	}

	HRESULT STDMETHODCALLTYPE AppxBundle::IndexOf(IAppxPackageCore* value, UINT32* index, boolean* found)
	{
		HRESULT hr{ S_OK };
		if (InitOnceExecuteOnce(&m_InitOnce, InitListStatic, this, reinterpret_cast<void**>(&hr)))
		{
			for (UINT32 i{ 0 }; i < m_Size; ++i)
			{
				if (static_cast<IAppxPackageCore*>(m_AppxPackages + i) == value)
				{
					*index = i;
					return S_OK;
				}
			}
			*index = 0;
			*found = false;
			return S_OK;
		}
		else
		{ return hr; }
	}

	HRESULT STDMETHODCALLTYPE AppxBundle::GetMany(UINT32 startIndex, UINT32 capacity, IAppxPackageCore** value, UINT32* actual)
	{
		HRESULT hr{ S_OK };
		if (InitOnceExecuteOnce(&m_InitOnce, InitListStatic, this, reinterpret_cast<void**>(&hr)))
		{
			if (startIndex <= m_Size)
			{
				UINT32 itemsGot{ 0 };
				for (UINT32 i{ startIndex }; itemsGot < capacity && i < m_Size; ++i, ++itemsGot)
				{
					auto* element{ m_AppxPackages + i };
					element->AddRef();
					value[itemsGot] = element;
				}
				*actual = itemsGot;
				return S_OK;
			}
			else
			{ return E_BOUNDS; }
		}
		else
		{ return hr; }
	}
#pragma endregion

#pragma region Windows.Foundation.Collections.IIterable<AppxPackage>
	HRESULT STDMETHODCALLTYPE AppxBundle::First(ABI::IIterator<AppxPackage*>** first)
	{
		HRESULT hr{ S_OK };
		if (InitOnceExecuteOnce(&m_InitOnce, InitListStatic, this, reinterpret_cast<void**>(&hr)))
		{
			auto* instance{ new AppxPackagesIterator{ m_AppxPackages, m_Size, this } };
			if (instance)
			{
				*first = instance;
				return S_OK;
			}
			else
			{ return E_OUTOFMEMORY; }
		}
		else
		{ return hr; }
	}
#pragma endregion

#pragma region IAppxBundleInterop
	HRESULT STDMETHODCALLTYPE AppxBundle::get_BundleReader(IAppxBundleReader** value)
	{
		m_BundleReader->AddRef();
		*value = m_BundleReader;
		return S_OK;
	}
#pragma endregion

#pragma region IInspectable
	HRESULT STDMETHODCALLTYPE AppxBundle::GetRuntimeClassName(HSTRING* className)
	{ return WindowsCreateString(L"AppxUtils.AppxBundle", 20, className); }
#pragma endregion

#pragma region Private methods
	HRESULT STDMETHODCALLTYPE AppxBundle::GetManifestReader(IAppxBundleManifestReader*& reader)
	{
		if (!m_ManifestReader)
		{
			HRESULT hr{ m_BundleReader->GetManifest(&m_ManifestReader) };
			if (FAILED(hr))
			{ return hr; }
		}
		reader = m_ManifestReader;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxBundle::GetBundleId(IAppxManifestPackageId*& bundleId)
	{
		if (!m_BundleId)
		{
			IAppxBundleManifestReader* reader{ nullptr };
			HRESULT hr{ m_BundleReader->GetManifest(&reader) };
			if (SUCCEEDED(hr))
			{
				hr = reader->GetPackageId(&m_BundleId);
				reader->Release();
			}
			if (FAILED(hr))
			{ return hr; }
		}
		bundleId = m_BundleId;
		return S_OK;
	}
#pragma endregion

	//Destructor
	AppxBundle::~AppxBundle() noexcept
	{
		m_BundleReader->Release();
		if (m_ManifestReader)
		{ m_ManifestReader->Release(); }
		if (m_BundleId)
		{ m_BundleId->Release(); }
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
		if (m_ManifestStream)
		{
			ABI::IClosable* closable{ nullptr };
			m_ManifestStream->QueryInterface(__uuidof(closable), to_void_pp(closable));
			closable->Close();
			closable->Release();
			m_ManifestStream->Release();
		}
		DeleteCriticalSection(m_CriticalSection);
		delete m_CriticalSection;
		if (m_AppxPackages)
		{
			for (UINT32 i{ 0 }; i < m_Size; ++i)
			{ m_AppxPackages[i].Release(); }
			delete[] m_CriticalSections;
			delete[] m_AppxPackages;
		}
		auto* factoryRefCopy = reinterpret_cast<IAppxFactory*>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&g_AppxPackageFactory), nullptr, nullptr));
		if (InterlockedCompareExchange(&g_FactoryRefCount, 0, 0) && !InterlockedDecrement(&g_FactoryRefCount))
		{
			InterlockedCompareExchangePointer(&g_InitOnce.Ptr, nullptr, g_InitOnce.Ptr);
			factoryRefCopy->Release();
		}
	}

#pragma region Static members
	BOOL WINAPI AppxBundle::InitListStatic(INIT_ONCE* InitOnce, void* Parameter, void** Context)
	{
		auto& external{ *reinterpret_cast<AppxBundle*>(Parameter) };
		auto& hr{ *reinterpret_cast<HRESULT*>(Context) };
		if (InitOnceExecuteOnce(&g_InitOnce, InitAppxPackageFactory, nullptr, reinterpret_cast<void**>(&hr)))
		{
			InterlockedIncrement(&g_FactoryRefCount);
			IAppxFilesEnumerator* enumerator{ nullptr };
			hr = external.m_BundleReader->GetPayloadPackages(&enumerator);
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
				hr = external.m_BundleReader->GetPayloadPackages(&enumerator);
				if (SUCCEEDED(hr))
				{
					auto* packages{ reinterpret_cast<AppxPackageElement*>(new byte[__aligned_size_of<AppxPackageElement> *count]) };
					if (packages)
					{
						auto* criticalSections{ new CRITICAL_SECTION[count] };
						if (criticalSections)
						{
							enumerator->GetHasCurrent(&hasNext);
							UINT32 completed{ 0 };
							while (hasNext)
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
										hr = g_AppxPackageFactory->CreatePackageReader(stream, &reader);
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
								if (FAILED(hr))
								{
									for (UINT32 i{ 0 }; i < completed; ++i)
									{ packages[i].Release(); }
									delete[] criticalSections;
									delete[] packages;
									break;
								}
							}
							if (SUCCEEDED(hr))
							{
								external.m_Size = count;
								external.m_AppxPackages = packages;
								external.m_CriticalSections = criticalSections;
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
					enumerator->Release();
				}
			}
			if (FAILED(hr))
			{ InterlockedDecrement(&g_FactoryRefCount); }
		}
		return SUCCEEDED(hr);
	}
#pragma endregion
#pragma endregion;
}
