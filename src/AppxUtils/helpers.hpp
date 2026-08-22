#pragma once

template<typename T>
inline constexpr void** to_void_pp(T*& p)
{ return reinterpret_cast<void**>(&p); }
