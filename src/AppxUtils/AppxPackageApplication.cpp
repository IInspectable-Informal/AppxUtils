#include "pch.h"
#include "AppxPackageApplication.h"
#include "helpers.hpp"

namespace ABI
{
	using namespace Windows::Foundation;
	using namespace Windows::Foundation::Collections;
	using namespace AppxUtils::Internal;
}

namespace ABI::AppxUtils
{
	constexpr wchar_t* const g_PropNames[]
	{
		L"AppListEntry",
		L"BackgroundColor",
		L"DefaultSize",
		L"Description",
		L"DisplayName",
		L"EntryPoint",
		L"Executable",
		L"ForegroundText",
		L"ID",
		L"LockScreenLogo",
		L"LockScreenNotification",
		L"Logo",
		L"MinWidth",
		L"ShortName",
		L"SmallLogo",
		L"Square150x150Logo",
		L"Square30x30Logo",
		L"Square310x310Logo",
		L"Square44x44Logo",
		L"Square70x70Logo",
		L"Square71x71Logo",
		L"StartPage",
		L"Tall150x310Logo",
		L"VisualGroup",
		L"WideLogo",
		L"Wide310x150Logo"
	};

	AppxPackageApplication::AppxPackageApplication(IAppxManifestApplication*& application, CRITICAL_SECTION* criticalSection) noexcept :
		m_Application(application), m_CriticalSection(criticalSection)
	{

	}

#pragma region IAppxPackageApplication
	HRESULT STDMETHODCALLTYPE AppxPackageApplication::get_AppUserModelId(HSTRING* value)
	{
		HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_AppUserModelId), nullptr, nullptr)) };
		if (local == nullptr)
		{
			EnterCriticalSection(m_CriticalSection);
			HRESULT hr{ S_OK };
			if (!m_AppUserModelId)
			{
				LPWSTR aumid{ nullptr };
				hr = m_Application->GetAppUserModelId(&aumid);
				if (SUCCEEDED(hr))
				{
					hr = WindowsCreateString(aumid, WStringLength(aumid), &m_AppUserModelId);
					CoTaskMemFree(aumid);
				}
			}
			local = m_AppUserModelId;
			LeaveCriticalSection(m_CriticalSection);
			if (FAILED(hr))
			{ return hr; }
		}
		return WindowsDuplicateString(local, value);
	}

	HRESULT STDMETHODCALLTYPE AppxPackageApplication::get_Id(HSTRING* value)
	{ return GetPropertyValue(AppxPackageApplicationProperty::Id, value); }

	HRESULT STDMETHODCALLTYPE AppxPackageApplication::get_DisplayName(HSTRING* value)
	{ return GetPropertyValue(AppxPackageApplicationProperty::DisplayName, value); }

	HRESULT STDMETHODCALLTYPE AppxPackageApplication::get_Description(HSTRING* value)
	{ return GetPropertyValue(AppxPackageApplicationProperty::Description, value); }

	HRESULT STDMETHODCALLTYPE AppxPackageApplication::get_BackgroundColor(HSTRING* value)
	{ return GetPropertyValue(AppxPackageApplicationProperty::BackgroundColor, value); }

	HRESULT STDMETHODCALLTYPE AppxPackageApplication::GetPropertyValue(AppxPackageApplicationProperty prop, HSTRING* result)
	{
		const INT32 propOrder{ static_cast<INT32>(prop) };
		if (propOrder < 26 && propOrder >= 0)
		{
			HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(m_PropValues + propOrder), nullptr, nullptr)) };
			if (local == nullptr)
			{
				EnterCriticalSection(m_CriticalSection);
				HRESULT hr{ S_OK };
				if (!m_PropValues[propOrder])
				{
					LPWSTR propValue{ nullptr };
					hr = m_Application->GetStringValue(g_PropNames[propOrder], &propValue);
					if (SUCCEEDED(hr))
					{
						hr = WindowsCreateString(propValue, WStringLength(propValue), m_PropValues + propOrder);
						CoTaskMemFree(propValue);
					}
				}
				local = m_PropValues[propOrder];
				LeaveCriticalSection(m_CriticalSection);
				if (FAILED(hr))
				{ return hr; }
			}
			return WindowsDuplicateString(local, result);
		}
		else
		{ return E_INVALIDARG; }
	}

	HRESULT STDMETHODCALLTYPE AppxPackageApplication::TryGetPropertyValue(AppxPackageApplicationProperty prop, HSTRING* propValue, boolean* result)
	{
		*result = SUCCEEDED(GetPropertyValue(prop, propValue));
		return S_OK;
	}
#pragma endregion

#pragma region IAppxPackageApplicationInterop
	HRESULT STDMETHODCALLTYPE AppxPackageApplication::get_Application(IAppxManifestApplication** value)
	{
		m_Application->AddRef();
		*value = m_Application;
		return S_OK;
	}
#pragma endregion

#pragma region IInspectable
	HRESULT STDMETHODCALLTYPE AppxPackageApplication::GetRuntimeClassName(HSTRING* className)
	{ return WindowsCreateString(L"AppxUtils.AppxPackageApplication", 32, className); }
#pragma endregion

	//Destructor
	AppxPackageApplication::~AppxPackageApplication() noexcept
	{
		m_Application->Release();
		if (m_AppUserModelId)
		{ WindowsDeleteString(m_AppUserModelId); }
		for (UINT32 i{ 0 }; i < 26; ++i)
		{
			auto& element{ m_PropValues[i] };
			if (element)
			{ WindowsDeleteString(element); }
		}
		DeleteCriticalSection(m_CriticalSection);
	}
}
