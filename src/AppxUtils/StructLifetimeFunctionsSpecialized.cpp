#include "pch.h"
#include "StructLifetimeFunctionsSpecialized.h"

namespace ABI
{
	using namespace Windows::Foundation;
	using namespace Windows::Foundation::Collections;
	using namespace Windows::ApplicationModel;
	using namespace AppxUtils;
}

namespace ABI::AppxUtils::Internal
{
	constexpr inline bool operator ==(const ABI::PackageVersion a, const ABI::PackageVersion b)
	{ return a.Major == b.Major && a.Minor == b.Minor && a.Build == b.Build && a.Revision == b.Revision; }

	inline bool IsEqualWinRTString(const HSTRING& a, const HSTRING& b)
	{
		if (a && b)
		{
			if (a != b)
			{
				INT32 cmp{ 0 };
				WindowsCompareStringOrdinal(a, b, &cmp);
				return cmp == 0;
			}
			else
			{ return true; }
		}
		else if (!a && !b)
		{ return true; }
		else
		{ return false; }
	}

	template<typename T>
	inline bool IsEqualReference(ABI::IReference<T>* a, ABI::IReference<T>* b)
	{
		if (a && b)
		{
			if (a != b)
			{
				T aValue{};
				T bValue{};
				a->get_Value(&aValue);
				b->get_Value(&bValue);
				return aValue == bValue;
			}
			else
			{ return true; }
		}
		else if (!a && !b)
		{ return true; }
		else
		{ return false; }
	}

	//AppxPackageDependency
	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageDependency>::DeepCopyStruct(const struct ABI::AppxPackageDependency& source, struct ABI::AppxPackageDependency& target)
	{
		HRESULT hr{ WindowsDuplicateString(source.Name, &target.Name) };
		if (SUCCEEDED(hr))
		{
			hr = WindowsDuplicateString(source.Publisher, &target.Publisher);
			if (SUCCEEDED(hr))
			{
				target.MinVersion = source.MinVersion;
				target.MaxMajorVersionTested = source.MaxMajorVersionTested;
				target.Optional = source.Optional;
				if (target.MaxMajorVersionTested)
				{ target.MaxMajorVersionTested->AddRef(); }
				if (target.Optional)
				{ target.Optional->AddRef(); }
			}
			else
			{ WindowsDeleteString(target.Name); }
		}
		return hr;
	}

	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageDependency>::ReleaseStruct(const struct ABI::AppxPackageDependency& source)
	{
		if (source.Name)
		{ WindowsDeleteString(source.Name); }
		if (source.Publisher)
		{ WindowsDeleteString(source.Publisher); }
		if (source.MaxMajorVersionTested)
		{ source.MaxMajorVersionTested->Release(); }
		if (source.Optional)
		{ source.Optional->Release(); }
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageDependency>::IsEqualStruct(const struct ABI::AppxPackageDependency& a, const struct ABI::AppxPackageDependency& b, bool& result)
	{
		bool results[5]{ false, false, false, false, false };
		results[0] = IsEqualWinRTString(a.Name, b.Name);
		results[1] = IsEqualWinRTString(a.Publisher, b.Publisher);
		results[2] = a.MinVersion == b.MinVersion;
		if (a.MaxMajorVersionTested != b.MaxMajorVersionTested)
		{
			if (a.MaxMajorVersionTested && b.MaxMajorVersionTested)
			{
				UINT16 versiona, versionb;
				a.MaxMajorVersionTested->get_Value(&versiona);
				b.MaxMajorVersionTested->get_Value(&versionb);
				results[3] = versiona == versionb;
			}
			else
			{ results[3] = false; }
		}
		else
		{ results[3] = true; }
		if (a.Optional != b.Optional)
		{
			if (a.Optional && b.Optional)
			{
				boolean optionala, optionalb;
				a.Optional->get_Value(&optionala);
				b.Optional->get_Value(&optionalb);
				results[4] = optionala == optionalb;
			}
			else
			{ results[4] = false; }
		}
		else
		{ results[4] = true; }
		result = results[0] && results[1] && results[2] && results[3] && results[4];
		return S_OK;
	}

	//AppxPackageResource
	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageResource>::DeepCopyStruct(const struct ABI::AppxPackageResource& source, struct ABI::AppxPackageResource& target)
	{
		if (source.Language)
		{
			HSTRING strCopy{ nullptr };
			HRESULT hr{ WindowsDuplicateString(source.Language, &strCopy) };
			if (SUCCEEDED(hr))
			{
				target = source;
				target.Language = strCopy;
				return S_OK;
			}
			else
			{ return hr; }
		}
		else
		{
			target = source;
			return S_OK;
		}
	}

	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageResource>::ReleaseStruct(const struct ABI::AppxPackageResource& source)
	{
		if (source.Language)
		{ WindowsDeleteString(source.Language); }
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageResource>::IsEqualStruct(const struct ABI::AppxPackageResource& a, const struct ABI::AppxPackageResource& b, bool& result)
	{
		bool results[4]{ false, false, false, false };
		results[0] = a.Type == b.Type;
		results[1] = IsEqualWinRTString(a.Language, b.Language);
		results[2] = a.Scale == b.Scale;
		results[3] = a.DirectXFeatureLevel == b.DirectXFeatureLevel;
		result = results[0] && results[1] && results[2] && results[3];
		return S_OK;
	}

	//AppxPackageTargetDeviceFamily
	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageTargetDeviceFamily>::DeepCopyStruct(const struct ABI::AppxPackageTargetDeviceFamily& source, struct ABI::AppxPackageTargetDeviceFamily& target)
	{
		if (source.Name)
		{
			HSTRING strCopy{ nullptr };
			HRESULT hr{ WindowsDuplicateString(source.Name, &strCopy) };
			if (SUCCEEDED(hr))
			{
				target = source;
				target.Name = strCopy;
				return S_OK;
			}
			else
			{ return hr; }
		}
		else
		{
			target = source;
			return S_OK;
		}
	}

	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageTargetDeviceFamily>::ReleaseStruct(const struct ABI::AppxPackageTargetDeviceFamily& source)
	{
		if (source.Name)
		{ WindowsDeleteString(source.Name); }
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageTargetDeviceFamily>::IsEqualStruct(const struct ABI::AppxPackageTargetDeviceFamily& a, const struct ABI::AppxPackageTargetDeviceFamily& b, bool& result)
	{
		bool results[3]{ false, false, false };
		results[0] = IsEqualWinRTString(a.Name, b.Name);
		results[1] = a.MinVersion == b.MinVersion;
		results[2] = a.MaxVersionTested == b.MaxVersionTested;
		result = results[0] && results[1] && results[2];
		return S_OK;
	}

	//AppxPackageMainPackageDependency
	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageMainPackageDependency>::DeepCopyStruct(const struct ABI::AppxPackageMainPackageDependency& source, struct ABI::AppxPackageMainPackageDependency& target)
	{
		HRESULT hr{ WindowsDuplicateString(source.Name, &target.Name) };
		if (SUCCEEDED(hr))
		{
			hr = WindowsDuplicateString(source.FamilyName, &target.FamilyName);
			if (SUCCEEDED(hr))
			{
				hr = WindowsDuplicateString(source.Publisher, &target.Publisher);
				if (FAILED(hr))
				{
					WindowsDeleteString(target.FamilyName);
					WindowsDeleteString(target.Name);
				}
			}
			else
			{ WindowsDeleteString(target.Name); }
		}
		return hr;
	}

	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageMainPackageDependency>::ReleaseStruct(const struct ABI::AppxPackageMainPackageDependency& source)
	{
	    if (source.Name)
		{ WindowsDeleteString(source.Name); }
		if (source.FamilyName)
		{ WindowsDeleteString(source.FamilyName); }
		if (source.Publisher)
		{ WindowsDeleteString(source.Publisher); }
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageMainPackageDependency>::IsEqualStruct(const struct ABI::AppxPackageMainPackageDependency& a, const struct ABI::AppxPackageMainPackageDependency& b, bool& result)
	{
		bool results[3]{ false, false, false };
		results[0] = IsEqualWinRTString(a.Name, b.Name);
		results[1] = IsEqualWinRTString(a.FamilyName, b.FamilyName);
		results[2] = IsEqualWinRTString(a.Publisher, b.Publisher);
		result = results[0] && results[1] && results[2];
		return S_OK;
	}
	
	//AppxPackageOSPackageDependency
	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageOSPackageDependency>::DeepCopyStruct(const struct ABI::AppxPackageOSPackageDependency& source, struct ABI::AppxPackageOSPackageDependency& target)
	{
		HRESULT hr{ WindowsDuplicateString(source.Name, &target.Name) };
		if (SUCCEEDED(hr))
		{ target.Version = source.Version; }
		return hr;
	}

	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageOSPackageDependency>::ReleaseStruct(const struct ABI::AppxPackageOSPackageDependency& source)
	{
	    if (source.Name)
		{ WindowsDeleteString(source.Name); }
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageOSPackageDependency>::IsEqualStruct(const struct ABI::AppxPackageOSPackageDependency& a, const struct ABI::AppxPackageOSPackageDependency& b, bool& result)
	{
		bool results[2]{ false, false };
		results[0] = IsEqualWinRTString(a.Name, b.Name);
		results[1] = a.Version == b.Version;
		result = results[0] && results[1];
		return S_OK;
	}
	
	//AppxPackageHostRuntimeDependency
	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageHostRuntimeDependency>::DeepCopyStruct(const struct ABI::AppxPackageHostRuntimeDependency& source, struct ABI::AppxPackageHostRuntimeDependency& target)
	{
		HRESULT hr{ WindowsDuplicateString(source.Name, &target.Name) };
		if (SUCCEEDED(hr))
		{
		    hr = WindowsDuplicateString(source.Publisher, &target.Publisher);
		    if (SUCCEEDED(hr))
		    { target.MinVersion = source.MinVersion; }
		    else
		    { WindowsDeleteString(target.Name); }
		}
		return hr;
	}

	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageHostRuntimeDependency>::ReleaseStruct(const struct ABI::AppxPackageHostRuntimeDependency& source)
	{
	    if (source.Name)
		{ WindowsDeleteString(source.Name); }
		if (source.Publisher)
		{ WindowsDeleteString(source.Publisher); }
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageHostRuntimeDependency>::IsEqualStruct(const struct ABI::AppxPackageHostRuntimeDependency& a, const struct ABI::AppxPackageHostRuntimeDependency& b, bool& result)
	{
		bool results[3]{ false, false };
		results[0] = IsEqualWinRTString(a.Name, b.Name);
		results[1] = IsEqualWinRTString(a.Publisher, b.Publisher);
		results[2] = a.MinVersion == b.MinVersion;
		result = results[0] && results[1] && results[2];
		return S_OK;
	}

	//AppxPackageDriverConstraint
	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageDriverConstraint>::DeepCopyStruct(const struct ABI::AppxPackageDriverConstraint& source, struct ABI::AppxPackageDriverConstraint& target)
	{
		HRESULT hr{ WindowsDuplicateString(source.Name, &target.Name) };
		if (SUCCEEDED(hr))
		{
			hr = WindowsDuplicateString(source.MinDate, &target.MinDate);
			if (SUCCEEDED(hr))
			{
				target.MinVersion = source.MinVersion;
				if (target.MinVersion)
				{ target.MinVersion->AddRef(); }
			}
			else
			{ WindowsDeleteString(target.Name); }
		}
		return hr;
	}

	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageDriverConstraint>::ReleaseStruct(const struct ABI::AppxPackageDriverConstraint& source)
	{
		if (source.Name)
		{ WindowsDeleteString(source.Name); }
		if (source.MinVersion)
		{ source.MinVersion->Release(); }
		if (source.MinDate)
		{ WindowsDeleteString(source.MinDate); }
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::AppxPackageDriverConstraint>::IsEqualStruct(const struct ABI::AppxPackageDriverConstraint& a, const struct ABI::AppxPackageDriverConstraint& b, bool& result)
	{
		bool results[3]{ false, false, false };
		results[0] = IsEqualWinRTString(a.Name, b.Name);
		results[1] = IsEqualReference(a.MinVersion, b.MinVersion);
		results[2] = IsEqualWinRTString(a.MinDate, b.MinDate);
		result = results[0] && results[1] && results[2];
		return S_OK;
	}

	//PackageVersion
	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::PackageVersion>::DeepCopyStruct(const struct ABI::PackageVersion& source, struct ABI::PackageVersion& target)
	{
		target = source;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::PackageVersion>::ReleaseStruct(const struct ABI::PackageVersion& source)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE StructLifetimeFunctions<struct ABI::PackageVersion>::IsEqualStruct(const struct ABI::PackageVersion& a, const struct ABI::PackageVersion& b, bool& result)
	{
		result = a == b;
		return S_OK;
	}
}
