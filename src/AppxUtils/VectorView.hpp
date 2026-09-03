#pragma once
#include "dllmain.h"

namespace ABI::AppxUtils::Internal
{
	template<typename T>
	class VectorView : public InspectableBase<BaseTrust,
		ABI::Windows::Foundation::Collections::IVectorView<T>,
		ABI::Windows::Foundation::Collections::IIterable<T>,
		IAgileObject
	>
	{
	private:
		using T_Complex = typename ABI::Windows::Foundation::Collections::IVectorView<T>::T_complex;
		using T_ABI = typename ABI::Windows::Foundation::Internal::GetAbiType<T_Complex>::type;
		using T_Logical = typename ABI::Windows::Foundation::Internal::GetLogicalType<T_Complex>::type;

		class InternalIterator final : public InspectableBase<BaseTrust, ABI::Windows::Foundation::Collections::IIterator<T>, IAgileObject>
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
					if constexpr (IsFundamentalType<T_ABI> || __is_enum(T_ABI))
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
				if (InitOnceExecuteOnce(&s_InitOnce, InitStringStatic, nullptr, nullptr))
				{ return WindowsCreateString(s_ClassName, s_ClassNameSize, className); }
				else
				{ return HRESULT_FROM_WIN32(GetLastError()); }
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

			static inline INIT_ONCE s_InitOnce{ INIT_ONCE_STATIC_INIT };
			static inline const wchar_t* s_ClassName{ nullptr };
			static inline UINT32 s_ClassNameSize{ 0 };

			static BOOL WINAPI InitStringStatic(INIT_ONCE* InitOnce, void* Parameter, void** Context)
			{
				s_ClassName = ABI::Windows::Foundation::Collections::IIterator<T>::z_get_rc_name_impl();
				while (*(s_ClassName + s_ClassNameSize))
				{ ++s_ClassNameSize; }
				return true;
			}
		};

	public:
		HRESULT STDMETHODCALLTYPE GetAt(UINT32 index, T_ABI* item)
		{
			if (index < m_Size)
			{
				T_ABI& element = m_Array[index];
				if constexpr (IsFundamentalType<T_ABI> || __is_enum(T_ABI))
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
				if constexpr (IsFundamentalType<T_ABI> || __is_enum(T_ABI))
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
			if (InitOnceExecuteOnce(&s_InitOnce, InitStringStatic, nullptr, nullptr))
			{ return WindowsCreateString(s_ClassName, s_ClassNameSize, className); }
			else
			{ return HRESULT_FROM_WIN32(GetLastError()); }
		}

		virtual ~VectorView() noexcept
		{
			for (UINT32 i = 0; i < m_Size; ++i)
			{
				T& element{ m_Array[i] };
				if constexpr (!IsFundamentalType<T_ABI> && !__is_enum(T_ABI) && !ABI::Windows::Foundation::Collections::is_foundation_struct<T_ABI>::value && __is_trivial(T_ABI) && __is_standard_layout(T_ABI))
				{ StructLifetimeFunctions<T>::ReleaseStruct(element); }
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

		static inline INIT_ONCE s_InitOnce{ INIT_ONCE_STATIC_INIT };
		static inline const wchar_t* s_ClassName{ nullptr };
		static inline UINT32 s_ClassNameSize{ 0 };

		static BOOL WINAPI InitStringStatic(INIT_ONCE* InitOnce, void* Parameter, void** Context)
		{
			s_ClassName = ABI::Windows::Foundation::Collections::IVectorView<T>::z_get_rc_name_impl();
			while (*(s_ClassName + s_ClassNameSize))
			{ ++s_ClassNameSize; }
			return true;
		}
	};

	template<typename T>
	class VectorView<T*> : public InspectableBase<BaseTrust,
		ABI::Windows::Foundation::Collections::IVectorView<T*>,
		ABI::Windows::Foundation::Collections::IIterable<T*>,
		IAgileObject
	>
	{
	private:
		using T_Complex = typename ABI::Windows::Foundation::Collections::IVectorView<T*>::T_complex;
		using T_ABI = typename ABI::Windows::Foundation::Internal::GetAbiType<T_Complex>::type;
		using T_Logical = typename ABI::Windows::Foundation::Internal::GetLogicalType<T_Complex>::type;

		class InternalIterator final : public InspectableBase<BaseTrust, ABI::Windows::Foundation::Collections::IIterator<T*>, IAgileObject>
		{
		public:
			InternalIterator(T*& list, UINT32 size, VectorView<T*>* container) noexcept : m_Array(list), m_Size(size), m_Container(container)
			{
				container->AddRef();
			}

			HRESULT STDMETHODCALLTYPE get_Current(T_ABI* current)
			{
				if (m_Current < m_Size)
				{
					__if_exists(T)
					{
						T* element{ m_Array + m_Current };
						if constexpr (__is_base_of(IUnknown, RemovePointer<T_ABI>::type))
						{
							element->AddRef();
							*current = element;
						}
						else
						{ *current = element; }
					}
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
				if (InitOnceExecuteOnce(&s_InitOnce, InitStringStatic, nullptr, nullptr))
				{ return WindowsCreateString(s_ClassName, s_ClassNameSize, className); }
				else
				{ return HRESULT_FROM_WIN32(GetLastError()); }
			}

			~InternalIterator() noexcept
			{
				m_Container->Release();
			}

		private:
			T* m_Array{ nullptr };
			UINT32 m_Size{ 0 };
			UINT32 m_Current{ 0 };
			VectorView<T*>* m_Container{ nullptr };

			static inline INIT_ONCE s_InitOnce{ INIT_ONCE_STATIC_INIT };
			static inline const wchar_t* s_ClassName{ nullptr };
			static inline UINT32 s_ClassNameSize{ 0 };

			static BOOL WINAPI InitStringStatic(INIT_ONCE* InitOnce, void* Parameter, void** Context)
			{
				s_ClassName = ABI::Windows::Foundation::Collections::IIterator<T*>::z_get_rc_name_impl();
				while (*(s_ClassName + s_ClassNameSize))
				{ ++s_ClassNameSize; }
				return true;
			}
		};

	public:
		HRESULT STDMETHODCALLTYPE GetAt(UINT32 index, T_ABI* item)
		{
			if (index < m_Size)
			{
				__if_exists(T)
				{
					T* element{ m_Array + index };
					if constexpr (__is_base_of(IUnknown, RemovePointer<T_ABI>::type))
					{
						element->AddRef();
						*item = element;
					}
					else
					{
						*item = element;
					}
				}
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
				__if_exists(T)
				{
					T* element{ m_Array + i };
					if constexpr (__is_base_of(IUnknown, RemovePointer<T_ABI>::type))
					{
						if (element == value)
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
			if (InitOnceExecuteOnce(&s_InitOnce, InitStringStatic, nullptr, nullptr))
			{ return WindowsCreateString(s_ClassName, s_ClassNameSize, className); }
			else
			{ return HRESULT_FROM_WIN32(GetLastError()); }
		}

		virtual ~VectorView() noexcept
		{
			for (UINT32 i = 0; i < m_Size; ++i)
			{
				__if_exists(T)
				{
					T* element{ m_Array + i };
					if constexpr (__is_base_of(IUnknown, RemovePointer<T_ABI>::type))
					{ element->Release(); }
				}
			}
			delete[] m_Array;
			if (m_CriticalSections)
			{ delete[] m_CriticalSections; }
		}

		static HRESULT STDMETHODCALLTYPE CreateInstance(T* list, UINT32 size, VectorView<T*>*& result, CRITICAL_SECTION* criticalSections = nullptr) noexcept
		{
			VectorView<T*>* instance{ new VectorView<T*>(list, size, criticalSections) };
			if (instance)
			{
				result = instance;
				return S_OK;
			}
			else
			{ return E_OUTOFMEMORY; }
		}

		VectorView(T* list, UINT32 size, CRITICAL_SECTION* criticalSections = nullptr) noexcept : m_Array(list), m_Size(size), m_CriticalSections(criticalSections)
		{

		}

	private:
		T* m_Array{ nullptr };
		CRITICAL_SECTION* m_CriticalSections{ nullptr };
		UINT32 m_Size{ 0 };

		static inline INIT_ONCE s_InitOnce{ INIT_ONCE_STATIC_INIT };
		static inline const wchar_t* s_ClassName{ nullptr };
		static inline UINT32 s_ClassNameSize{ 0 };

		static BOOL WINAPI InitStringStatic(INIT_ONCE* InitOnce, void* Parameter, void** Context)
		{
			s_ClassName = ABI::Windows::Foundation::Collections::IVectorView<T*>::z_get_rc_name_impl();
			while (*(s_ClassName + s_ClassNameSize))
			{ ++s_ClassNameSize; }
			return true;
		}
	};

	template<>
	class VectorView<HSTRING> : public InspectableBase<BaseTrust,
		ABI::Windows::Foundation::Collections::IVectorView<HSTRING>,
		ABI::Windows::Foundation::Collections::IIterable<HSTRING>,
		IAgileObject
	>
	{
	private:
		class InternalIterator final : public InspectableBase<BaseTrust, ABI::Windows::Foundation::Collections::IIterator<HSTRING>, IAgileObject>
		{
		public:
			InternalIterator(HSTRING*& list, UINT32 size, VectorView<HSTRING>* container) noexcept : m_Array(list), m_Size(size), m_Container(container)
			{
				container->AddRef();
			}

			HRESULT STDMETHODCALLTYPE get_Current(HSTRING* current)
			{
				if (m_Current < m_Size)
				{ return WindowsDuplicateString(m_Array[m_Current], current); }
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

			HRESULT STDMETHODCALLTYPE GetMany(UINT32 capacity, HSTRING* value, UINT32* actual) override
			{
				return E_NOTIMPL;
			}

			//IInspectable
			HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className)
			{ return WindowsCreateString(L"Windows.Foundation.Collections.IIterator`1<String>", 50, className); }

			~InternalIterator() noexcept
			{
				m_Container->Release();
			}

		private:
			HSTRING* m_Array{ nullptr };
			UINT32 m_Size{ 0 };
			UINT32 m_Current{ 0 };
			VectorView<HSTRING>* m_Container{ nullptr };
		};

	public:
		HRESULT STDMETHODCALLTYPE GetAt(UINT32 index, HSTRING* item)
		{
			if (index < m_Size)
			{ return WindowsDuplicateString(m_Array[index], item); }
			else
			{ return E_BOUNDS; }
		}

		HRESULT STDMETHODCALLTYPE get_Size(UINT32* size)
		{
			*size = m_Size;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE IndexOf(HSTRING value, UINT32* index, boolean* found)
		{
			for (UINT32 i = 0; i < m_Size; ++i)
			{
				HSTRING& element{ m_Array[i] };
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
			*found = false;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE GetMany(UINT32 startIndex, UINT32 capacity, HSTRING* value, UINT32* actual) override
		{
			return E_NOTIMPL;
		}

		//IIterable
		HRESULT STDMETHODCALLTYPE First(ABI::Windows::Foundation::Collections::IIterator<HSTRING>** first)
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
			return WindowsCreateString(L"Windows.Foundation.Collections.IVectorView`1<String>", 52, className);
		}

		virtual ~VectorView() noexcept
		{
			for (UINT32 i = 0; i < m_Size; ++i)
			{ WindowsDeleteString(m_Array[i]); }
			delete[] m_Array;
		}

		static HRESULT STDMETHODCALLTYPE CreateInstance(HSTRING* list, UINT32 size, VectorView<HSTRING>*& result) noexcept
		{
			VectorView<HSTRING>* instance{ new VectorView<HSTRING>(list, size) };
			if (instance)
			{
				result = instance;
				return S_OK;
			}
			else
			{ return E_OUTOFMEMORY; }
		}

		VectorView(HSTRING* list, UINT32 size) noexcept : m_Array(list), m_Size(size)
		{

		}

	private:
		HSTRING* m_Array{ nullptr };
		UINT32 m_Size{ 0 };
	};
}
