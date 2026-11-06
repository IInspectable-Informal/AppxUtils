// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
#include "AsyncActionUtils.h"

namespace ABI
{
	using namespace Windows::Foundation;
}

namespace ABI::AppxUtils::Internal
{
	class AsyncActionCompletedHandlerInternal final : public UnknownBase<ABI::IAsyncActionCompletedHandler>
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

		//IAsyncActionCompletedHandler<T>
		HRESULT STDMETHODCALLTYPE Invoke(ABI::IAsyncAction* asyncInfo, ABI::AsyncStatus status)
		{
			SetEvent(m_EventHandle);
			return S_OK;
		}

		static HRESULT STDMETHODCALLTYPE CreateInstance(const bool manualReset, const bool initialState,
			AsyncActionCompletedHandlerInternal*& result)
		{
			const HANDLE hEvent{ CreateEventW(nullptr, manualReset, initialState, nullptr) };
			if (hEvent)
			{
				result = new AsyncActionCompletedHandlerInternal{ hEvent };
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

		~AsyncActionCompletedHandlerInternal() noexcept
		{
			CloseHandle(m_EventHandle);
		}

	private:
		const HANDLE m_EventHandle{ nullptr };

		AsyncActionCompletedHandlerInternal(const HANDLE hEvent) noexcept : m_EventHandle(hEvent)
		{ }
	};

	HRESULT STDMETHODCALLTYPE AsyncActionUtils::AwaitAsyncObject(IBase* const async)
	{
		ABI::Windows::Foundation::AsyncStatus status{ ABI::Windows::Foundation::AsyncStatus::Started };
		HRESULT hr{ GetAsyncStatus(async, status) };
		if (SUCCEEDED(hr))
		{
			if (status == ABI::Windows::Foundation::AsyncStatus::Started)
			{
				AsyncActionCompletedHandlerInternal* handler{ nullptr };
				hr = AsyncActionCompletedHandlerInternal::CreateInstance(false, false, handler);
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
						hr = async->GetResults();
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

	HRESULT STDMETHODCALLTYPE AsyncActionUtils::GetAsyncStatus(IBase* const async, ABI::AsyncStatus& status)
	{
		ABI::IAsyncInfo* asyncInfo{ nullptr };
		async->QueryInterface(__uuidof(asyncInfo), to_void_pp(asyncInfo));
		const HRESULT hr{ asyncInfo->get_Status(&status) };
		asyncInfo->Release();
		return hr;
	}

	HRESULT STDMETHODCALLTYPE AsyncActionUtils::GetErrorCode(IBase* const async, HRESULT& errorCode)
	{
		ABI::IAsyncInfo* asyncInfo{ nullptr };
		async->QueryInterface(__uuidof(asyncInfo), to_void_pp(asyncInfo));
		const HRESULT hr{ asyncInfo->get_ErrorCode(&errorCode) };
		asyncInfo->Release();
		return hr;
	}

	void STDMETHODCALLTYPE AsyncActionUtils::CloseAsyncObject(IBase* const async)
	{
		ABI::IAsyncInfo* asyncInfo{ nullptr };
		async->QueryInterface(__uuidof(asyncInfo), to_void_pp(asyncInfo));
		asyncInfo->Close();
		asyncInfo->Release();
	}
}
