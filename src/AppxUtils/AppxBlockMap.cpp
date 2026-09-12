// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
#include "AppxBlockMapFile.h"
#include "AppxBlockMap.h"
#include "helpers.hpp"

namespace ABI
{
	using namespace Windows::Foundation;
	using namespace Windows::Foundation::Collections;
	using namespace Windows::Storage::Streams;
	using namespace AppxUtils::Internal;
}

namespace ABI::AppxUtils::Internal
{
#pragma region AppxBlockMapFileKVPair
#pragma region IKeyValuePair
	HRESULT STDMETHODCALLTYPE AppxBlockMapFileKVPair::get_Key(HSTRING* key)
	{ return WindowsDuplicateString(m_Key, key); }

	HRESULT STDMETHODCALLTYPE AppxBlockMapFileKVPair::get_Value(AppxBlockMapFile** value)
	{
		m_Value->AddRef();
		*value = m_Value;
		return S_OK;
	}
#pragma endregion

#pragma region IInspectable
	HRESULT STDMETHODCALLTYPE AppxBlockMapFileKVPair::GetRuntimeClassName(HSTRING* className)
	{ return WindowsCreateString(L"Windows.Foundation.Collections.IKeyValuePair`2<String, AppxUtils.AppxBlockMapFile>", 82, className); }
#pragma endregion

	//Destructor
	AppxBlockMapFileKVPair::~AppxBlockMapFileKVPair() noexcept
	{
		WindowsDeleteString(m_Key);
		m_Value->Release();
	}
#pragma endregion

#pragma region AppxBlockMapFileMap
#pragma region IMapView<String, AppxBlockMapFile>
	HRESULT STDMETHODCALLTYPE AppxBlockMapFileMap::Lookup(K_ABI key, V_ABI* value)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE AppxBlockMapFileMap::get_Size(UINT32* size)
	{
		*size = m_EndIndex;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxBlockMapFileMap::HasKey(K_ABI key, boolean* found)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE AppxBlockMapFileMap::Split(IMapView<K_Logical, V_Logical>** firstPartition, IMapView<K_Logical, V_Logical>** secondPartition)
	{
		return E_NOTIMPL;
	}
#pragma endregion

#pragma region IIterator
	HRESULT STDMETHODCALLTYPE AppxBlockMapFileMap::First(ABI::Windows::Foundation::Collections::IIterator<IKVPair>** first)
	{
		return E_NOTIMPL;
	}
#pragma endregion

#pragma region IInspectable
	HRESULT STDMETHODCALLTYPE AppxBlockMapFileMap::GetRuntimeClassName(HSTRING* className)
	{ return WindowsCreateString(L"Windows.Foundation.Collections.IMapView`2<String, AppxUtils.AppxBlockMapFile>", 77, className); }
#pragma endregion
#pragma endregion
}

namespace ABI::AppxUtils
{
#pragma region IAppxBlockMap
	HRESULT STDMETHODCALLTYPE AppxBlockMap::get_HashMethod(HSTRING* value)
	{

	}

	HRESULT STDMETHODCALLTYPE AppxBlockMap::get_Files(ABI::IMapView<HSTRING, AppxBlockMapFile*>** value)
	{

	}

	HRESULT STDMETHODCALLTYPE AppxBlockMap::GetBlockMapStream(ABI::IInputStream** result)
	{

	}
#pragma endregion

#pragma region IInspectable
	HRESULT STDMETHODCALLTYPE AppxBlockMap::GetRuntimeClassName(HSTRING* className)
	{ return WindowsCreateString(L"AppxUtils.AppxBlockMap", 22, className); }
#pragma endregion
}
