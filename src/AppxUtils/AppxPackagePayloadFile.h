#pragma once

namespace ABI::AppxUtils
{
	class AppxPackagePayloadFile final : public InspectableElementBase<BaseTrust,
        IAppxPackagePayloadFile,
        ABI::Windows::Storage::IStorageFile,
        ABI::Windows::Storage::IStorageFile2,
        ABI::Windows::Storage::IStorageItem,
        ABI::Windows::Storage::IStorageFilePropertiesWithAvailability,
        ABI::Windows::Storage::Streams::IInputStreamReference,
        ABI::Windows::Storage::Streams::IRandomAccessStreamReference,
        IAppxPackagePayloadFileInterop,
        IAgileObject
    >
	{
	public:
		AppxPackagePayloadFile(IAppxFile*& internalFile, CRITICAL_SECTION*& criticalSection) noexcept;

		HRESULT STDMETHODCALLTYPE get_CompressionOption(ABI::AppxUtils::AppxPackagePayloadFileCompressionOption* value);
		HRESULT STDMETHODCALLTYPE get_RelativePath(HSTRING* value);

		//Windows.Storage.IStorageFile
        HRESULT STDMETHODCALLTYPE get_FileType(HSTRING* value);
        HRESULT STDMETHODCALLTYPE get_ContentType(HSTRING* value);
        HRESULT STDMETHODCALLTYPE OpenAsync(ABI::Windows::Storage::FileAccessMode accessMode, ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Storage::Streams::IRandomAccessStream*>** operation);
        HRESULT STDMETHODCALLTYPE OpenTransactedWriteAsync(ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Storage::StorageStreamTransaction*>** operation);
        HRESULT STDMETHODCALLTYPE CopyOverloadDefaultNameAndOptions(ABI::Windows::Storage::IStorageFolder* destinationFolder, ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Storage::StorageFile*>** operation);
        HRESULT STDMETHODCALLTYPE CopyOverloadDefaultOptions(ABI::Windows::Storage::IStorageFolder* destinationFolder, HSTRING desiredNewName, ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Storage::StorageFile*>** operation);
        HRESULT STDMETHODCALLTYPE CopyOverload(ABI::Windows::Storage::IStorageFolder* destinationFolder, HSTRING desiredNewName, ABI::Windows::Storage::NameCollisionOption option, ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Storage::StorageFile*>** operation);
        HRESULT STDMETHODCALLTYPE CopyAndReplaceAsync(ABI::Windows::Storage::IStorageFile* fileToReplace, ABI::Windows::Foundation::IAsyncAction** operation);
        HRESULT STDMETHODCALLTYPE MoveOverloadDefaultNameAndOptions( ABI::Windows::Storage::IStorageFolder* destinationFolder, ABI::Windows::Foundation::IAsyncAction** operation);
        HRESULT STDMETHODCALLTYPE MoveOverloadDefaultOptions(ABI::Windows::Storage::IStorageFolder* destinationFolder, HSTRING desiredNewName, ABI::Windows::Foundation::IAsyncAction** operation);
        HRESULT STDMETHODCALLTYPE MoveOverload(ABI::Windows::Storage::IStorageFolder* destinationFolder, HSTRING desiredNewName, ABI::Windows::Storage::NameCollisionOption option, ABI::Windows::Foundation::IAsyncAction** operation);
        HRESULT STDMETHODCALLTYPE MoveAndReplaceAsync(ABI::Windows::Storage::IStorageFile* fileToReplace, ABI::Windows::Foundation::IAsyncAction** operation);

        //Windows.Storage.IStorageFile2
        HRESULT STDMETHODCALLTYPE OpenWithOptionsAsync(ABI::Windows::Storage::FileAccessMode accessMode, ABI::Windows::Storage::StorageOpenOptions options, ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Storage::Streams::IRandomAccessStream*>** operation);
        HRESULT STDMETHODCALLTYPE OpenTransactedWriteWithOptionsAsync(ABI::Windows::Storage::StorageOpenOptions options, ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Storage::StorageStreamTransaction*>** operation);

        //Windows.Storage.IStorageItem
        HRESULT STDMETHODCALLTYPE RenameAsyncOverloadDefaultOptions(HSTRING desiredName, ABI::Windows::Foundation::IAsyncAction** operation);
        HRESULT STDMETHODCALLTYPE RenameAsync(HSTRING desiredName, ABI::Windows::Storage::NameCollisionOption option, ABI::Windows::Foundation::IAsyncAction** operation);
        HRESULT STDMETHODCALLTYPE DeleteAsyncOverloadDefaultOptions(ABI::Windows::Foundation::IAsyncAction** operation);
        HRESULT STDMETHODCALLTYPE DeleteAsync(ABI::Windows::Storage::StorageDeleteOption option,ABI::Windows::Foundation::IAsyncAction** operation);
        HRESULT STDMETHODCALLTYPE GetBasicPropertiesAsync(ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Storage::FileProperties::BasicProperties*>** operation);
        HRESULT STDMETHODCALLTYPE get_Name(HSTRING* value);
        HRESULT STDMETHODCALLTYPE get_Path(HSTRING* value);
        HRESULT STDMETHODCALLTYPE get_Attributes(ABI::Windows::Storage::FileAttributes* value);
        HRESULT STDMETHODCALLTYPE get_DateCreated(ABI::Windows::Foundation::DateTime* value);
        HRESULT STDMETHODCALLTYPE IsOfType(ABI::Windows::Storage::StorageItemTypes type, boolean* value);

        //Windows.Storage.IStorageFilePropertiesWithAvalibility
        HRESULT STDMETHODCALLTYPE get_IsAvailable(boolean* value);

        //Windows.Storage.Streams.IInputStreamReference
        HRESULT STDMETHODCALLTYPE OpenSequentialReadAsync(ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Storage::Streams::IInputStream*>** operation);

        //Windows.Storage.Streams.IRandomAccessStreamReference
        HRESULT STDMETHODCALLTYPE OpenReadAsync(ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Storage::Streams::IRandomAccessStreamWithContentType*>** operation);

        //IAppxPackagePayloadFileInterop
        HRESULT STDMETHODCALLTYPE get_AppxPayloadFile(IAppxFile** value);

		//IInspectable
        HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className);

        ~AppxPackagePayloadFile() noexcept;

    private:
        IAppxFile* m_AppxPayloadFile{ nullptr };

        BOOL m_HasCompressionOption{ false };
        AppxPackagePayloadFileCompressionOption m_CompressionOption{};
        HSTRING m_RelativePath{ nullptr };

        HSTRING m_FileType{ nullptr };
        HSTRING m_ContentType{ nullptr };

        HSTRING m_Name{ nullptr };

        CRITICAL_SECTION* m_CriticalSection{ nullptr };
	};
}
