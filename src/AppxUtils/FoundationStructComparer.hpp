#pragma once

//IsEqualFoundationStruct
constexpr inline bool operator ==(const struct ABI::Windows::Foundation::DateTime& a, const struct ABI::Windows::Foundation::DateTime& b)
{ return a.UniversalTime == b.UniversalTime; }

constexpr inline bool operator ==(const struct ABI::Windows::Foundation::TimeSpan& a, const struct ABI::Windows::Foundation::TimeSpan& b)
{ return a.Duration == b.Duration; }

constexpr inline bool operator ==(const struct ABI::Windows::Foundation::Point& a, const struct ABI::Windows::Foundation::Point& b)
{ return a.X == b.X && a.Y == b.Y; }

constexpr inline bool operator ==(const struct ABI::Windows::Foundation::Rect& a, const struct ABI::Windows::Foundation::Rect& b)
{ return a.X == b.X && a.Y == b.Y && a.Width == b.Width && a.Height == b.Height; }

constexpr inline bool operator ==(const struct ABI::Windows::Foundation::Size& a, const struct ABI::Windows::Foundation::Size& b)
{ return a.Width == b.Width && a.Height == b.Height; }

//IsNotEqualFoundationStruct
constexpr inline bool operator !=(const struct ABI::Windows::Foundation::DateTime& a, const struct ABI::Windows::Foundation::DateTime& b)
{ return a.UniversalTime != b.UniversalTime; }

constexpr inline bool operator !=(const struct ABI::Windows::Foundation::TimeSpan& a, const struct ABI::Windows::Foundation::TimeSpan& b)
{ return a.Duration != b.Duration; }

constexpr inline bool operator !=(const struct ABI::Windows::Foundation::Point& a, const struct ABI::Windows::Foundation::Point& b)
{ return a.X != b.X || a.Y != b.Y; }

constexpr inline bool operator !=(const struct ABI::Windows::Foundation::Rect& a, const struct ABI::Windows::Foundation::Rect& b)
{ return a.X != b.X || a.Y != b.Y || a.Width != b.Width || a.Height != b.Height; }

constexpr inline bool operator !=(const struct ABI::Windows::Foundation::Size& a, const struct ABI::Windows::Foundation::Size& b)
{ return a.Width != b.Width || a.Height != b.Height; }
