#pragma once

namespace ABI::AppxUtils::Internal
{
	template<>
	class StructLifetimeFunctions<struct ABI::AppxUtils::AppxPackageDependency> final
	{
	public:
		static HRESULT STDMETHODCALLTYPE DeepCopyStruct(const struct ABI::AppxUtils::AppxPackageDependency& source, struct ABI::AppxUtils::AppxPackageDependency& target);
		static HRESULT STDMETHODCALLTYPE ReleaseStruct(const struct ABI::AppxUtils::AppxPackageDependency& source);
		static HRESULT STDMETHODCALLTYPE IsEqualStruct(const struct ABI::AppxUtils::AppxPackageDependency& a, const struct ABI::AppxUtils::AppxPackageDependency& b, bool& result);

		StructLifetimeFunctions() = delete;
		StructLifetimeFunctions(const StructLifetimeFunctions&) = delete;
		StructLifetimeFunctions(StructLifetimeFunctions&&) = delete;
		~StructLifetimeFunctions() = delete;
	};

	template<>
	class StructLifetimeFunctions<struct ABI::AppxUtils::AppxPackageResource> final
	{
	public:
		static HRESULT STDMETHODCALLTYPE DeepCopyStruct(const struct ABI::AppxUtils::AppxPackageResource& source, struct ABI::AppxUtils::AppxPackageResource& target);
		static HRESULT STDMETHODCALLTYPE ReleaseStruct(const struct ABI::AppxUtils::AppxPackageResource& source);
		static HRESULT STDMETHODCALLTYPE IsEqualStruct(const struct ABI::AppxUtils::AppxPackageResource& a, const struct ABI::AppxUtils::AppxPackageResource& b, bool& result);

		StructLifetimeFunctions() = delete;
		StructLifetimeFunctions(const StructLifetimeFunctions&) = delete;
		StructLifetimeFunctions(StructLifetimeFunctions&&) = delete;
		~StructLifetimeFunctions() = delete;
	};

	template<>
	class StructLifetimeFunctions<struct ABI::AppxUtils::AppxPackageTargetDeviceFamily> final
	{
	public:
		static HRESULT STDMETHODCALLTYPE DeepCopyStruct(const struct ABI::AppxUtils::AppxPackageTargetDeviceFamily& source, struct ABI::AppxUtils::AppxPackageTargetDeviceFamily& target);
		static HRESULT STDMETHODCALLTYPE ReleaseStruct(const struct ABI::AppxUtils::AppxPackageTargetDeviceFamily& source);
		static HRESULT STDMETHODCALLTYPE IsEqualStruct(const struct ABI::AppxUtils::AppxPackageTargetDeviceFamily& a, const struct ABI::AppxUtils::AppxPackageTargetDeviceFamily& b, bool& result);

		StructLifetimeFunctions() = delete;
		StructLifetimeFunctions(const StructLifetimeFunctions&) = delete;
		StructLifetimeFunctions(StructLifetimeFunctions&&) = delete;
		~StructLifetimeFunctions() = delete;
	};

	template<>
	class StructLifetimeFunctions<struct ABI::AppxUtils::AppxPackageMainPackageDependency> final
	{
	public:
		static HRESULT STDMETHODCALLTYPE DeepCopyStruct(const struct ABI::AppxUtils::AppxPackageMainPackageDependency& source, struct ABI::AppxUtils::AppxPackageMainPackageDependency& target);
		static HRESULT STDMETHODCALLTYPE ReleaseStruct(const struct ABI::AppxUtils::AppxPackageMainPackageDependency& source);
		static HRESULT STDMETHODCALLTYPE IsEqualStruct(const struct ABI::AppxUtils::AppxPackageMainPackageDependency& a, const struct ABI::AppxUtils::AppxPackageMainPackageDependency& b, bool& result);

		StructLifetimeFunctions() = delete;
		StructLifetimeFunctions(const StructLifetimeFunctions&) = delete;
		StructLifetimeFunctions(StructLifetimeFunctions&&) = delete;
		~StructLifetimeFunctions() = delete;
	};
	
	template<>
	class StructLifetimeFunctions<struct ABI::AppxUtils::AppxPackageOSPackageDependency> final
	{
	public:
		static HRESULT STDMETHODCALLTYPE DeepCopyStruct(const struct ABI::AppxUtils::AppxPackageOSPackageDependency& source, struct ABI::AppxUtils::AppxPackageOSPackageDependency& target);
		static HRESULT STDMETHODCALLTYPE ReleaseStruct(const struct ABI::AppxUtils::AppxPackageOSPackageDependency& source);
		static HRESULT STDMETHODCALLTYPE IsEqualStruct(const struct ABI::AppxUtils::AppxPackageOSPackageDependency& a, const struct ABI::AppxUtils::AppxPackageOSPackageDependency& b, bool& result);

		StructLifetimeFunctions() = delete;
		StructLifetimeFunctions(const StructLifetimeFunctions&) = delete;
		StructLifetimeFunctions(StructLifetimeFunctions&&) = delete;
		~StructLifetimeFunctions() = delete;
	};
	
	template<>
	class StructLifetimeFunctions<struct ABI::AppxUtils::AppxPackageHostRuntimeDependency> final
	{
	public:
		static HRESULT STDMETHODCALLTYPE DeepCopyStruct(const struct ABI::AppxUtils::AppxPackageHostRuntimeDependency& source, struct ABI::AppxUtils::AppxPackageHostRuntimeDependency& target);
		static HRESULT STDMETHODCALLTYPE ReleaseStruct(const struct ABI::AppxUtils::AppxPackageHostRuntimeDependency& source);
		static HRESULT STDMETHODCALLTYPE IsEqualStruct(const struct ABI::AppxUtils::AppxPackageHostRuntimeDependency& a, const struct ABI::AppxUtils::AppxPackageHostRuntimeDependency& b, bool& result);

		StructLifetimeFunctions() = delete;
		StructLifetimeFunctions(const StructLifetimeFunctions&) = delete;
		StructLifetimeFunctions(StructLifetimeFunctions&&) = delete;
		~StructLifetimeFunctions() = delete;
	};
}
