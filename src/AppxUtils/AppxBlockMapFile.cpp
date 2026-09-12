// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
#include "AppxBlockMapFile.h"
#include "helpers.hpp"

namespace ABI
{
	using namespace Windows::Foundation;
	using namespace Windows::Foundation::Collections;
	using namespace Windows::Storage::Streams;
	using namespace AppxUtils::Internal;
}

namespace ABI::AppxUtils
{
#pragma region AppxBlockMapBlock
	AppxBlockMapBlock::AppxBlockMapBlock(const UINT32 compressedSize, const UINT32 hashArraySize, BYTE* hashArray) noexcept :
		m_CompressedSize(compressedSize), m_HashArraySize(hashArraySize), m_HashArray(hashArray)
	{

	}

#pragma region IAppxBlockMapBlockRuntimeClass
	HRESULT STDMETHODCALLTYPE AppxBlockMapBlock::get_Hash(UINT32* valueLength, byte** value)
	{
		auto* arrayCopy{ reinterpret_cast<byte*>(CoTaskMemAlloc(__aligned_size_of<byte> * m_HashArraySize)) };
		if (arrayCopy)
		{
			for (UINT32 i{ 0 }; i < m_HashArraySize; ++i)
			{ arrayCopy[i] = m_HashArray[i]; }
			*valueLength = m_HashArraySize;
			*value = arrayCopy;
			return S_OK;
		}
		else
		{ return E_OUTOFMEMORY; }
	}

	HRESULT STDMETHODCALLTYPE AppxBlockMapBlock::get_CompressedSize(UINT32* value)
	{
		*value = m_CompressedSize;
		return S_OK;
	}
#pragma endregion

#pragma region IInspectable
	HRESULT STDMETHODCALLTYPE AppxBlockMapBlock::GetRuntimeClassName(HSTRING* className)
	{ return WindowsCreateString(L"AppxUtils.AppxBlockMapBlock", 27, className); }
#pragma endregion

	//Destructor
	AppxBlockMapBlock::~AppxBlockMapBlock() noexcept
	{ CoTaskMemFree(m_HashArray); }
#pragma endregion

#pragma region AppxBlockMapFile
#pragma region IAppxBlockMapFileRuntimeClass
	HRESULT STDMETHODCALLTYPE AppxBlockMapFile::get_LocalFileHeaderSize(UINT32* value)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE AppxBlockMapFile::get_UncompressedSize(UINT64* value)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE AppxBlockMapFile::get_Blocks(ABI::IVectorView<AppxBlockMapBlock*>** value)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE AppxBlockMapFile::ValidateFileHash(ABI::IInputStream* fileStream, boolean* result)
	{
		IStream* stream{ nullptr };
		HRESULT hr{ CreateStreamOverRandomAccessStream(fileStream, __uuidof(stream), to_void_pp(stream)) };
		if (SUCCEEDED(hr))
		{
			BOOL result0{ false };
			hr = m_BlockMapFile->ValidateFileHash(stream, &result0);
			if (SUCCEEDED(hr))
			{ *result = result0; }
			stream->Release();
		}
		return hr;
	}
#pragma endregion

#pragma region IInspectable
	HRESULT STDMETHODCALLTYPE AppxBlockMapFile::GetRuntimeClassName(HSTRING* className)
	{ return WindowsCreateString(L"AppxUtils.AppxBlockMapFile", 26, className); }
#pragma endregion

	//Destructor
	AppxBlockMapFile::~AppxBlockMapFile() noexcept
	{
		if (m_Blocks)
		{ m_Blocks->Release(); }
		m_BlockMapFile->Release();
	}
#pragma endregion
}
