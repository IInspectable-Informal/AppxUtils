#pragma once

namespace ABI::AppxUtils
{
	class AppxPackageDriverDependency final : public InspectableElementBase<BaseTrust,
		IAppxPackageDriverDependency,
		ABI::Windows::Foundation::Collections::IVectorView<struct AppxPackageDriverConstraint>,
		ABI::Windows::Foundation::Collections::IIterable<struct AppxPackageDriverConstraint>,
		IAppxPackageDriverDependencyInterop,
		IAgileObject
	>
	{
	public:
		AppxPackageDriverDependency(IAppxManifestDriverDependency*& driverDependency) noexcept;

		//Windows.Foundation.Collections.IVectorView<AppxPackageDriverConstraint>
		HRESULT STDMETHODCALLTYPE GetAt(UINT32 index, struct AppxPackageDriverConstraint* item);
		HRESULT STDMETHODCALLTYPE get_Size(UINT32* size);
		HRESULT STDMETHODCALLTYPE IndexOf(struct AppxPackageDriverConstraint value, UINT32* index, boolean* found);
		HRESULT STDMETHODCALLTYPE GetMany(UINT32 startIndex, UINT32 capacity, struct AppxPackageDriverConstraint* value, UINT32* actual);

		//Windows.Foundation.Collections.IIterable<AppxPackageDriverConstraint>
		HRESULT STDMETHODCALLTYPE First(ABI::Windows::Foundation::Collections::IIterator<struct AppxPackageDriverConstraint>** first);

		//IAppxPackageDriverDependencyInterop
		HRESULT STDMETHODCALLTYPE get_DriverDependency(IAppxManifestDriverDependency** value);
		HRESULT STDMETHODCALLTYPE get_DriverConstraints(IAppxManifestDriverConstraintsEnumerator** value);

		//IInspectable
		HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className);

		~AppxPackageDriverDependency() noexcept;

	private:
		IAppxManifestDriverDependency* m_DriverDependency{ nullptr };

		INIT_ONCE m_InitOnce{ INIT_ONCE_STATIC_INIT };
		struct AppxPackageDriverConstraint* m_Array{ nullptr };
		UINT32 m_Count{ 0 };

		static BOOL WINAPI InitListStatic(INIT_ONCE* InitOnce, void* Parameter, void** Context);
	};
}
