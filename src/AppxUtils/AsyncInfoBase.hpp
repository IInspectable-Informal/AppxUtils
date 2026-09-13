// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once

namespace ABI::AppxUtils::Internal
{
	template<typename IAsyncInterface>
	class AsyncInfoBase abstract : public InspectableBase<BaseTrust,
		IAsyncInterface,
		ABI::Windows::Foundation::IAsyncInfo,
		IAgileObject
	>
	{
	public:
		AsyncInfoBase() noexcept : m_Id(InterlockedIncrement(&s_AsyncInfoId))
		{

		}

		bool STDMETHODCALLTYPE get_CanContinue()
		{
			bool local{};
			AcquireSRWLockShared(&m_Lock);
			local = m_CanContinue;
			ReleaseSRWLockShared(&m_Lock);
			return local;
		}

		//IAsyncInfo
		HRESULT STDMETHODCALLTYPE get_Id(UINT32* id)
		{
			HRESULT hr{ S_OK };
			AcquireSRWLockShared(&m_Lock);
			if (m_IsOpening)
			{ *id = m_Id; }
			else
			{ hr = E_ILLEGAL_METHOD_CALL; }
			ReleaseSRWLockShared(&m_Lock);
			return hr;
		}

		HRESULT STDMETHODCALLTYPE get_Status(ABI::Windows::Foundation::AsyncStatus* status)
		{
			HRESULT hr{ S_OK };
			AcquireSRWLockShared(&m_Lock);
			if (m_IsOpening)
			{ *status = static_cast<ABI::Windows::Foundation::AsyncStatus>(m_Status); }
			else
			{ hr = E_ILLEGAL_METHOD_CALL; }
			ReleaseSRWLockShared(&m_Lock);
			return hr;
		}

		HRESULT STDMETHODCALLTYPE get_ErrorCode(HRESULT* errorCode)
		{
			HRESULT hr{ S_OK };
			AcquireSRWLockShared(&m_Lock);
			if (m_IsOpening)
			{ *errorCode = m_ErrorCode; }
			else
			{ hr = E_ILLEGAL_METHOD_CALL; }
			ReleaseSRWLockShared(&m_Lock);
			return hr;
		}

		HRESULT STDMETHODCALLTYPE Cancel()
		{
			AcquireSRWLockExclusive(&m_Lock);
			m_CanContinue = false;
			ReleaseSRWLockExclusive(&m_Lock);
			return S_OK;
		}

		virtual ~AsyncInfoBase() noexcept = default;

	protected:
		SRWLOCK m_Lock{ SRWLOCK_INIT };

		const UINT32 m_Id{ 0 };
		LONG m_Status{ static_cast<LONG>(ABI::Windows::Foundation::AsyncStatus::Started) };
		HRESULT m_ErrorCode{ S_OK };
		short m_CanContinue{ true };
		short m_IsOpening{ true };

	private:
		static inline UINT32 s_AsyncInfoId{ 0 };
	};
}
