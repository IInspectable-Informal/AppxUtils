// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
#include "AsyncActionBase.h"

namespace ABI
{
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Collections;
}

namespace ABI::AppxUtils::Internal
{
#pragma region IAsyncAction
    HRESULT STDMETHODCALLTYPE AsyncActionBase::put_Completed(ABI::IAsyncActionCompletedHandler* handler)
    {
        if (handler)
        { handler->AddRef(); }
        ABI::AsyncStatus status{};
        HRESULT hr{ S_OK };
        AcquireSRWLockExclusive(&m_Lock);
        if (m_IsOpening)
        {
            if (m_Completed)
            { hr = E_ILLEGAL_DELEGATE_ASSIGNMENT; }
            else
            {
                m_Completed = handler;
                status = static_cast<ABI::AsyncStatus>(m_Status);
            }
        }
        else
        { hr = E_ILLEGAL_METHOD_CALL; }
        ReleaseSRWLockExclusive(&m_Lock);
        if (SUCCEEDED(hr))
        {
            if (status != ABI::AsyncStatus::Started && handler)
            {
                handler->Invoke(this, status);
                handler->Release();
                AcquireSRWLockExclusive(&m_Lock);
                m_Completed = nullptr;
                ReleaseSRWLockExclusive(&m_Lock);
            }
        }
        else if (handler)
        { handler->Release(); }
        return hr;
    }

    HRESULT STDMETHODCALLTYPE AsyncActionBase::get_Completed(ABI::IAsyncActionCompletedHandler** handler)
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

    HRESULT STDMETHODCALLTYPE AsyncActionBase::GetResults()
    {
        HRESULT hr{ S_OK };
        AcquireSRWLockShared(&m_Lock);
        if (m_IsOpening)
        {
            switch (static_cast<ABI::AsyncStatus>(m_Status))
            {
            case ABI::AsyncStatus::Completed:
                hr = S_OK;
                break;

            case ABI::AsyncStatus::Error:
                hr = m_ErrorCode;
                break;

            case ABI::AsyncStatus::Canceled:
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
#pragma endregion

#pragma region IInspectable
    HRESULT STDMETHODCALLTYPE AsyncActionBase::GetRuntimeClassName(HSTRING* className)
    {
        constexpr wchar_t classNameRaw[]{ L"Windows.Foundation.IAsyncAction" };
        return WindowsCreateString(classNameRaw, sizeof(classNameRaw) / sizeof(wchar_t) - 1, className);
    }
#pragma endregion

    //Destructractor
    AsyncActionBase::~AsyncActionBase() noexcept
    {
        if (m_Completed)
        { m_Completed->Release(); }
    }
}
