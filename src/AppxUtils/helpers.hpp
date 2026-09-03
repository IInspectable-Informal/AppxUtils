#pragma once

template<typename T>
inline constexpr void** to_void_pp(T*& p)
{ return reinterpret_cast<void**>(&p); }

constexpr inline UINT32 WStringLength(LPCWSTR wstr)
{
    UINT32 count{ 0 };
    if (wstr)
    {
        while (*(wstr + count))
        { ++count; }
    }
    return count;
}

constexpr inline ABI::Windows::ApplicationModel::PackageVersion UInt64ToPkgVer(UINT64 verNum)
{
    ABI::Windows::ApplicationModel::PackageVersion ver{};
    ver.Major = (verNum >> 48) & 0xFFFF;
    ver.Minor = (verNum >> 32) & 0xFFFF;
    ver.Build = (verNum >> 16) & 0xFFFF;
    ver.Revision = verNum & 0xFFFF;
    return ver;
}
