// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "dllmain.h"

namespace ABI::AppxUtils::Internal
{
	template<typename TResult>
	class AsyncOperationCompletedHandler abstract : public UnknownBase<
		ABI::Windows::Foundation::IAsyncOperationCompletedHandler<TResult>
	>
	{
	public:
		virtual ~AsyncOperationCompletedHandler() noexcept = default;
	};

	template<typename TResult, typename TProgress>
	class AsyncOperationWithProgressCompletedHandler abstract : public UnknownBase<
		ABI::Windows::Foundation::IAsyncOperationWithProgressCompletedHandler<TResult, TProgress>
	>
	{
	public:
		virtual ~AsyncOperationWithProgressCompletedHandler() noexcept = default;
	};
}
