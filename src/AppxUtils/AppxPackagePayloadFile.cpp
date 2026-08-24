#include "pch.h"
#include "AppxPackagePayloadFile.h"
#include "helpers.hpp"

namespace ABI
{
	using namespace Windows::Foundation;
	using namespace Windows::Foundation::Collections;
	using namespace Windows::Storage;
	using namespace Windows::Storage::Streams;
}

namespace ABI::AppxUtils
{
	class BasicPropertiesDummyClass final : InspectableBase<BaseTrust, ABI::FileProperties::IBasicProperties>
	{

	};

	AppxPackagePayloadFile::AppxPackagePayloadFile(IAppxFile*& internalFile, CRITICAL_SECTION*& criticalSection) noexcept : m_AppxPayloadFile(internalFile), m_CriticalSection(criticalSection)
	{

	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::get_CompressionOption(AppxPackagePayloadFileCompressionOption* value)
	{
		long local{ InterlockedCompareExchange(reinterpret_cast<long*>(&m_HasCompressionOption), false, false) };
		if (local == false)
		{
			EnterCriticalSection(m_CriticalSection);
			HRESULT hr{ S_OK };
			if (!m_HasCompressionOption)
			{
				hr = m_AppxPayloadFile->GetCompressionOption(reinterpret_cast<APPX_COMPRESSION_OPTION*>(&m_CompressionOption));
				if (SUCCEEDED(hr))
				{ m_HasCompressionOption = true; }
			}
			LeaveCriticalSection(m_CriticalSection);
			if (FAILED(hr))
			{ return hr; }
		}

		*value = m_CompressionOption;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::get_RelativePath(HSTRING* value)
	{
		HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_RelativePath), nullptr, nullptr)) };
		if (local == nullptr)
		{
			EnterCriticalSection(m_CriticalSection);
			HRESULT hr{ S_OK };
			if (m_RelativePath)
			{
				LPWSTR relativePath{ nullptr };
				hr = m_AppxPayloadFile->GetName(&relativePath);
				if (SUCCEEDED(hr))
				{
					const UINT32 length{ WStringLength(relativePath) };
					for (UINT32 i{ 0 }; i < length; ++i)
					{
						auto& element{ relativePath[i] };
						if (element == L'/')
						{ element = L'\\'; }
					}
					hr = WindowsCreateString(relativePath, length, &m_RelativePath);
					CoTaskMemFree(relativePath);
				}
			}
			LeaveCriticalSection(m_CriticalSection);
			if (FAILED(hr))
			{ return hr; }
			local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_RelativePath), nullptr, nullptr));
		}

		return WindowsDuplicateString(local, value);
	}

	//Windows.Storage.IStorageFile
	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::get_FileType(HSTRING* value)
	{
		HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_FileType), nullptr, nullptr)) };
		if (local == nullptr)
		{
			EnterCriticalSection(m_CriticalSection);
			HRESULT hr{ S_OK };
			if (m_FileType)
			{
				LPWSTR relativePath{ nullptr };
				hr = m_AppxPayloadFile->GetName(&relativePath);
				if (SUCCEEDED(hr))
				{
					const UINT32 length{ WStringLength(relativePath) };
					UINT32 pos{ length - 1 };
					bool hasExt{ true };
					while (relativePath[pos] != L'.')
					{
						if (relativePath[pos] != L'/' && relativePath[pos] != L'\\')
						{
							if (pos)
							{ --pos; }
							else
							{
								hasExt = false;
								break;
							}
						}
						else
						{
							pos = length - 1;
							hasExt = false;
							break;
						}
					}
					if (hasExt)
					{ hr = WindowsCreateString(relativePath + pos, length - pos, &m_FileType); }
					else
					{ hr = WindowsCreateString(nullptr, 0, &m_FileType); }
					CoTaskMemFree(relativePath);
				}
			}
			LeaveCriticalSection(m_CriticalSection);
			if (FAILED(hr))
			{ return hr; }
			local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_FileType), nullptr, nullptr));
		}

		return WindowsDuplicateString(local, value);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::get_ContentType(HSTRING* value)
	{
		HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_ContentType), nullptr, nullptr)) };
		if (local == nullptr)
		{
			EnterCriticalSection(m_CriticalSection);
			HRESULT hr{ S_OK };
			if (m_ContentType)
			{
				LPWSTR contentType{ nullptr };
				hr = m_AppxPayloadFile->GetContentType(&contentType);
				if (SUCCEEDED(hr))
				{
					hr = WindowsCreateString(contentType, WStringLength(contentType), &m_ContentType);
					CoTaskMemFree(contentType);
				}
			}
			LeaveCriticalSection(m_CriticalSection);
			if (FAILED(hr))
			{ return hr; }
			local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_ContentType), nullptr, nullptr));
		}

		return WindowsDuplicateString(local, value);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::OpenAsync(ABI::FileAccessMode accessMode, ABI::IAsyncOperation<ABI::IRandomAccessStream*>** operation)
	{
		if (accessMode == ABI::FileAccessMode_Read)
		{
			return E_NOTIMPL;
		}
		else
		{ return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED); }
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::OpenTransactedWriteAsync(ABI::IAsyncOperation<ABI::StorageStreamTransaction*>** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::CopyOverloadDefaultNameAndOptions(ABI::IStorageFolder* destinationFolder, ABI::IAsyncOperation<ABI::StorageFile*>** operation)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::CopyOverloadDefaultOptions(ABI::IStorageFolder* destinationFolder, HSTRING desiredNewName, ABI::IAsyncOperation<ABI::StorageFile*>** operation)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::CopyOverload(ABI::IStorageFolder* destinationFolder, HSTRING desiredNewName, ABI::NameCollisionOption option, ABI::IAsyncOperation < ABI::StorageFile*>** operation)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::CopyAndReplaceAsync(ABI::IStorageFile* fileToReplace, ABI::IAsyncAction** operation)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::MoveOverloadDefaultNameAndOptions(ABI::IStorageFolder* destinationFolder, ABI::IAsyncAction** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::MoveOverloadDefaultOptions(ABI::IStorageFolder* destinationFolder, HSTRING desiredNewName, ABI::IAsyncAction** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::MoveOverload(ABI::IStorageFolder* destinationFolder, HSTRING desiredNewName, ABI::NameCollisionOption option, ABI::IAsyncAction** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::MoveAndReplaceAsync(ABI::IStorageFile* fileToReplace, ABI::IAsyncAction** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	//Windows.Storage.IStorageFile2
	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::OpenWithOptionsAsync(ABI::FileAccessMode accessMode, ABI::StorageOpenOptions options, ABI::IAsyncOperation<ABI::IRandomAccessStream*>** operation)
	{
		if (accessMode == ABI::FileAccessMode_Read && options != ABI::StorageOpenOptions_AllowReadersAndWriters)
		{
			return E_NOTIMPL;
		}
		else
		{
			return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
		}
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::OpenTransactedWriteWithOptionsAsync(ABI::StorageOpenOptions options, ABI::IAsyncOperation<ABI::StorageStreamTransaction*>** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	//Windows.Storage.IStorageItem
	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::RenameAsyncOverloadDefaultOptions(HSTRING desiredName, ABI::IAsyncAction** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::RenameAsync(HSTRING desiredName, ABI::NameCollisionOption option, ABI::IAsyncAction** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::DeleteAsyncOverloadDefaultOptions(ABI::IAsyncAction** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::DeleteAsync(ABI::StorageDeleteOption option, ABI::IAsyncAction** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::GetBasicPropertiesAsync(ABI::IAsyncOperation<ABI::FileProperties::BasicProperties*>** operation)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::get_Name(HSTRING* value)
	{
		HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Name), nullptr, nullptr)) };
		if (local == nullptr)
		{
			EnterCriticalSection(m_CriticalSection);
			HRESULT hr{ S_OK };
			if (m_Name)
			{
				LPWSTR relativePath{ nullptr };
				hr = m_AppxPayloadFile->GetName(&relativePath);
				if (SUCCEEDED(hr))
				{
					const UINT32 length{ WStringLength(relativePath) };
					UINT32 pos{ length - 1 };
					bool hasSeparator{ true };
					while (relativePath[pos] != L'\\' && relativePath[pos] != L'/')
					{
						if (pos)
						{ --pos; }
						else
						{
							pos = length - 1;
							hasSeparator = false;
							break;
						}
					}
					if (hasSeparator)
					{ hr = WindowsCreateString(relativePath + pos + 1, length + 1 - pos, &m_Name); }
					else
					{ hr = WindowsCreateString(relativePath, length, &m_Name); }
					CoTaskMemFree(relativePath);
				}
			}
			LeaveCriticalSection(m_CriticalSection);
			if (FAILED(hr))
			{ return hr; }
			local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Name), nullptr, nullptr));
		}

		return WindowsDuplicateString(local, value);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::get_Path(HSTRING* value)
	{
		*value = nullptr;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::get_Attributes(ABI::FileAttributes* value)
	{
		*value = ABI::FileAttributes_ReadOnly;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::get_DateCreated(ABI::DateTime* value)
	{
		value->UniversalTime = 116,444,736,000,000,000;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::IsOfType(ABI::StorageItemTypes type, boolean* value)
	{
		*value = type == ABI::StorageItemTypes_File;
		return S_OK;
	}

	//Windows.Storage.IStorageFilePropertiesWithAvalibility
	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::get_IsAvailable(boolean* value)
	{
		*value = true;
		return S_OK;
	}

	//Windows.Storage.Streams.IInputStreamReference
	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::OpenSequentialReadAsync(ABI::IAsyncOperation<ABI::IInputStream*>** operation)
	{
		return E_NOTIMPL;
	}

	//Windows.Storage.Streams.IRandomAccessStreamReference
	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::OpenReadAsync(ABI::IAsyncOperation<ABI::IRandomAccessStreamWithContentType*>** operation)
	{
		return E_NOTIMPL;
	}

	//IAppxPackagePayloadFileInterop
	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::get_AppxPayloadFile(IAppxFile** value)
	{
		m_AppxPayloadFile->AddRef();
		*value = m_AppxPayloadFile;
		return S_OK;
	}

	//IInspectable
	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFile::GetRuntimeClassName(HSTRING* className)
	{ return WindowsCreateString(L"AppxUtils.AppxPackagePayloadFile", 32, className); }

	//Destructor
	AppxPackagePayloadFile::~AppxPackagePayloadFile() noexcept
	{
		m_AppxPayloadFile->Release();

		if (m_RelativePath)
		{ WindowsDeleteString(m_RelativePath); }

		if (m_FileType)
		{ WindowsDeleteString(m_FileType); }
		if (m_ContentType)
		{ WindowsDeleteString(m_ContentType); }

		if (m_Name)
		{ WindowsDeleteString(m_Name); }

		DeleteCriticalSection(m_CriticalSection);
	}
}
