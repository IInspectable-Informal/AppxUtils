// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <type_traits>

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
	struct RemovePointerHelper
	{ using type = T; };

	template<typename T>
	struct RemovePointerHelper<T*>
	{ using type = T; };

	template<typename T>
	struct RemovePointerHelper<T* const>
	{ using type = T; };

	template<typename T>
	struct RemovePointerHelper<T* volatile>
	{ using type = T; };

	template<typename T>
	struct RemovePointerHelper<T* const volatile>
	{ using type = T; };

	template<typename T>
	using RemovePointer = typename RemovePointerHelper<T>::type;

	template<typename I>
	concept ICOMInterface = __is_base_of(IUnknown, RemovePointer<I>);

	template<typename I>
	concept ICOMInterfaceNotIUnknown = (ICOMInterface<I> && !IsSameType<RemovePointer<I>, IUnknown>);

	template<typename I>
	concept IWinRTInterface = __is_base_of(IInspectable, RemovePointer<I>);

	template<typename I>
	concept IWinRTInterfaceNotIInspectable = (IWinRTInterface<I> && !IsSameType<RemovePointer<I>, IInspectable>);

	template<typename T, template<typename> class Tmpl>
	constexpr bool IsSpec1{ false };

	template<template<typename> class Tmpl, typename A>
	constexpr bool IsSpec1<Tmpl<A>, Tmpl>{ true };

	template<typename T, template<typename, typename> class Tmpl>
	constexpr bool IsSpec2{ false };

	template<template<typename, typename> class Tmpl, typename A, typename B>
	constexpr bool IsSpec2<Tmpl<A, B>, Tmpl>{ true };

	template<typename I>
	constexpr bool IsAsyncInterface
	{
		IsSameType<I, ABI::Windows::Foundation::IAsyncAction> ||
		IsSpec1<I, ABI::Windows::Foundation::IAsyncActionWithProgress> ||
		IsSpec1<I, ABI::Windows::Foundation::IAsyncOperation> ||
		IsSpec2<I, ABI::Windows::Foundation::IAsyncOperationWithProgress>
	};

	template<typename I>
	concept IAsyncInterface = IsAsyncInterface<I>;

	template<typename T>
	constexpr auto __aligned_size_of{ alignof(T) ? (sizeof(T) % alignof(T) ? (sizeof(T) / alignof(T) + 1) * alignof(T) : sizeof(T)) : 0 };

	template<typename T, typename = void>
	constexpr bool __is_complete_type{ false };

	template<typename T>
	constexpr bool __is_complete_type<T, decltype(void(sizeof(T)))>{ true };
}
