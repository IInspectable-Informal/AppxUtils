// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once

namespace ABI::AppxUtils::Internal
{
    class AppxBundleGetPackagesAsyncOp final : public InspectableBase<BaseTrust,
        ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Foundation::Collections::IVectorView<AppxPackage*>*>,
        ABI::Windows::Foundation::IAsyncInfo,
        IAgileObject
    >
    {
    private:
        using T_Complex = typename ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Foundation::Collections::IVectorView<AppxPackage*>*>::TResult_complex;
        using TResult_ABI = typename ABI::Windows::Foundation::Internal::GetAbiType<T_Complex>::type;
        using TResult_Logical = typename ABI::Windows::Foundation::Internal::GetLogicalType<T_Complex>::type;
        using TCompletedHandler = ABI::Windows::Foundation::IAsyncOperationCompletedHandler<TResult_Logical>*;

    public:
        AppxBundleGetPackagesAsyncOp(DWORD& bundleReader) noexcept;

        HRESULT STDMETHODCALLTYPE LaunchAsyncTask();

        //IAsyncOperation<IVectorView<AppxPackage>>
        HRESULT STDMETHODCALLTYPE put_Completed(TCompletedHandler handler);
        HRESULT STDMETHODCALLTYPE get_Completed(TCompletedHandler* handler);
        HRESULT STDMETHODCALLTYPE GetResults(TResult_ABI* results);

        //IAsyncInfo
        HRESULT STDMETHODCALLTYPE get_Id(UINT32* id);
        HRESULT STDMETHODCALLTYPE get_Status(ABI::Windows::Foundation::AsyncStatus* status);
        HRESULT STDMETHODCALLTYPE get_ErrorCode(HRESULT* errorCode);
        HRESULT STDMETHODCALLTYPE Cancel();
        HRESULT STDMETHODCALLTYPE Close();

        //IInspectable
        HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className);

        ~AppxBundleGetPackagesAsyncOp() noexcept;

    private:
        TCompletedHandler m_Completed{ nullptr };
        TResult_ABI m_Result{ nullptr };

        const UINT32 m_Id{ 0 };
        LONG m_Status{ static_cast<LONG>(ABI::Windows::Foundation::AsyncStatus::Started) };
        HRESULT m_ErrorCode{ S_OK };
        short m_CanContinue{ true };

        const DWORD m_BundleReader{ 0 };

        static void CALLBACK InitPackagesCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context);
    };
}
