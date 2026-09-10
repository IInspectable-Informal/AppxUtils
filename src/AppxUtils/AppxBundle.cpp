// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
#include "AppxPackage.h"
#include "AppxBundle.h"
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
	using namespace AppxUtils::Internal;
}

namespace ABI::AppxUtils
{
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

	HRESULT STDMETHODCALLTYPE AppxBundle::GetPackagesAsync(ABI::IAsyncOperation<ABI::IVectorView<AppxPackage*>*>** operation)
	{
		auto local{ reinterpret_cast<ABI::IAsyncOperation<ABI::IVectorView<AppxPackage*>*>*>(InterlockedCompareExchangePointer(to_void_pp(m_GetPackagesAsyncOp), nullptr, nullptr)) };
		if (local == nullptr)
		{
			EnterCriticalSection(m_CriticalSection);
			HRESULT hr{ S_OK };
			if (!m_GetPackagesAsyncOp)
			{
				IGlobalInterfaceTable* pGIT{ nullptr };
				hr = CoCreateInstance(CLSID_StdGlobalInterfaceTable, nullptr, CLSCTX_INPROC_SERVER, __uuidof(pGIT), to_void_pp(pGIT));
				if (SUCCEEDED(hr))
				{
					DWORD cookie{ 0 };
					hr = pGIT->RegisterInterfaceInGlobal(m_BundleReader, __uuidof(m_BundleReader), &cookie);
					if (SUCCEEDED(hr))
					{
						auto* const instance{ new AppxBundleGetPackagesAsyncOp{ cookie } };
						if (instance)
						{
							hr = instance->LaunchAsyncTask();
							if (SUCCEEDED(hr))
							{ m_GetPackagesAsyncOp = instance; }
							else
							{ instance->Release(); }
						}
						else
						{ hr = E_OUTOFMEMORY; }
						if (FAILED(hr))
						{ pGIT->RevokeInterfaceFromGlobal(cookie); }
					}
					else
					{ printf("regFailed\n"); }
					pGIT->Release();
				}
			}
			local = m_GetPackagesAsyncOp;
			LeaveCriticalSection(m_CriticalSection);
			if (FAILED(hr))
			{ return hr; }
		}
		local->AddRef();
		*operation = local;
		return S_OK;
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
		if (m_GetPackagesAsyncOp)
		{ m_GetPackagesAsyncOp->Release(); }
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
	}

#pragma endregion;
}
