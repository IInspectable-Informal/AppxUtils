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
		if (a.Name != b.Name)
		{
			INT32 cmp{ 0 };
			WindowsCompareStringOrdinal(a.Name, b.Name, &cmp);
			results[0] = cmp == 0;
		}
		else
		{ results[0] = true; }
		if (a.Publisher != b.Publisher)
		{
			INT32 cmp{ 0 };
			WindowsCompareStringOrdinal(a.Publisher, b.Publisher, &cmp);
			results[1] = cmp == 0;
		}
		else
		{ results[1] = true; }
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
		if (a.Language && b.Language)
		{
			if (a.Language != b.Language)
			{
				INT32 cmp{ 0 };
				WindowsCompareStringOrdinal(a.Language, b.Language, &cmp);
				results[1] = cmp == 0;
			}
			else
			{ results[1] = true; }
		}
		else if (!a.Language && !b.Language)
		{ results[1] = true; }
		else
		{ results[1] = false; }
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
		if (a.Name && b.Name)
		{
			if (a.Name != b.Name)
			{
				INT32 cmp{ 0 };
				WindowsCompareStringOrdinal(a.Name, b.Name, &cmp);
				results[0] = cmp == 0;
			}
			else
			{ results[0] = true; }
		}
		else if (!a.Name && !b.Name)
		{ results[0] = true; }
		else
		{ results[0] = false; }
		results[1] = a.MinVersion == b.MinVersion;
		results[2] = a.MaxVersionTested == b.MaxVersionTested;
		result = results[0] && results[1] && results[2];
		return S_OK;
	}
}
