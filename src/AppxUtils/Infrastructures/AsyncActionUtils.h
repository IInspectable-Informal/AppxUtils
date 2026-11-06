// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once

namespace ABI::AppxUtils::Internal
{
	class AsyncActionUtils final
	{
	private:
		using IBase = ABI::Windows::Foundation::IAsyncAction;
		using ICompletedHandler = ABI::Windows::Foundation::IAsyncActionCompletedHandler;

	public:
		static HRESULT STDMETHODCALLTYPE AwaitAsyncObject(IBase* const async);
		static HRESULT STDMETHODCALLTYPE GetAsyncStatus(IBase* const async, ABI::Windows::Foundation::AsyncStatus& status);
		static HRESULT STDMETHODCALLTYPE GetErrorCode(IBase* const async, HRESULT& errorCode);
		static void STDMETHODCALLTYPE CloseAsyncObject(IBase* const async);

		AsyncActionUtils() = delete;
		AsyncActionUtils(const AsyncActionUtils&) = delete;
		AsyncActionUtils(AsyncActionUtils&&) = delete;
		AsyncActionUtils operator =(const AsyncActionUtils&) = delete;
		AsyncActionUtils operator =(AsyncActionUtils&&) = delete;
		~AsyncActionUtils() = delete;
	};
}
