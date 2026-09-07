// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once

namespace ABI::AppxUtils
{
	class AppxBlockMapBlock final : public InspectableElementBase<BaseTrust,
		IAppxBlockMapBlockRuntimeClass,
		IAgileObject
	>
	{
	public:
		AppxBlockMapBlock(const UINT32 compressedSize, const UINT32 hashArraySize, BYTE* hashArray) noexcept;

		HRESULT STDMETHODCALLTYPE get_Hash(UINT32* valueLength, byte** value);
		HRESULT STDMETHODCALLTYPE get_CompressedSize(UINT32* value);

		//IInspectable
		HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className);

		~AppxBlockMapBlock() noexcept;

	private:
		const UINT32 m_CompressedSize{ 0 };
		const UINT32 m_HashArraySize{ 0 };
		byte* m_HashArray{ nullptr };
	};

	class AppxBlockMapFile final : public InspectableElementBase<BaseTrust,
		IAppxBlockMapFileRuntimeClass,
		IAgileObject
	>
	{
	public:
		HRESULT STDMETHODCALLTYPE get_LocalFileHeaderSize(UINT32* value);
		HRESULT STDMETHODCALLTYPE get_UncompressedSizeSize(UINT64* value);
		HRESULT STDMETHODCALLTYPE get_Blocks(ABI::Windows::Foundation::Collections::IVectorView<AppxBlockMapBlock*>** value);

		HRESULT STDMETHODCALLTYPE ValidateFileHash(ABI::Windows::Storage::Streams::IInputStream* fileStream, boolean* result);

		//IInspectable
		HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className);
	};
}
