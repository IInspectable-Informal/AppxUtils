// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once

namespace ABI::AppxUtils
{
	class AppxPackageApplication final : public InspectableElementBase<BaseTrust,
		IAppxPackageApplication,
		IAppxPackageApplicationInterop,
		IAgileObject>
	{
	public:
		AppxPackageApplication(IAppxManifestApplication*& application) noexcept;

		HRESULT STDMETHODCALLTYPE get_AppUserModelId(HSTRING* value);
		HRESULT STDMETHODCALLTYPE get_Id(HSTRING* value);
		HRESULT STDMETHODCALLTYPE get_DisplayName(HSTRING* value);
		HRESULT STDMETHODCALLTYPE get_Description(HSTRING* value);
		HRESULT STDMETHODCALLTYPE get_BackgroundColor(HSTRING* value);

		HRESULT STDMETHODCALLTYPE GetPropertyValue(AppxPackageApplicationProperty prop, HSTRING* result);
		HRESULT STDMETHODCALLTYPE TryGetPropertyValue(AppxPackageApplicationProperty prop, HSTRING* propValue, boolean* result);

		//IAppxPackageApplicationInterop
		HRESULT STDMETHODCALLTYPE get_Application(IAppxManifestApplication** value);

		//IInspectable
		HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className);

		~AppxPackageApplication() noexcept;

	private:
		HSTRING m_AppUserModelId{ nullptr };

		HSTRING m_PropValues[26]{ nullptr };

		IAppxManifestApplication* const m_Application{ nullptr };
		SRWLOCK m_Lock{ SRWLOCK_INIT };
	};
}
