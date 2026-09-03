#pragma once

namespace ABI::AppxUtils::Internal
{
	template<typename T1, typename T2>
	constexpr bool IsSameType = false;

	template<typename T>
	constexpr bool IsSameType<T, T> = true;

	template<typename T>
	constexpr bool IsTypeWinRTString{ false };

	template<>
	constexpr bool IsTypeWinRTString<HSTRING>{ true };

	template<typename T>
	constexpr bool IsNumericType{ false };

	template<>
	constexpr bool IsNumericType<BYTE>{ true };

	template<>
	constexpr bool IsNumericType<INT16>{ true };

	template<>
	constexpr bool IsNumericType<UINT16>{ true };

	template<>
	constexpr bool IsNumericType<INT32>{ true };

	template<>
	constexpr bool IsNumericType<UINT32>{ true };

	template<>
	constexpr bool IsNumericType<INT64>{ true };

	template<>
	constexpr bool IsNumericType<UINT64>{ true };

	template<>
	constexpr bool IsNumericType<FLOAT>{ true };

	template<>
	constexpr bool IsNumericType<DOUBLE>{ true };

	template<typename T>
	constexpr bool IsFundamentalType{ false };

	template<>
	constexpr bool IsFundamentalType<BYTE>{ true };

	template<>
	constexpr bool IsFundamentalType<INT16>{ true };

	template<>
	constexpr bool IsFundamentalType<UINT16>{ true };

	template<>
	constexpr bool IsFundamentalType<INT32>{ true };

	template<>
	constexpr bool IsFundamentalType<UINT32>{ true };

	template<>
	constexpr bool IsFundamentalType<INT64>{ true };

	template<>
	constexpr bool IsFundamentalType<UINT64>{ true };

	template<>
	constexpr bool IsFundamentalType<FLOAT>{ true };

	template<>
	constexpr bool IsFundamentalType<DOUBLE>{ true };

	template<>
	constexpr bool IsFundamentalType<WCHAR>{ true };

	template<>
	constexpr bool IsFundamentalType<bool>{ true };

	template<typename T>
	constexpr auto __aligned_size_of{ alignof(T) ? (sizeof(T) % alignof(T) ? (sizeof(T) / alignof(T) + 1) * alignof(T) : sizeof(T)) : 0 };

	template<typename T>
	struct RemovePointer
	{ using type = T; };

	template<typename T>
	struct RemovePointer<T*>
	{ using type = T; };

	template<typename T>
	struct RemovePointer<T* const>
	{ using type = T; };

	template<typename T>
	struct RemovePointer<T* volatile>
	{ using type = T; };

	template<typename T>
	struct RemovePointer<T* const volatile>
	{ using type = T; };
}
