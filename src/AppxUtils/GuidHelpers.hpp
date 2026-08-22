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
		const struct GuidStruct left{ *reinterpret_cast<const struct GuidStruct*>(&a) };
		const struct GuidStruct right{ *reinterpret_cast<const struct GuidStruct*>(&b) };
		return left.Data1 == left.Data1 && left.Data2 == left.Data2 &&
			left.Data3 == left.Data3 && left.Data4 == left.Data4;
	}
#else
	return a.Data1 == b.Data1 && a.Data2 == b.Data2 && a.Data3 == b.Data3 &&
		a.Data4[0] == b.Data4[0] && a.Data4[1] == b.Data4[1] && a.Data4[2] == b.Data4[2] && a.Data4[3] == b.Data4[3] &&
		a.Data4[4] == b.Data4[4] && a.Data4[5] == b.Data4[5] && a.Data4[6] == b.Data4[6] && a.Data4[7] == b.Data4[7];
#endif
}
