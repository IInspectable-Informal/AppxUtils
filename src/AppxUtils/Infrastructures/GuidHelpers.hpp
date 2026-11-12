// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once

constexpr inline bool IsEqualGuid(const GUID& a, const GUID& b)
{
#if __cplusplus >= 202302L
	if consteval
	{
		return a.Data1 == b.Data1 && a.Data2 == b.Data2 && a.Data3 == b.Data3 &&
			a.Data4[0] == b.Data4[0] && a.Data4[1] == b.Data4[1] && a.Data4[2] == b.Data4[2] && a.Data4[3] == b.Data4[3] &&
			a.Data4[4] == b.Data4[4] && a.Data4[5] == b.Data4[5] && a.Data4[6] == b.Data4[6] && a.Data4[7] == b.Data4[7];
	}
	else
	{
		struct GuidStruct
		{
			UINT32 Data1;
			UINT32 Data2;
			UINT32 Data3;
			UINT32 Data4;
		};
		const struct GuidStruct& left{ *reinterpret_cast<const struct GuidStruct*>(&a) };
		const struct GuidStruct& right{ *reinterpret_cast<const struct GuidStruct*>(&b) };
		return left.Data1 == right.Data1 && left.Data2 == right.Data2 &&
			left.Data3 == right.Data3 && left.Data4 == right.Data4;
	}
#else
	const unsigned char(&aData4)[8]{ a.Data4 };
	const unsigned char(&bData4)[8]{ b.Data4 };
	return a.Data1 == b.Data1 && a.Data2 == b.Data2 && a.Data3 == b.Data3 &&
		aData4[0] == bData4[0] && aData4[1] == bData4[1] && aData4[2] == bData4[2] && aData4[3] == bData4[3] &&
		aData4[4] == bData4[4] && aData4[5] == bData4[5] && aData4[6] == bData4[6] && aData4[7] == bData4[7];
#endif
}

constexpr inline void CopyGuid(const GUID& source, GUID& target)
{
#if __cplusplus >= 202302L
	if consteval
	{
		target.Data1 = source.Data1;
		target.Data2 = source.Data2;
		target.Data3 = source.Data3;
		unsigned char(&targetData4)[8]{ target.Data4 };
		const unsigned char(&sourceData4)[8]{ source.Data4 };
		for (UINT8 i{ 0 }; i < 8; ++i)
		{ targetData4[i] = sourceData4[i]; }
	}
	else
	{
		struct GuidStruct
		{
			UINT32 Data1;
			UINT32 Data2;
			UINT32 Data3;
			UINT32 Data4;
		};
		const struct GuidStruct& left{ *reinterpret_cast<const struct GuidStruct*>(&source) };
		struct GuidStruct& right{ *reinterpret_cast<struct GuidStruct*>(&target) };
		right.Data1 = left.Data1;
		right.Data2 = left.Data2;
		right.Data3 = left.Data3;
		right.Data4 = left.Data4;
	}
#else
	target.Data1 = source.Data1;
	target.Data2 = source.Data2;
	target.Data3 = source.Data3;
	const unsigned char(&sourceData4)[8]{ source.Data4 };
	unsigned char (&targetData4)[8]{ target.Data4 };
	for (UINT8 i{ 0 }; i < 8; ++i)
	{ targetData4[i] = sourceData4[i]; }
#endif
}
