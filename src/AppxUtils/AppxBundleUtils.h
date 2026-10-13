// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once

namespace ABI::AppxUtils::Internal
{
    class AppxBundleGetPackagesAsyncOp final : public AsyncOperation<ABI::Windows::Foundation::Collections::IVectorView<AppxPackage*>*>
    {
    public:
        AppxBundleGetPackagesAsyncOp(DWORD& bundleReader) noexcept;

        HRESULT STDMETHODCALLTYPE LaunchAsyncTask();

        //IAsyncInfo
        HRESULT STDMETHODCALLTYPE Close();

        ~AppxBundleGetPackagesAsyncOp() noexcept;

    private:
        const DWORD m_BundleReader{ 0 };

        static void CALLBACK InitPackagesCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context);
    };
}
