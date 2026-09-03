#pragma once

namespace ABI::AppxUtils::Internal
{
	template<typename T>
	class StructLifetimeFunctions final
	{
		static_assert(!ABI::Windows::Foundation::Collections::is_pointer<T>::value && !__is_enum(T) && __is_trivial(T) && __is_standard_layout(T), "T must be a struct");
		static_assert(sizeof(T) == 0, "StructLifetimeFunctions<T> must be specialized!");

	public:
		static HRESULT STDMETHODCALLTYPE DeepCopyStruct(const T& source, T& target) = delete;
		static HRESULT STDMETHODCALLTYPE ReleaseStruct(const T& source) = delete;
		static HRESULT STDMETHODCALLTYPE IsEqualStruct(const T& a, const T& b, bool& result) = delete;

		StructLifetimeFunctions() = delete;
		StructLifetimeFunctions(const StructLifetimeFunctions&) = delete;
		StructLifetimeFunctions(StructLifetimeFunctions&&) = delete;
		~StructLifetimeFunctions() = delete;
		StructLifetimeFunctions& operator =(const StructLifetimeFunctions&) = delete;
		StructLifetimeFunctions& operator =(StructLifetimeFunctions&&) = delete;
	};
}
