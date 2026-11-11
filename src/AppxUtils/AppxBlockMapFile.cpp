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
	AppxBlockMapBlock::AppxBlockMapBlock(const UINT32 compressedSize, const UINT32 hashArraySize, BYTE* const hashArray) noexcept :
		m_CompressedSize(compressedSize), m_HashArraySize(hashArraySize), m_HashArray(hashArray)
	{

	}

#pragma region IAppxBlockMapBlockRuntimeClass
	HRESULT STDMETHODCALLTYPE AppxBlockMapBlock::get_Hash(UINT32* valueLength, byte** value)
	{
		auto* const arrayCopy{ static_cast<byte*>(CoTaskMemAlloc(__aligned_size_of<byte> * m_HashArraySize)) };
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
	AppxBlockMapFile::AppxBlockMapFile(IAppxBlockMapFile* const blockMapFile) noexcept : m_BlockMapFile(blockMapFile)
	{

	}

#pragma region IAppxBlockMapFileRuntimeClass
	HRESULT STDMETHODCALLTYPE AppxBlockMapFile::get_LocalFileHeaderSize(UINT32* value)
	{
		auto local{ InterlockedCompareExchange16(&m_HasLocalFileHeaderSize, false, false) };
		if (!local)
		{
			HRESULT hr{ S_OK };
			AcquireSRWLockExclusive(&m_Lock);
			if (!m_HasLocalFileHeaderSize)
			{
				hr = m_BlockMapFile->GetLocalFileHeaderSize(&m_LocalFileHeaderSize);
				if (SUCCEEDED(hr))
				{ m_HasLocalFileHeaderSize = true; }
			}
			ReleaseSRWLockExclusive(&m_Lock);
			if (FAILED(hr))
			{ return hr; }
		}
		*value = m_LocalFileHeaderSize;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxBlockMapFile::get_UncompressedSize(UINT64* value)
	{
		auto local{ InterlockedCompareExchange16(&m_HasUncompressedSize, false, false) };
		if (!local)
		{
			HRESULT hr{ S_OK };
			AcquireSRWLockExclusive(&m_Lock);
			if (!m_HasUncompressedSize)
			{
				hr = m_BlockMapFile->GetUncompressedSize(&m_UncompressedSize);
				if (SUCCEEDED(hr))
				{ m_HasUncompressedSize = true; }
			}
			ReleaseSRWLockExclusive(&m_Lock);
			if (FAILED(hr))
			{ return hr; }
		}
		*value = m_UncompressedSize;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxBlockMapFile::get_Blocks(ABI::IVectorView<AppxBlockMapBlock*>** value)
	{
		auto* local{ static_cast<ABI::IVectorView<AppxBlockMapBlock*>*>(InterlockedCompareExchangePointer(to_void_pp(m_Blocks), nullptr, nullptr)) };
		if (!local)
		{
			HRESULT hr{ S_OK };
			AcquireSRWLockExclusive(&m_Lock);
			if (!m_Blocks)
			{
				IAppxBlockMapBlocksEnumerator* enumerator{ nullptr };
				hr = m_BlockMapFile->GetBlocks(&enumerator);
				if (SUCCEEDED(hr))
				{
					UINT32 count{ 0 };
					BOOL hasNext{ false };
					enumerator->GetHasCurrent(&hasNext);
					while (hasNext)
					{
						++count;
						enumerator->MoveNext(&hasNext);
					}
					enumerator->Release();
					hr = m_BlockMapFile->GetBlocks(&enumerator);
					if (SUCCEEDED(hr))
					{
						auto* const blocks{ static_cast<AppxBlockMapBlock*>(::operator new[](__aligned_size_of<AppxBlockMapBlock> * count)) };
						if (blocks)
						{
							enumerator->GetHasCurrent(&hasNext);
							UINT32 completed{ 0 };
							while (hasNext)
							{
								IAppxBlockMapBlock* element{ nullptr };
								hr = enumerator->GetCurrent(&element);
								if (SUCCEEDED(hr))
								{
									UINT32 compressedSize{};
									hr = element->GetCompressedSize(&compressedSize);
									if (SUCCEEDED(hr))
									{
										UINT32 hashArraySize{};
										BYTE* hashArray{ nullptr };
										hr = element->GetHash(&hashArraySize, &hashArray);
										if (SUCCEEDED(hr))
										{
											new (blocks + completed) AppxBlockMapBlock{ compressedSize, hashArraySize, hashArray };
											++completed;
											enumerator->MoveNext(&hasNext);
										}
									}
									element->Release();
								}
								if (FAILED(hr))
								{
									for (UINT32 i{ completed }; i > 0;)
									{ blocks[--i].Release(); }
									::operator delete[](blocks);
									break;
								}
							}
							if (SUCCEEDED(hr))
							{
								m_Blocks = new VectorView<AppxBlockMapBlock*>{ blocks, count };
								if (!m_Blocks)
								{
									hr = E_OUTOFMEMORY;
									for (UINT32 i{ completed }; i > 0;)
									{ blocks[--i].Release(); }
									::operator delete[](blocks);
								}
							}
						}
						else
						{ hr = E_OUTOFMEMORY; }
						enumerator->Release();
					}
				}
			}
			local = m_Blocks;
			ReleaseSRWLockExclusive(&m_Lock);
			if (FAILED(hr))
			{ return hr; }
		}
		local->AddRef();
		*value = local;
		return S_OK;
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
