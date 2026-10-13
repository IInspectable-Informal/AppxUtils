// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once

namespace ABI::AppxUtils::Internal
{
	class AppxBlockMapFileKVPair final : public InspectableElementBase<BaseTrust,
		ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING, AppxBlockMapFile*>,
		IAgileObject
	>
	{
	public:
		HRESULT STDMETHODCALLTYPE get_Key(HSTRING* key);
		HRESULT STDMETHODCALLTYPE get_Value(AppxBlockMapFile** value);

		//IInspectable
		HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className);

		~AppxBlockMapFileKVPair() noexcept;

	private:
		const HSTRING m_Key{ nullptr };
		AppxBlockMapFile* const m_Value{ nullptr };
	};

	class AppxBlockMapFileMap final : public InspectableBase<BaseTrust,
		ABI::Windows::Foundation::Collections::IMapView<HSTRING, AppxBlockMapFile*>,
		ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING, AppxBlockMapFile*>*>,
		IAgileObject
	>
	{
	private:
		using K = HSTRING;
		using K_ABI = typename Windows::Foundation::Internal::GetAbiType<K>::type;
		using K_Logical = typename Windows::Foundation::Internal::GetLogicalType<K>::type;
		using V = AppxBlockMapFile*;
		using V_ABI = typename Windows::Foundation::Internal::GetAbiType<V>::type;
		using V_Logical = typename Windows::Foundation::Internal::GetLogicalType<V>::type;
		using IKVPair = ABI::Windows::Foundation::Collections::IKeyValuePair<K_Logical, V_Logical>*;

		class InternalSplitedMapView final
		{

		};

	public:
		HRESULT STDMETHODCALLTYPE Lookup(K_ABI key, V_ABI* value);
		HRESULT STDMETHODCALLTYPE get_Size(UINT32* size);
		HRESULT STDMETHODCALLTYPE HasKey(K_ABI key, boolean* found);
		HRESULT STDMETHODCALLTYPE Split(IMapView<K_Logical, V_Logical>** firstPartition, IMapView<K_Logical, V_Logical>** secondPartition);

		//IIterable
		HRESULT STDMETHODCALLTYPE First(ABI::Windows::Foundation::Collections::IIterator<IKVPair>** first);

		//IInspectable
		HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className);

	private:
		UINT32 m_StartIndex{ 0 };
		UINT32 m_EndIndex{ 0 };
	};
}

namespace ABI::AppxUtils
{
	class AppxBlockMap final : public InspectableBase<BaseTrust,
		IAppxBlockMap,
		IAgileObject
	>
	{
	public:
		HRESULT STDMETHODCALLTYPE get_HashMethod(HSTRING* value);
		HRESULT STDMETHODCALLTYPE get_Files(ABI::Windows::Foundation::Collections::IMapView<HSTRING, AppxBlockMapFile*>** value);
		HRESULT STDMETHODCALLTYPE GetBlockMapStream(ABI::Windows::Storage::Streams::IInputStream** result);

		//IInspectable
		HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className);

	private:
		IAppxBlockMapReader* const m_BlockMapReader{ nullptr };
	};
}
