// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once

namespace ABI::AppxUtils::Internal
{
	template<typename TResult>
	class AsyncOperationBase abstract : public AsyncInfoBase<ABI::Windows::Foundation::IAsyncOperation<TResult>>
	{
	protected:
		using IBase = ABI::Windows::Foundation::IAsyncOperation<TResult>;
		using TResult_Complex = typename IBase::TResult_complex;
		using TResult_ABI = typename ABI::Windows::Foundation::Internal::GetAbiType<TResult_Complex>::type;
		using TResult_Logical = typename ABI::Windows::Foundation::Internal::GetLogicalType<TResult_Complex>::type;
		using ICompletedHandler = ABI::Windows::Foundation::IAsyncOperationCompletedHandler<TResult>*;

	public:
		AsyncOperationBase() noexcept
		{

		}

		//IAsyncOperation<T>
		HRESULT STDMETHODCALLTYPE put_Completed(ICompletedHandler handler)
		{
			if (handler)
			{ handler->AddRef(); }
			ICompletedHandler oldHandler{ nullptr };
			ABI::Windows::Foundation::AsyncStatus status{};
			HRESULT hr{ S_OK };
			AcquireSRWLockExclusive(&m_Lock);
			if (m_IsOpening)
			{
				oldHandler = m_Completed;
				m_Completed = handler;
				status = static_cast<ABI::Windows::Foundation::AsyncStatus>(m_Status);
				hr = S_OK;
			}
			else
			{ hr = E_ILLEGAL_METHOD_CALL; }
			ReleaseSRWLockExclusive(&m_Lock);
			if (SUCCEEDED(hr))
			{
				if (oldHandler)
				{ oldHandler->Release(); }
				if (status != ABI::Windows::Foundation::AsyncStatus::Started && handler)
				{ handler->Invoke(this, status); }
			}
			else if (handler)
			{ handler->Release(); }
			return hr;
		}

		HRESULT STDMETHODCALLTYPE get_Completed(ICompletedHandler* handler)
		{
			HRESULT hr{ S_OK };
			AcquireSRWLockShared(&m_Lock);
			if (m_IsOpening)
			{
				if (m_Completed)
				{ m_Completed->AddRef(); }
				*handler = m_Completed;
				hr = S_OK;
			}
			else
			{ hr = E_ILLEGAL_METHOD_CALL; }
			ReleaseSRWLockShared(&m_Lock);
			return hr;
		}

		//IInspectable
		HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className)
		{
			if (InitOnceExecuteOnce(&s_InitOnce, InitStringStatic, nullptr, nullptr))
			{ return WindowsCreateString(s_ClassName, s_ClassNameLength, className); }
			else
			{ return E_OUTOFMEMORY; }
		}

		virtual ~AsyncOperationBase() noexcept
		{
			if (m_Completed)
			{ m_Completed->Release(); }
		}

	protected:
		ICompletedHandler m_Completed{ nullptr };

	private:
		static inline UINT32 s_ClassNameLength{ 0 };
		static inline const wchar_t* s_ClassName{ nullptr };
		static inline INIT_ONCE s_InitOnce{ INIT_ONCE_STATIC_INIT };

		static BOOL WINAPI InitStringStatic(INIT_ONCE* InitOnce, void* Parameter, void** Context)
		{
			s_ClassName = IBase::z_get_rc_name_impl();
			while (*(s_ClassName + s_ClassNameLength))
			{ ++s_ClassNameLength; }
			return true;
		}
	};

	template<typename TResult>
	class AsyncOperation abstract : public AsyncOperationBase<TResult>
	{
	public:
		HRESULT STDMETHODCALLTYPE GetResults(TResult_ABI* results)
		{
			HRESULT hr{ S_OK };
			AcquireSRWLockShared(&m_Lock);
			if (m_IsOpening)
			{
				switch (static_cast<ABI::Windows::Foundation::AsyncStatus>(m_Status))
				{
					case ABI::Windows::Foundation::AsyncStatus::Completed:
					{
						if constexpr (IsFundamentalType<TResult_ABI> || __is_enum(TResult_ABI))
						{ *results = m_Result; }
						else if constexpr (ABI::Windows::Foundation::Collections::is_foundation_struct<TResult_ABI>::value)
						{ *results = m_Result; }
						else if constexpr (__is_trivial(TResult_ABI) && __is_standard_layout(TResult_ABI))
						{ hr = StructLifetimeFunctions<TResult_ABI>::DeepCopyStruct(m_Result, *results); }
						else
						{ *results = m_Result; }
					} break;

					case ABI::Windows::Foundation::AsyncStatus::Error:
						hr = m_ErrorCode;
						break;

					case ABI::Windows::Foundation::AsyncStatus::Canceled:
						hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
						break;

					default:
						hr = E_ILLEGAL_METHOD_CALL;
						break;
				}
			}
			else
			{ hr = E_ILLEGAL_METHOD_CALL; }
			ReleaseSRWLockShared(&m_Lock);
			return hr;
		}

		virtual ~AsyncOperation() noexcept
		{
			if constexpr (!IsFundamentalType<T_ABI> && !__is_enum(T_ABI) && !ABI::Windows::Foundation::Collections::is_foundation_struct<T_ABI>::value && __is_trivial(TResult_ABI) && __is_standard_layout(TResult_ABI))
			{ StructLifetimeFunctions<TResult_ABI>::ReleaseStruct(m_Result); }
		}

	protected:
		TResult_ABI m_Result{};
	};

	template<typename TResult>
	class AsyncOperation<TResult*> abstract : public AsyncOperationBase<TResult*>
	{
	protected:
		using IBase = ABI::Windows::Foundation::IAsyncOperation<TResult*>;
		using TResult_Complex = typename IBase::TResult_complex;
		using TResult_ABI = typename ABI::Windows::Foundation::Internal::GetAbiType<TResult_Complex>::type;
		using TResult_Logical = typename ABI::Windows::Foundation::Internal::GetLogicalType<TResult_Complex>::type;

	public:
		HRESULT STDMETHODCALLTYPE GetResults(TResult_ABI* results)
		{
			HRESULT hr{ S_OK };
			AcquireSRWLockShared(&m_Lock);
			if (m_IsOpening)
			{
				switch (static_cast<ABI::Windows::Foundation::AsyncStatus>(m_Status))
				{
					case ABI::Windows::Foundation::AsyncStatus::Completed:
					{
						if (m_Result)
						{ m_Result->AddRef(); }
						*results = m_Result;
					} break;

					case ABI::Windows::Foundation::AsyncStatus::Error:
						hr = m_ErrorCode;
						break;

					case ABI::Windows::Foundation::AsyncStatus::Canceled:
						hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
						break;

					default:
						hr = E_ILLEGAL_METHOD_CALL;
						break;
				}
			}
			else
			{ hr = E_ILLEGAL_METHOD_CALL; }
			ReleaseSRWLockShared(&m_Lock);
			return hr;
		}

		virtual ~AsyncOperation() noexcept
		{
			if (m_Result)
			{ m_Result->Release(); }
		}

	protected:
		TResult_ABI m_Result{ nullptr };
	};

	template<>
	class AsyncOperation<HSTRING> abstract : public AsyncOperationBase<HSTRING>
	{
	public:
		HRESULT STDMETHODCALLTYPE GetResults(HSTRING* results)
		{
			HRESULT hr{ S_OK };
			AcquireSRWLockShared(&m_Lock);
			if (m_IsOpening)
			{
				switch (static_cast<ABI::Windows::Foundation::AsyncStatus>(m_Status))
				{
					case ABI::Windows::Foundation::AsyncStatus::Completed:
					{
						if (m_Result)
						{ hr = WindowsDuplicateString(m_Result, results); }
						else
						{ *results = nullptr; }
					} break;

					case ABI::Windows::Foundation::AsyncStatus::Error:
						hr = m_ErrorCode;
						break;

					case ABI::Windows::Foundation::AsyncStatus::Canceled:
						hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
						break;

					default:
						hr = E_ILLEGAL_METHOD_CALL;
						break;
				}
			}
			else
			{ hr = E_ILLEGAL_METHOD_CALL; }
			ReleaseSRWLockShared(&m_Lock);
			return hr;
		}

		virtual ~AsyncOperation() noexcept
		{
			if (m_Result)
			{ WindowsDeleteString(m_Result); }
		}

	protected:
		HSTRING m_Result{ nullptr };
	};
}
