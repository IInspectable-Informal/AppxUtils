#pragma once
#include "dllmain.h"

namespace ABI::AppxUtils::Internal
{
	template<typename T>
	class VectorView : public InspectableBase<BaseTrust,
		ABI::Windows::Foundation::Collections::IVectorView<T>,
		ABI::Windows::Foundation::Collections::IIterable<T>
	>
	{
	private:
		using T_Complex = typename ABI::Windows::Foundation::Collections::IVectorView<T>::T_complex;
		using T_ABI = typename ABI::Windows::Foundation::Internal::GetAbiType<T_Complex>::type;
		using T_Logical = typename ABI::Windows::Foundation::Internal::GetLogicalType<T_Complex>::type;

		class InternalIterator final : public InspectableBase<BaseTrust, ABI::Windows::Foundation::Collections::IIterator<T>>
		{
		public:
			InternalIterator(T*& list, UINT32 size, VectorView<T>* container) noexcept : m_Array(list), m_Size(size), m_Container(container)
			{
				container->AddRef();
			}

			HRESULT STDMETHODCALLTYPE get_Current(T_ABI* current)
			{
				if (m_Current < m_Size)
				{
					T_ABI& element{ m_Array[m_Current] };
					if constexpr (IsTypeWinRTString<T_ABI>)
					{ return WindowsDuplicateString(element, current); }
					else if constexpr (ABI::Windows::Foundation::Collections::is_pointer<T_ABI>::value && __is_base_of(RemovePointer<T_ABI>::type, IUnknown))
					{
						element->AddRef();
						*current = element;
					}
					else if constexpr (IsFundamentalType<T_ABI> || __is_enum(T_ABI))
					{ *current = element; }
					else if constexpr (ABI::Windows::Foundation::Collections::is_foundation_struct<T_ABI>::value)
					{ *current = element; }
					else if constexpr (__is_trivial(T_ABI) && __is_standard_layout(T_ABI))
					{
						HRESULT hr{ StructLifetimeFunctions<T>::DeepCopyStruct(element, *current) };
						if (FAILED(hr))
						{ return hr; }
					}
					else
					{ *current = element; }
					return S_OK;
				}
				else
				{ return E_BOUNDS; }
			}

			HRESULT STDMETHODCALLTYPE get_HasCurrent(boolean* hasCurrent)
			{
				*hasCurrent = m_Current < m_Size;
				return S_OK;
			}

			HRESULT STDMETHODCALLTYPE MoveNext(boolean* hasCurrent)
			{
				if (m_Current <= m_Size)
				{
					++m_Current;
					*hasCurrent = m_Current < m_Size;
					return S_OK;
				}
				else
				{ return E_BOUNDS; }
			}

			HRESULT STDMETHODCALLTYPE GetMany(UINT32 capacity, T_ABI* value, UINT32* actual) override
			{
				return E_NOTIMPL;
			}

			//IInspectable
			HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className)
			{
				if (!m_ClassName)
				{
					m_ClassName = ABI::Windows::Foundation::Collections::IIterator<T>::z_get_rc_name_impl();
					while (*(m_ClassName + m_ClassNameSize))
					{ ++m_ClassNameSize; }
				}
				return WindowsCreateString(m_ClassName, m_ClassNameSize, className);
			}

			~InternalIterator() noexcept
			{
				m_Container->Release();
			}

		private:
			T* m_Array{ nullptr };
			UINT32 m_Size{ 0 };
			UINT32 m_Current{ 0 };
			VectorView<T>* m_Container{ nullptr };

			const wchar_t* m_ClassName{ nullptr };
			UINT32 m_ClassNameSize{ 0 };
		};

	public:
		HRESULT STDMETHODCALLTYPE GetAt(UINT32 index, T_ABI* item)
		{
			if (index < m_Size)
			{
				T_ABI& element = m_Array[index];
				if constexpr (IsTypeWinRTString<T_ABI>)
				{
					//WriteConsoleW(GetStdHandle(-11), L"test", 4, nullptr, nullptr);
					//*item = element;
					//return S_OK;
					return WindowsDuplicateString(element, item);
				}
				else if constexpr (ABI::Windows::Foundation::Collections::is_pointer<T_ABI>::value && __is_base_of(RemovePointer<T_ABI>::type, IUnknown))
				{
					element->AddRef();
					*item = element;
				}
				else if constexpr (IsFundamentalType<T_ABI> || __is_enum(T_ABI))
				{ *item = element; }
				else if constexpr (ABI::Windows::Foundation::Collections::is_foundation_struct<T_ABI>::value)
				{ *item = element; }
				else if constexpr (__is_trivial(T_ABI) && __is_standard_layout(T_ABI))
				{
					HRESULT hr{ StructLifetimeFunctions<T>::DeepCopyStruct(element, *item) };
					if (FAILED(hr))
					{ return hr; }
				}
				else
				{ *item = element; }
				return S_OK;
			}
			else
			{ return E_BOUNDS; }
		}

		HRESULT STDMETHODCALLTYPE get_Size(UINT32* size)
		{
			*size = m_Size;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE IndexOf(T_ABI value, UINT32* index, boolean* found)
		{
			for (UINT32 i = 0; i < m_Size; ++i)
			{
				T_ABI& element{ m_Array[i] };
				if constexpr (IsTypeWinRTString<T_ABI>)
				{
					if (element == value)
					{
						*index = i;
						*found = true;
						return S_OK;
					}
					else
					{
						INT32 result{ 0 };
						WindowsCompareStringOrdinal(element, value, &result);
						if (result == 0)
						{
							*index = i;
							*found = true;
							return S_OK;
						}
					}
				}
				else if constexpr (IsFundamentalType<T_ABI> || __is_enum(T_ABI))
				{
					if (element == value)
					{
						*index = i;
						*found = true;
						return S_OK;
					}
				}
				else if constexpr (IsSameType<T_ABI, GUID>)
				{
					if (IsEqualGuid(element, value))
					{
						*index = i;
						*found = true;
						return S_OK;
					}
				}
				else if constexpr (ABI::Windows::Foundation::Collections::is_foundation_struct<T_ABI>::value)
				{
					if (element == value)
					{
						*index = i;
						*found = true;
						return S_OK;
					}
				}
				else if constexpr (__is_trivial(T_ABI) && __is_standard_layout(T_ABI))
				{
					bool result{};
					StructLifetimeFunctions<T>::IsEqualStruct(element, value, result);
					if (result)
					{
						*index = i;
						*found = true;
						return S_OK;
					}
				}
				else
				{
					if (element == value)
					{
						*index = i;
						*found = true;
						return S_OK;
					}
				}
			}
			*found = false;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE GetMany(UINT32 startIndex, UINT32 capacity, T_ABI* value, UINT32* actual) override
		{
			return E_NOTIMPL;
		}

		//IIterable
		HRESULT STDMETHODCALLTYPE First(ABI::Windows::Foundation::Collections::IIterator<T_Logical>** first)
		{
			InternalIterator* iterator{ new InternalIterator(m_Array, m_Size, this) };
			if (iterator)
			{
				*first = iterator;
				return S_OK;
			}
			else
			{ return E_OUTOFMEMORY; }
		}

		//IInspectable
		HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className)
		{
			if (!m_ClassName)
			{
				m_ClassName = ABI::Windows::Foundation::Collections::IVectorView<T>::z_get_rc_name_impl();
				while (*(m_ClassName + m_ClassNameSize))
				{ ++m_ClassNameSize; }
			}
			return WindowsCreateString(m_ClassName, m_ClassNameSize, className);
		}

		virtual ~VectorView() noexcept
		{
			for (UINT32 i = 0; i < m_Size; ++i)
			{
				T& element{ m_Array[i] };
				if constexpr (IsTypeWinRTString<T_ABI>)
				{
					WindowsDeleteString(element);
				}
				else if constexpr (ABI::Windows::Foundation::Collections::is_pointer<T_ABI>::value && __is_base_of(RemovePointer<T_ABI>::type, IUnknown))
				{
					element->Release();
				}
				else if constexpr (!IsFundamentalType<T_ABI> && !__is_enum(T_ABI) && !ABI::Windows::Foundation::Collections::is_foundation_struct<T_ABI>::value && __is_trivial(T_ABI) && __is_standard_layout(T_ABI))
				{
					StructLifetimeFunctions<T>::ReleaseStruct(element);
				}
			}
			delete[] m_Array;
		}

		static HRESULT STDMETHODCALLTYPE CreateInstance(T*& list, UINT32 size, VectorView<T>*& result) noexcept
		{
			VectorView<T>* instance{ new VectorView<T>(list, size) };
			if (instance)
			{
				result = instance;
				return S_OK;
			}
			else
			{ return E_OUTOFMEMORY; }
		}
		
		VectorView(T*& list, UINT32 size) noexcept : m_Array(list), m_Size(size)
		{

		}

        private:
		T* m_Array{ nullptr };
		UINT32 m_Size{ 0 };

		const wchar_t* m_ClassName{ nullptr };
		UINT32 m_ClassNameSize{ 0 };
	};
}
