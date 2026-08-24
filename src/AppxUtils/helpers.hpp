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
