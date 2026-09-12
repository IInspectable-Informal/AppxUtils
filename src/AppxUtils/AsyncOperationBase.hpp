// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"

namespace ABI::AppxUtils::Internal
{
	template<typename TResult>
	class AsyncOperationBase abstract : public InspectableBase<BaseTrust,
		ABI::Windows::Foundation::IAsyncOperation<TResult>,
		ABI::Windows::Foundation::IAsyncInfo,
		IAgileObject
	>
	{
	private:
		using IBase = ABI::Windows::Foundation::IAsyncOperation<TResult>;
		using TResult_ABI = typename ABI::Windows::Foundation::Internal::GetAbiType<TResult_complex>;
		using TResult_Logical = typename ABI::Windows::Foundation::Internal::GetLogicalType<TResult_complex>;
		using ICompletedHandler = ABI::Windows::Foundation::IAsyncOperationProgressHandler<TResult_complex>;

	public:
		AsyncOperationBase() noexcept : m_Id(InterlockedIncrement(&s_AsyncOperationId))
		{

		}

		//IAsyncOperation<T>
		HRESULT STDMETHODCALLTYPE put_Completed(ICompletedHandler handler)
		{
			auto* const oldValue{ static_cast<ICompletedHandler>(InterlockedExchangePointer(to_void_pp(m_Completed), handler)) };
			if (handler)
			{
				handler->AddRef();
				const auto status{ static_cast<ABI::Windows::Foundation::AsyncStatus>(InterlockedCompareExchange(&m_Status, 0, 0)) };
				if (status != ABI::Windows::Foundation::AsyncStatus::Started)
				{ handler->Invoke(this, status); }
			}
			if (oldValue)
			{ oldValue->Release(); }
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE get_Completed(ICompletedHandler* handler)
		{
			auto* const local{ static_cast<ICompletedHandler>(InterlockedCompareExchangePointer(to_void_pp(m_Completed), nullptr, nullptr)) };
			if (local)
			{ local->AddRef(); }
			*handler = local;
			return S_OK;
		}

		//IAsyncInfo
		HRESULT STDMETHODCALLTYPE get_Id(UINT32* id)
		{
			*id = m_Id;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE get_Status(ABI::Windows::Foundation::AsyncStatus* status)
		{
			*status = static_cast<ABI::AsyncStatus>(InterlockedCompareExchange(&m_Status, static_cast<LONG>(ABI::Windows::Foundation::AsyncStatus::Started), static_cast<LONG>(ABI::Windows::Foundation::AsyncStatus::Started)));
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE get_ErrorCode(HRESULT* errorCode)
		{
			*errorCode = InterlockedCompareExchange(&m_ErrorCode, S_OK, S_OK);
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE Cancel()
		{
			InterlockedExchange16(&m_CanContinue, false);
			return S_OK;
		}

		//IInspectable
		HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className)
		{
			if (InitOnceExecuteOnce(&s_InitOnce, InitStringStatic, nullptr, nullptr))
			{ return WindowsCreateString(s_ClassName, s_ClassNameLength, className); }
			else
			{ return E_OUTOFMEMORY; }
		}

	protected:
		ICompletedHandler m_Completed{ nullptr };
		TResult_ABI m_Result{ nullptr };

		const UINT32 m_Id{ 0 };
		LONG m_Status{ static_cast<LONG>(ABI::Windows::Foundation::AsyncStatus::Started) };
		HRESULT m_ErrorCode{ S_OK };
		short m_CanContinue{ true };

	private:
		static inline UINT32 s_AsyncOperationId{ 0 };

		static inline UINT32 s_ClassNameLength{ 0 };
		static inline const wchar_t* const s_ClassName{ nullptr };
		static INIT_ONCE s_InitOnce{ INIT_ONCE_STATIC_INIT };

		static BOOL WINAPI InitStringStatic(INIT_ONCE* InitOnce, void* Parameter, void** Context)
		{
			s_ClassName = IBase::z_get_rc_name_impl();
			while (*(s_ClassName + s_ClassNameSize))
			{ ++s_ClassNameSize; }
			return true;
		}
	};
}
