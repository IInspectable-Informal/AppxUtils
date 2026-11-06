// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "AsyncInfoBase.hpp"

namespace ABI::AppxUtils::Internal
{
	class AsyncActionBase abstract : public AsyncInfoBase<ABI::Windows::Foundation::IAsyncAction>
	{
	public:
		AsyncActionBase() noexcept = default;

		//IAsyncAction
        HRESULT STDMETHODCALLTYPE put_Completed(ABI::Windows::Foundation::IAsyncActionCompletedHandler* handler);
        HRESULT STDMETHODCALLTYPE get_Completed(ABI::Windows::Foundation::IAsyncActionCompletedHandler** handler);
        HRESULT STDMETHODCALLTYPE GetResults();

		//IInspectable
		HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className);

		virtual ~AsyncActionBase() noexcept;

	protected:
		ABI::Windows::Foundation::IAsyncActionCompletedHandler* m_Completed{ nullptr };
	};
}
