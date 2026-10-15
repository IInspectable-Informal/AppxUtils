// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "AsyncActionUtils.h"
#include "helpers.hpp"

namespace ABI::AppxUtils::Internal
{
	template<typename TResult>
	class AsyncOperationUtils final
	{
	private:
		using IBase = ABI::Windows::Foundation::IAsyncOperation<TResult>;
		using TResult_Complex = typename IBase::TResult_complex;
		using TResult_ABI = typename ABI::Windows::Foundation::Internal::GetAbiType<TResult_Complex>::type;
		using TResult_Logical = typename ABI::Windows::Foundation::Internal::GetLogicalType<TResult_Complex>::type;
		using ICompletedHandler = ABI::Windows::Foundation::IAsyncOperationCompletedHandler<TResult>;

		class AsyncOperationCompletedHandlerInternal final : public AsyncOperationCompletedHandler<TResult>
		{
		public:
			HRESULT STDMETHODCALLTYPE Wait()
			{ return WaitFor(INFINITE); }

			HRESULT STDMETHODCALLTYPE WaitFor(const UINT32 intervalMilliseconds)
			{
				DWORD win32ErrCode{ ERROR_SUCCESS };
				do
				{ win32ErrCode = WaitForSingleObjectEx(m_EventHandle, intervalMilliseconds, true); }
				while (win32ErrCode == WAIT_IO_COMPLETION);
				return HRESULT_FROM_WIN32(win32ErrCode != WAIT_FAILED ? win32ErrCode : GetLastError());
			}

			HRESULT STDMETHODCALLTYPE ManualResetEvent()
			{ return ResetEvent(m_EventHandle) ? S_OK : HRESULT_FROM_WIN32(GetLastError()); }

			//IAsyncOperationCompletedHandler<T>
			HRESULT STDMETHODCALLTYPE Invoke(IBase* asyncInfo, ABI::Windows::Foundation::AsyncStatus status)
			{
				SetEvent(m_EventHandle);
				return S_OK;
			}

			static HRESULT STDMETHODCALLTYPE CreateInstance(const bool manualReset, const bool initialState,
				AsyncOperationCompletedHandlerInternal*& result)
			{
				const HANDLE hEvent{ CreateEventW(nullptr, manualReset, initialState, nullptr) };
				if (hEvent)
				{
					result = new AsyncOperationCompletedHandlerInternal{ hEvent };
					if (result)
					{ return S_OK; }
					else
					{
						CloseHandle(hEvent);
						return E_OUTOFMEMORY;
					}
				}
				else
				{ return HRESULT_FROM_WIN32(GetLastError()); }
			}

			~AsyncOperationCompletedHandlerInternal() noexcept
			{ CloseHandle(m_EventHandle); }

		private:
			const HANDLE m_EventHandle{ nullptr };

			AsyncOperationCompletedHandlerInternal(const HANDLE hEvent) noexcept : m_EventHandle(hEvent)
			{ }
		};

	public:
		static HRESULT STDMETHODCALLTYPE AwaitAsyncObject(IBase* const async, TResult_ABI& result)
		{
			ABI::Windows::Foundation::AsyncStatus status{ ABI::Windows::Foundation::AsyncStatus::Started };
			HRESULT hr{ GetAsyncStatus(async, status) };
			if (SUCCEEDED(hr))
			{
				if (status == ABI::Windows::Foundation::AsyncStatus::Started)
				{
					AsyncOperationCompletedHandlerInternal* handler{ nullptr };
					hr = AsyncOperationCompletedHandlerInternal::CreateInstance(true, false, handler);
					if (SUCCEEDED(hr))
					{
						hr = async->put_Completed(handler);
						if (SUCCEEDED(hr))
						{ hr = handler->Wait(); }
						handler->Release();
					}
				}
				if (SUCCEEDED(hr))
				{
					hr = GetAsyncStatus(async, status);
					if (SUCCEEDED(hr))
					{
						switch (status)
						{
							case ABI::Windows::Foundation::AsyncStatus::Completed:
								hr = async->GetResults(&result);
								break;

							case ABI::Windows::Foundation::AsyncStatus::Error:
								GetErrorCode(async, hr);
								break;

							case ABI::Windows::Foundation::AsyncStatus::Canceled:
								hr = E_CANCELED;
								break;
						}
					}
				}
			}
			return hr;
		}

		static HRESULT STDMETHODCALLTYPE GetAsyncStatus(IBase* const async, ABI::Windows::Foundation::AsyncStatus& status)
		{
			ABI::Windows::Foundation::IAsyncInfo* asyncInfo{ nullptr };
			async->QueryInterface(__uuidof(asyncInfo), to_void_pp(asyncInfo));
			const HRESULT hr{ asyncInfo->get_Status(&status) };
			asyncInfo->Release();
			return hr;
		}

		static HRESULT STDMETHODCALLTYPE GetErrorCode(IBase* const async, HRESULT& errorCode)
		{
			ABI::Windows::Foundation::IAsyncInfo* asyncInfo{ nullptr };
			async->QueryInterface(__uuidof(asyncInfo), to_void_pp(asyncInfo));
			const HRESULT hr{ asyncInfo->get_ErrorCode(&errorCode) };
			asyncInfo->Release();
			return hr;
		}

	public:
		static void STDMETHODCALLTYPE CloseAsyncObject(IBase* const async)
		{
			ABI::Windows::Foundation::IAsyncInfo* asyncInfo{ nullptr };
			async->QueryInterface(__uuidof(asyncInfo), to_void_pp(asyncInfo));
			asyncInfo->Close();
			asyncInfo->Release();
		}

		AsyncOperationUtils() = delete;
		AsyncOperationUtils(const AsyncOperationUtils&) = delete;
		AsyncOperationUtils(AsyncOperationUtils&&) = delete;
		AsyncOperationUtils operator =(const AsyncOperationUtils&) = delete;
		AsyncOperationUtils operator =(AsyncOperationUtils&&) = delete;
		~AsyncOperationUtils() = delete;
	};

	template<typename TResult, typename TProgress>
	class AsyncOperationWithProgressUtils final
	{
	private:
		using IBase = ABI::Windows::Foundation::IAsyncOperationWithProgress<TResult, TProgress>;
		using TResult_Complex = typename IBase::TResult_complex;
		using TResult_ABI = typename ABI::Windows::Foundation::Internal::GetAbiType<TResult_Complex>::type;
		using TResult_Logical = typename ABI::Windows::Foundation::Internal::GetLogicalType<TResult_Complex>::type;
		using TProgress_Complex = typename IBase::TProgress_complex;
		using TProgress_ABI = typename ABI::Windows::Foundation::Internal::GetAbiType<TProgress_Complex>::type;
		using TProgress_Logical = typename ABI::Windows::Foundation::Internal::GetLogicalType<TProgress_Complex>::type;
		using ICompletedHandler = ABI::Windows::Foundation::IAsyncOperationWithProgressCompletedHandler<TResult, TProgress>;
		using IProgressHandler = ABI::Windows::Foundation::IAsyncOperationProgressHandler<TResult, TProgress>;

		class AsyncOperationWithProgressCompletedHandlerInternal final : public AsyncOperationWithProgressCompletedHandler<TResult, TProgress>
		{
		public:
			HRESULT STDMETHODCALLTYPE Wait()
			{ return WaitFor(INFINITE); }

			HRESULT STDMETHODCALLTYPE WaitFor(const UINT32 intervalMilliseconds)
			{
				DWORD win32ErrCode{ ERROR_SUCCESS };
				do
				{ win32ErrCode = WaitForSingleObjectEx(m_EventHandle, intervalMilliseconds, true); }
				while (win32ErrCode == WAIT_IO_COMPLETION);
				return HRESULT_FROM_WIN32(win32ErrCode != WAIT_FAILED ? win32ErrCode : GetLastError());
			}

			HRESULT STDMETHODCALLTYPE ManualResetEvent()
			{ return ResetEvent(m_EventHandle) ? S_OK : HRESULT_FROM_WIN32(GetLastError()); }

			//IAsyncOperationWithProgressCompletedHandler<T>
			HRESULT STDMETHODCALLTYPE Invoke(IBase* asyncInfo, ABI::Windows::Foundation::AsyncStatus status)
			{
				SetEvent(m_EventHandle);
				return S_OK;
			}

			static HRESULT STDMETHODCALLTYPE CreateInstance(const bool manualReset, const bool initialState,
				AsyncOperationWithProgressCompletedHandlerInternal*& result)
			{
				const HANDLE hEvent{ CreateEventW(nullptr, manualReset, initialState, nullptr) };
				if (hEvent)
				{
					result = new AsyncOperationWithProgressCompletedHandlerInternal{ hEvent };
					if (result)
					{ return S_OK; }
					else
					{
						CloseHandle(hEvent);
						return E_OUTOFMEMORY;
					}
				}
				else
				{ return HRESULT_FROM_WIN32(GetLastError()); }
			}

			~AsyncOperationWithProgressCompletedHandlerInternal() noexcept
			{ CloseHandle(m_EventHandle); }

		private:
			const HANDLE m_EventHandle{ nullptr };

			AsyncOperationWithProgressCompletedHandlerInternal(const HANDLE hEvent) noexcept : m_EventHandle(hEvent)
			{ }
		};

	public:
		static HRESULT STDMETHODCALLTYPE AwaitAsyncObject(IBase* const async, TResult_ABI& result)
		{
			ABI::Windows::Foundation::AsyncStatus status{ ABI::Windows::Foundation::AsyncStatus::Started };
			HRESULT hr{ GetAsyncStatus(async, status) };
			if (SUCCEEDED(hr))
			{
				if (status == ABI::Windows::Foundation::AsyncStatus::Started)
				{
					AsyncOperationWithProgressCompletedHandlerInternal* handler{ nullptr };
					hr = AsyncOperationWithProgressCompletedHandlerInternal::CreateInstance(false, false, handler);
					if (SUCCEEDED(hr))
					{
						hr = async->put_Completed(handler);
						if (SUCCEEDED(hr))
						{ hr = handler->Wait(); }
						handler->Release();
					}
				}
				if (SUCCEEDED(hr))
				{
					hr = GetAsyncStatus(async, status);
					if (SUCCEEDED(hr))
					{
						switch (status)
						{
						case ABI::Windows::Foundation::AsyncStatus::Completed:
							hr = async->GetResults(&result);
							break;

						case ABI::Windows::Foundation::AsyncStatus::Error:
							GetErrorCode(async, hr);
							break;

						case ABI::Windows::Foundation::AsyncStatus::Canceled:
							hr = E_CANCELED;
							break;
						}
					}
				}
			}
			return hr;
		}

		static HRESULT STDMETHODCALLTYPE GetAsyncStatus(IBase* const async, ABI::Windows::Foundation::AsyncStatus& status)
		{
			ABI::Windows::Foundation::IAsyncInfo* asyncInfo{ nullptr };
			async->QueryInterface(__uuidof(asyncInfo), to_void_pp(asyncInfo));
			const HRESULT hr{ asyncInfo->get_Status(&status) };
			asyncInfo->Release();
			return hr;
		}

		static HRESULT STDMETHODCALLTYPE GetErrorCode(IBase* const async, HRESULT& errorCode)
		{
			ABI::Windows::Foundation::IAsyncInfo* asyncInfo{ nullptr };
			async->QueryInterface(__uuidof(asyncInfo), to_void_pp(asyncInfo));
			const HRESULT hr{ asyncInfo->get_ErrorCode(&errorCode) };
			asyncInfo->Release();
			return hr;
		}

	public:
		static void STDMETHODCALLTYPE CloseAsyncObject(IBase* const async)
		{
			ABI::Windows::Foundation::IAsyncInfo* asyncInfo{ nullptr };
			async->QueryInterface(__uuidof(asyncInfo), to_void_pp(asyncInfo));
			asyncInfo->Close();
			asyncInfo->Release();
		}

		AsyncOperationWithProgressUtils() = delete;
		AsyncOperationWithProgressUtils(const AsyncOperationWithProgressUtils&) = delete;
		AsyncOperationWithProgressUtils(AsyncOperationWithProgressUtils&&) = delete;
		AsyncOperationWithProgressUtils operator =(const AsyncOperationWithProgressUtils&) = delete;
		AsyncOperationWithProgressUtils operator =(AsyncOperationWithProgressUtils&&) = delete;
		~AsyncOperationWithProgressUtils() = delete;
	};
}
