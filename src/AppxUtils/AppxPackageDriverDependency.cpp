#include "pch.h"
#include "AppxPackageDriverDependency.h"
#include "helpers.hpp"

namespace ABI
{
	using namespace Windows::Foundation;
	using namespace Windows::Foundation::Collections;
	using namespace Windows::ApplicationModel;
	using namespace AppxUtils::Internal;
}

namespace ABI::AppxUtils::Internal
{
	class AppxPackageDriverDependencyIterator final : public InspectableBase<BaseTrust,
		ABI::IIterator<struct AppxPackageDriverConstraint>,
		IAgileObject>
	{
	public:
		AppxPackageDriverDependencyIterator(AppxPackageDriverDependency* container, struct AppxPackageDriverConstraint* list, UINT32 count) noexcept : m_Container(container), m_Array(list), m_Capacity(count)
		{
			container->AddRef();
		}

		HRESULT STDMETHODCALLTYPE get_Current(struct AppxPackageDriverConstraint* current)
		{
			if (m_Current < m_Capacity)
			{ return StructLifetimeFunctions<struct AppxPackageDriverConstraint>::DeepCopyStruct(m_Array[m_Current], *current); }
			else
			{ return E_BOUNDS; }
		}

		HRESULT STDMETHODCALLTYPE get_HasCurrent(boolean* hasCurrent)
		{
			*hasCurrent = m_Current < m_Capacity;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE MoveNext(boolean* hasCurrent)
		{
			if (m_Current <= m_Capacity)
			{
				++m_Current;
				*hasCurrent = m_Current < m_Capacity;
				return S_OK;
			}
			else
			{ return E_BOUNDS; }
		}

		HRESULT STDMETHODCALLTYPE GetMany(UINT32 capacity, struct AppxPackageDriverConstraint* value, UINT32* actual)
		{
			return E_NOTIMPL;
		}

		//IInspectable
		HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className)
		{ return WindowsCreateString(L"Windows.Foundation.Collections.IIterator`1<AppxUtils.AppxPackageDriverConstraint>", 81, className); }

		~AppxPackageDriverDependencyIterator() noexcept
		{
			m_Container->Release();
		}

	private:
		struct AppxPackageDriverConstraint* m_Array{ nullptr };
		UINT32 m_Capacity{ 0 };
		UINT32 m_Current{ 0 };
		AppxPackageDriverDependency* m_Container{ nullptr };
	};
}

namespace ABI::AppxUtils
{
	AppxPackageDriverDependency::AppxPackageDriverDependency(IAppxManifestDriverDependency*& driverDependency) noexcept : m_DriverDependency(driverDependency)
	{

	}

	//Windows.Foundation.Collections.IVectorView<AppxPackageDriverConstraint>
	HRESULT STDMETHODCALLTYPE AppxPackageDriverDependency::GetAt(UINT32 index, struct AppxPackageDriverConstraint* item)
	{
		if (InitOnceExecuteOnce(&m_InitOnce, InitListStatic, this, nullptr))
		{
			if (index < m_Count)
			{ return StructLifetimeFunctions<struct AppxPackageDriverConstraint>::DeepCopyStruct(m_Array[index], *item); }
			else
			{ return E_BOUNDS; }
		}
		else
		{ return HRESULT_FROM_WIN32(GetLastError()); }
	}

	HRESULT STDMETHODCALLTYPE AppxPackageDriverDependency::get_Size(UINT32* size)
	{
		if (InitOnceExecuteOnce(&m_InitOnce, InitListStatic, this, nullptr))
		{
			*size = m_Count;
			return S_OK;
		}
		else
		{ return HRESULT_FROM_WIN32(GetLastError()); }
	}

	HRESULT STDMETHODCALLTYPE AppxPackageDriverDependency::IndexOf(struct AppxPackageDriverConstraint value, UINT32* index, boolean* found)
	{
		if (InitOnceExecuteOnce(&m_InitOnce, InitListStatic, this, nullptr))
		{
			for (UINT32 i = 0; i < m_Count; ++i)
			{
				struct AppxPackageDriverConstraint& element{ m_Array[i] };
				bool result{};
				StructLifetimeFunctions<struct AppxPackageDriverConstraint>::IsEqualStruct(element, value, result);
				if (result)
				{
					*index = i;
					*found = true;
					return S_OK;
				}
			}
			*found = false;
			return S_OK;
		}
		else
		{ return HRESULT_FROM_WIN32(GetLastError()); }
	}

	HRESULT STDMETHODCALLTYPE AppxPackageDriverDependency::GetMany(UINT32 startIndex, UINT32 capacity, struct AppxPackageDriverConstraint* value, UINT32* actual)
	{
		if (InitOnceExecuteOnce(&m_InitOnce, InitListStatic, this, nullptr))
		{
			return E_NOTIMPL;
		}
		else
		{ return HRESULT_FROM_WIN32(GetLastError()); }
	}

	//Windows.Foundation.Collections.IIterable<AppxPackageDriverConstraint>
	HRESULT STDMETHODCALLTYPE AppxPackageDriverDependency::First(ABI::IIterator<struct AppxPackageDriverConstraint>** first)
	{
		if (InitOnceExecuteOnce(&m_InitOnce, InitListStatic, this, nullptr))
		{
			auto* instance{ new AppxPackageDriverDependencyIterator(this, m_Array, m_Count) };
			if (instance)
			{
				*first = instance;
				return S_OK;
			}
			else
			{ return E_OUTOFMEMORY; }
		}
		else
		{ return HRESULT_FROM_WIN32(GetLastError()); }
	}

	//IAppxPackageDriverDependencyInterop
	HRESULT STDMETHODCALLTYPE AppxPackageDriverDependency::get_DriverDependency(IAppxManifestDriverDependency** value)
	{
		m_DriverDependency->AddRef();
		*value = m_DriverDependency;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxPackageDriverDependency::get_DriverConstraints(IAppxManifestDriverConstraintsEnumerator** value)
	{
		return m_DriverDependency->GetDriverConstraints(value);
	}

	//IInspectable
	HRESULT STDMETHODCALLTYPE AppxPackageDriverDependency::GetRuntimeClassName(HSTRING* className)
	{ return WindowsCreateString(L"AppxUtils.AppxPackageDriverDependency", 37, className); }

	//Destructor
	AppxPackageDriverDependency::~AppxPackageDriverDependency() noexcept
	{
		m_DriverDependency->Release();
		if (m_Array)
		{
			for (UINT32 i{ 0 }; i < m_Count; ++i)
			{ StructLifetimeFunctions<struct AppxPackageDriverConstraint>::ReleaseStruct(m_Array[i]); }
			delete[] m_Array;
		}
	}

	//Static members
	BOOL WINAPI AppxPackageDriverDependency::InitListStatic(INIT_ONCE* InitOnce, void* Parameter, void** Context)
	{
		auto* externalThis{ reinterpret_cast<AppxPackageDriverDependency*>(Parameter) };
		HRESULT hr{ S_OK };
		IAppxManifestDriverConstraintsEnumerator* enumerator{ nullptr };
		hr = externalThis->m_DriverDependency->GetDriverConstraints(&enumerator);
		if (SUCCEEDED(hr))
		{
			UINT32 count{ 0 };
			BOOL hasNext{ false };
			enumerator->GetHasCurrent(&hasNext);
			while (hasNext)
			{
				++count;
				enumerator->MoveNext(&hasNext);
			}
			enumerator->Release();
			hr = externalThis->m_DriverDependency->GetDriverConstraints(&enumerator);
			if (SUCCEEDED(hr))
			{
				auto* constraints{ new struct AppxPackageDriverConstraint[count] };
				if (constraints)
				{
					enumerator->GetHasCurrent(&hasNext);
					UINT32 completed{ 0 };
					while (hasNext)
					{
						IAppxManifestDriverConstraint* element{ nullptr };
						hr = enumerator->GetCurrent(&element);
						if (SUCCEEDED(hr))
						{
							auto& constraint{ constraints[completed] };
							LPWSTR name{ nullptr };
							hr = element->GetName(&name);
							if (SUCCEEDED(hr))
							{
								UINT64 minVer{};
								hr = element->GetMinVersion(&minVer);
								if (SUCCEEDED(hr))
								{
									LPWSTR minDate{ nullptr };
									hr = element->GetMinDate(&minDate);
									if (SUCCEEDED(hr))
									{
										hr = WindowsCreateString(name, WStringLength(name), &constraint.Name);
										if (SUCCEEDED(hr))
										{
											hr = Reference<ABI::PackageVersion>::CreateInstance(UInt64ToPkgVer(minVer), constraint.MinVersion);
											if (SUCCEEDED(hr))
											{
												hr = WindowsCreateString(minDate, WStringLength(minDate), &constraint.MinDate);
												if (SUCCEEDED(hr))
												{
													++completed;
													enumerator->MoveNext(&hasNext);
												}
												else
												{
													constraint.MinVersion->Release();
													WindowsDeleteString(constraint.Name);
												}
											}
											else
											{ WindowsDeleteString(constraint.Name); }
										}
										CoTaskMemFree(minDate);
									}
								}
								CoTaskMemFree(name);
							}
							element->Release();
						}
						if (FAILED(hr))
						{
							for (UINT32 i{ 0 }; i < completed; ++i)
							{ StructLifetimeFunctions<struct AppxPackageDriverConstraint>::ReleaseStruct(externalThis->m_Array[i]); }
							delete[] constraints;
							break;
						}
					}
					if (SUCCEEDED(hr))
					{
						externalThis->m_Array = constraints;
						externalThis->m_Count = count;
					}
				}
				else
				{ hr = E_OUTOFMEMORY; }
				enumerator->Release();
			}
		}
		return SUCCEEDED(hr);
	}
}
