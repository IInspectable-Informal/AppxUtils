#pragma once

namespace ABI::AppxUtils::Internal
{
	template<typename T>
    class Reference final : public InspectableBase<BaseTrust,
        ABI::Windows::Foundation::IReference<T>,
        ABI::Windows::Foundation::IPropertyValue,
        IAgileObject
    >
    {
    private:
        using T_ABI = typename ABI::Windows::Foundation::Internal::GetAbiType<typename ABI::Windows::Foundation::IReference<T>::T_complex>::type;

        Reference(const T_ABI& value) noexcept : m_Value(value)
        {
            
        }

    public:
        static HRESULT STDMETHODCALLTYPE CreateInstance(const T_ABI& value, ABI::Windows::Foundation::IReference<T>*& result) noexcept
        {
            if constexpr (IsTypeWinRTString<T_ABI>)
            {
                HRESULT hr{ S_OK };
                HSTRING newString{ nullptr };
                hr = WindowsDuplicateString(value, &newString);
                if (SUCCEEDED(hr))
                {
                    result = new Reference<T>(newString);
                    return result ? S_OK : E_OUTOFMEMORY;
                }
                else
                { return hr; }
            }
            else if constexpr (IsFundamentalType<T_ABI> || __is_enum(T_ABI))
            {
                result = new Reference<T>(value);
                return result ? S_OK : E_OUTOFMEMORY;
            }
            else if constexpr (ABI::Windows::Foundation::Collections::is_foundation_struct<T_ABI>::value)
            {
                result = new Reference<T>(value);
                return result ? S_OK : E_OUTOFMEMORY;
            }
            else if constexpr (__is_trivial(T_ABI) && __is_standard_layout(T_ABI))
            {
                HRESULT hr{ S_OK };
                T_ABI structCopy{};
                hr = StructLifetimeFunctions<T_ABI>::DeepCopyStruct(value, structCopy);
                if (SUCCEEDED(hr))
                {
                    result = new Reference<T>(structCopy);
                    return result ? S_OK : E_OUTOFMEMORY;
                }
                else
                { return hr; }
            }
        }

        HRESULT STDMETHODCALLTYPE get_Value(T_ABI* value)
        {
            if constexpr (IsTypeWinRTString<T_ABI>)
            { return WindowsDuplicateString(m_Value, value); }
            else if constexpr (IsFundamentalType<T_ABI> || __is_enum(T_ABI))
            {
                *value = m_Value;
                return S_OK;
            }
            else if constexpr (ABI::Windows::Foundation::Collections::is_foundation_struct<T_ABI>::value)
            {
                *value = m_Value;
                return S_OK;
            }
            else if constexpr (__is_trivial(T_ABI) && __is_standard_layout(T_ABI))
            { return StructLifetimeFunctions<T_ABI>::DeepCopyStruct(m_Value, *value); }
        }

        //IPropertyValue
        HRESULT STDMETHODCALLTYPE get_Type(ABI::Windows::Foundation::PropertyType* value)
        {
            if constexpr (IsSameType<T, BYTE>)
            {
                *value = ABI::Windows::Foundation::PropertyType_UInt8;
                return S_OK;
            }
            else if constexpr (IsSameType<T, INT16>)
            {
                *value = ABI::Windows::Foundation::PropertyType_Int16;
                return S_OK;
            }
            else if constexpr (IsSameType<T, UINT16>)
            {
                *value = ABI::Windows::Foundation::PropertyType_UInt16;
                return S_OK;
            }
            else if constexpr (IsSameType<T, INT32>)
            {
                *value = ABI::Windows::Foundation::PropertyType_Int32;
                return S_OK;
            }
            else if constexpr (IsSameType<T, UINT32>)
            {
                *value = ABI::Windows::Foundation::PropertyType_UInt32;
                return S_OK;
            }
            else if constexpr (IsSameType<T, INT64>)
            {
                *value = ABI::Windows::Foundation::PropertyType_Int64;
                return S_OK;
            }
            else if constexpr (IsSameType<T, UINT64>)
            {
                *value = ABI::Windows::Foundation::PropertyType_UInt64;
                return S_OK;
            }
            else if constexpr (IsSameType<T, FLOAT>)
            {
                *value = ABI::Windows::Foundation::PropertyType_Single;
                return S_OK;
            }
            else if constexpr (IsSameType<T, DOUBLE>)
            {
                *value = ABI::Windows::Foundation::PropertyType_Double;
                return S_OK;
            }
            else if constexpr (IsSameType<T, WCHAR>)
            {
                *value = ABI::Windows::Foundation::PropertyType_Char16;
                return S_OK;
            }
            else if constexpr (IsSameType<T, bool>)
            {
                *value = ABI::Windows::Foundation::PropertyType_Boolean;
                return S_OK;
            }
            else if constexpr (IsSameType<T, HSTRING>)
            {
                *value = ABI::Windows::Foundation::PropertyType_String;
                return S_OK;
            }
            else if constexpr (IsSameType<T, ABI::Windows::Foundation::DateTime>)
            {
                *value = ABI::Windows::Foundation::PropertyType_DateTime;
                return S_OK;
            }
            else if constexpr (IsSameType<T, ABI::Windows::Foundation::TimeSpan>)
            {
                *value = ABI::Windows::Foundation::PropertyType_TimeSpan;
                return S_OK;
            }
            else if constexpr (IsSameType<T, GUID>)
            {
                *value = ABI::Windows::Foundation::PropertyType_Guid;
                return S_OK;
            }
            else if constexpr (IsSameType<T, ABI::Windows::Foundation::Point>)
            {
                *value = ABI::Windows::Foundation::PropertyType_Point;
                return S_OK;
            }
            else if constexpr (IsSameType<T, ABI::Windows::Foundation::Size>)
            {
                *value = ABI::Windows::Foundation::PropertyType_Size;
                return S_OK;
            }
            else if constexpr (IsSameType<T, ABI::Windows::Foundation::Rect>)
            {
                *value = ABI::Windows::Foundation::PropertyType_Rect;
                return S_OK;
            }
            else
            {
                *value = ABI::Windows::Foundation::PropertyType_OtherType;
                return S_OK;
            }
        }

        HRESULT STDMETHODCALLTYPE get_IsNumericScalar(boolean* value)
        {
            if constexpr (IsNumericType<T_ABI>)
            { *value = true; }
            else
            { *value = false; }
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE GetUInt8(BYTE* value)
        {
            if constexpr (IsSameType<T, BYTE>)
            {
                *value = m_Value;
                return S_OK;
            }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetInt16(INT16* value)
        {
            if constexpr (IsSameType<T, INT16>)
            {
                *value = m_Value;
                return S_OK;
            }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetUInt16(UINT16* value)
        {
            if constexpr (IsSameType<T, UINT16>)
            {
                *value = m_Value;
                return S_OK;
            }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetInt32(INT32* value)
        {
            if constexpr (IsSameType<T, INT32>)
            {
                *value = m_Value;
                return S_OK;
            }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetUInt32(UINT32* value)
        {
            if constexpr (IsSameType<T, UINT32>)
            {
                *value = m_Value;
                return S_OK;
            }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetInt64(INT64* value)
        {
            if constexpr (IsSameType<T, INT64>)
            {
                *value = m_Value;
                return S_OK;
            }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetUInt64(UINT64* value)
        {
            if constexpr (IsSameType<T, UINT64>)
            {
                *value = m_Value;
                return S_OK;
            }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetSingle(FLOAT* value)
        {
            if constexpr (IsSameType<T, FLOAT>)
            {
                *value = m_Value;
                return S_OK;
            }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetDouble(DOUBLE* value)
        {
            if constexpr (IsSameType<T, DOUBLE>)
            {
                *value = m_Value;
                return S_OK;
            }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetChar16(WCHAR* value)
        {
            if constexpr (IsSameType<T, WCHAR>)
            {
                *value = m_Value;
                return S_OK;
            }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetBoolean(boolean* value)
        {
            if constexpr (IsSameType<T, bool>)
            {
                *value = m_Value;
                return S_OK;
            }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetString(HSTRING* value)
        {
            if constexpr (IsSameType<T, HSTRING>)
            { return WindowsDuplicateString(m_Value, value); }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetGuid(GUID* value)
        {
            if constexpr (IsSameType<T, GUID>)
            {
                *value = m_Value;
                return S_OK;
            }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetDateTime(ABI::Windows::Foundation::DateTime* value)
        {
            if constexpr (IsSameType<T, ABI::Windows::Foundation::DateTime>)
            {
                *value = m_Value;
                return S_OK;
            }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetTimeSpan(ABI::Windows::Foundation::TimeSpan* value)
        {
            if constexpr (IsSameType<T, ABI::Windows::Foundation::TimeSpan>)
            {
                *value = m_Value;
                return S_OK;
            }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetPoint(ABI::Windows::Foundation::Point* value)
        {
            if constexpr (IsSameType<T, ABI::Windows::Foundation::Point>)
            {
                *value = m_Value;
                return S_OK;
            }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetSize(ABI::Windows::Foundation::Size* value)
        {
            if constexpr (IsSameType<T, ABI::Windows::Foundation::Size>)
            {
                *value = m_Value;
                return S_OK;
            }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetRect(ABI::Windows::Foundation::Rect* value)
        {
            if constexpr (IsSameType<T, ABI::Windows::Foundation::Rect>)
            {
                *value = m_Value;
                return S_OK;
            }
            else
            { return TYPE_E_TYPEMISMATCH; }
        }

        HRESULT STDMETHODCALLTYPE GetUInt8Array(UINT32* valueLength, BYTE** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetInt16Array(UINT32* valueLength, INT16** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetUInt16Array(UINT32* valueLength, UINT16** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetInt32Array(UINT32* valueLength, INT32** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetUInt32Array(UINT32* valueLength, UINT32** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetInt64Array(UINT32* valueLength, INT64** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetUInt64Array(UINT32* valueLength, UINT64** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetSingleArray(UINT32* valueLength, FLOAT** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetDoubleArray(UINT32* valueLength, DOUBLE** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetChar16Array(UINT32* valueLength, WCHAR** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetBooleanArray(UINT32* valueLength, boolean** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetStringArray(UINT32* valueLength, HSTRING** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetInspectableArray(UINT32* valueLength, IInspectable*** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetGuidArray(UINT32* valueLength, GUID** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetDateTimeArray(UINT32* valueLength, ABI::Windows::Foundation::DateTime** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetTimeSpanArray(UINT32* valueLength, ABI::Windows::Foundation::TimeSpan** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetPointArray(UINT32* valueLength, ABI::Windows::Foundation::Point** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetSizeArray(UINT32* valueLength, ABI::Windows::Foundation::Size** value)
        { return TYPE_E_TYPEMISMATCH; }

        HRESULT STDMETHODCALLTYPE GetRectArray(UINT32* valueLength, ABI::Windows::Foundation::Rect** value)
        { return TYPE_E_TYPEMISMATCH; }

        //IInspectable
        HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className)
        {
            if (!m_ClassName)
            {
                m_ClassName = ABI::Windows::Foundation::IReference<T>::z_get_rc_name_impl();
                while (*(m_ClassName + m_ClassNameSize))
                { ++m_ClassNameSize; }
            }
            return WindowsCreateString(m_ClassName, m_ClassNameSize, className);
        }

        ~Reference() noexcept
        {
            if constexpr (IsSameType<T, HSTRING>)
            { WindowsDeleteString(m_Value); }
            else if constexpr (!IsFundamentalType<T_ABI> && !__is_enum(T_ABI) && !ABI::Windows::Foundation::Collections::is_foundation_struct<T_ABI>::value && __is_trivial(T_ABI) && __is_standard_layout(T_ABI))
            { StructLifetimeFunctions<T_ABI>::ReleaseStruct(m_Value); }
        }

        private:
            T_ABI m_Value{};

            const wchar_t* m_ClassName{ nullptr };
            UINT32 m_ClassNameSize{ 0 };
    };
}
