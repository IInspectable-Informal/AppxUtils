// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "helpers.hpp"

template<typename T>
inline void __stdcall TryCloseObject(T* const object)
{
	static_assert(__is_base_of(IUnknown, ABI::AppxUtils::Internal::RemovePointer<T>::type), "T must be a COM interface!");
	if (object)
	{
		ABI::Windows::Foundation::IClosable* closable{ nullptr };
		if (SUCCEEDED(object->QueryInterface(__uuidof(closable), to_void_pp(closable))))
		{
			closable->Close();
			closable->Release();
		}
	}
}
