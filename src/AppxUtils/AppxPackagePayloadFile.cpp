// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
#include "AppxPackagePayloadFile.h"
#include "helpers.hpp"

namespace ABI
{
	using namespace Windows::Foundation;
	using namespace Windows::Foundation::Collections;
	using namespace Windows::Storage;
	using namespace Windows::Storage::Streams;
	using namespace AppxUtils::Internal;
}

namespace ABI::AppxUtils::Internal
{
	template<UINT32 capacity>
	class StreamReaderBuffer final : public InspectableBase<BaseTrust,
		ABI::IBuffer,
		::Windows::Storage::Streams::IBufferByteAccess,
		IAgileObject
	>
	{
	public:
		StreamReaderBuffer() noexcept = default;

		//IBuffer
		HRESULT STDMETHODCALLTYPE get_Capacity(UINT32* value)
		{
			*value = capacity;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE get_Length(UINT32* value)
		{
			*value = InterlockedCompareExchange(&m_Length, 0, 0);
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE put_Length(UINT32 value)
		{
			if (value <= capacity)
			{
				InterlockedExchange(&m_Length, value);
				return S_OK;
			}
			else
			{
				return E_INVALIDARG;
			}
		}

		//IBufferByteAccess
		HRESULT STDMETHODCALLTYPE Buffer(byte** value)
		{
			*value = m_Buffer;
			return S_OK;
		}

		//IInspectable
		HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className)
		{
			return WindowsCreateString(L"Windows.Storage.Streams.IBuffer", 31, className);
		}

	private:
		UINT32 m_Length{ 0 };
		byte m_Buffer[capacity]{ 0x00 };
	};

	class AppxPackagePayloadFileCopyAsyncOp final : public AsyncOperation<ABI::StorageFile*>
	{
	public:
		HRESULT STDMETHODCALLTYPE LaunclAsyncTask()
		{
			this->AddRef();
			if (TrySubmitThreadpoolCallback(CopyFileCallback, this, nullptr))
			{ return S_OK; }
			else
			{
				this->Release();
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		void STDMETHODCALLTYPE ReleaseResourcesInternal()
		{
			reinterpret_cast<IUnknown*>(m_Source)->Release();
			WindowsDeleteString(m_DesiredName);
			if (m_Folder)
			{ m_Folder->Release(); }
			else
			{
				m_GIT->RevokeInterfaceFromGlobal(m_FolderGITCookie);
				m_GIT->Release();
			}
		}

		//IAsyncInfo
		HRESULT STDMETHODCALLTYPE Close() override
		{
			HRESULT hr{ S_OK };
			AcquireSRWLockExclusive(&m_Lock);
			const bool opening{ m_IsOpening != 0 };
			if (static_cast<ABI::AsyncStatus>(m_Status) == ABI::AsyncStatus::Started)
			{ hr = E_ILLEGAL_METHOD_CALL; }
			else
			{
				m_IsOpening = false;
			}
			auto* const result{ m_Result };
			m_Result = nullptr;
			ReleaseSRWLockExclusive(&m_Lock);
			if (hr == S_OK && opening)
			{
				if (result)
				{ result->Release(); }
				ReleaseResourcesInternal();
			}
			return hr;
		}

		static HRESULT STDMETHODCALLTYPE CreateInstance(AppxPackagePayloadFileBase* const source, ABI::IStorageFolder* const destinationFolder,
			const HSTRING desiredName, const ABI::NameCollisionOption option,
			AppxPackagePayloadFileCopyAsyncOp*& result)
		{
			HRESULT hr{ S_OK };
			HSTRING nameCopy{ nullptr };
			hr = WindowsDuplicateString(desiredName, &nameCopy);
			if (SUCCEEDED(hr))
			{
				IAgileObject* agileObj{ nullptr };
				if (SUCCEEDED(destinationFolder->QueryInterface(__uuidof(agileObj), to_void_pp(agileObj))))
				{
					agileObj->Release();
					result = new AppxPackagePayloadFileCopyAsyncOp{ source, destinationFolder, nameCopy, option };
					if (result)
					{ return S_OK; }
					else
					{
						hr = E_OUTOFMEMORY;
						WindowsDeleteString(nameCopy);
					}
				}
				else
				{
					IGlobalInterfaceTable* pGIT{ nullptr };
					hr = CoCreateInstance(CLSID_StdGlobalInterfaceTable, nullptr, CLSCTX_INPROC_SERVER, __uuidof(pGIT), to_void_pp(pGIT));
					if (SUCCEEDED(hr))
					{
						DWORD cookie{ 0 };
						hr = pGIT->RegisterInterfaceInGlobal(destinationFolder, __uuidof(destinationFolder), &cookie);
						if (SUCCEEDED(hr))
						{
							result = new AppxPackagePayloadFileCopyAsyncOp{ source, pGIT, cookie, nameCopy, option };
							if (result)
							{ return S_OK; }
							else
							{
								hr = E_OUTOFMEMORY;
								pGIT->RevokeInterfaceFromGlobal(cookie);
								pGIT->Release();
							}
						}
						else
						{ pGIT->Release(); }
					}
				}
			}
		}

	private:
		AppxPackagePayloadFileBase* const m_Source{ nullptr };
		IGlobalInterfaceTable* const m_GIT{ nullptr };
		ABI::IStorageFolder* const m_Folder{ nullptr };
		const HSTRING m_DesiredName{ nullptr };
		const DWORD m_FolderGITCookie{};
		const ABI::NameCollisionOption m_Option{ ABI::NameCollisionOption_FailIfExists };

		AppxPackagePayloadFileCopyAsyncOp(AppxPackagePayloadFileBase* const source,
			ABI::IStorageFolder* const folder, const HSTRING desiredName, const ABI::NameCollisionOption option) noexcept :
			m_Source(source), m_DesiredName(desiredName), m_Folder(folder), m_Option(option)
		{
			source->AddRef();
			folder->AddRef();
		}

		AppxPackagePayloadFileCopyAsyncOp(AppxPackagePayloadFileBase* const source, IGlobalInterfaceTable* const pGIT,
			const DWORD cookie, const HSTRING desiredName, const ABI::NameCollisionOption option) noexcept :
			m_Source(source), m_DesiredName(desiredName), m_GIT(pGIT), m_FolderGITCookie(cookie), m_Option(option)
		{
			source->AddRef();
		}

		static void CALLBACK CopyFileCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context)
		{
			auto& external{ *static_cast<AppxPackagePayloadFileCopyAsyncOp*>(Context) };
			HRESULT hr{ RoInitialize(RO_INIT_MULTITHREADED) };
			ABI::IStorageFile* result{ nullptr };
			if (SUCCEEDED(hr))
			{
				ABI::IStorageFolder* folder{ external.m_Folder };
				if (folder)
				{ folder->AddRef(); }
				else
				{ hr = external.m_GIT->GetInterfaceFromGlobal(external.m_FolderGITCookie, __uuidof(folder), to_void_pp(folder)); }
				if (SUCCEEDED(hr))
				{
					ABI::IRandomAccessStream* stream{ nullptr };
					hr = external.m_Source->GetStream(stream);
					if (SUCCEEDED(hr))
					{
						ABI::IAsyncOperation<ABI::StorageFile*>* async0{ nullptr };
						hr = folder->CreateFileAsync(external.m_DesiredName, static_cast<ABI::CreationCollisionOption>(external.m_Option), &async0);
						if (SUCCEEDED(hr))
						{
							ABI::IStorageFile* file{ nullptr };
							hr = AsyncOperationUtils<ABI::StorageFile*>::AwaitAsyncObject(async0, file);
							if (SUCCEEDED(hr))
							{
								UINT64 size{ 0 };
								hr = stream->get_Size(&size);
								if (SUCCEEDED(hr))
								{
									if (size > 0)
									{
										ABI::IAsyncOperation<ABI::IRandomAccessStream*>* async1{ nullptr };
										hr = file->OpenAsync(ABI::FileAccessMode_ReadWrite, &async1);
										if (SUCCEEDED(hr))
										{
											ABI::IRandomAccessStream* fileStream{ nullptr };
											hr = AsyncOperationUtils<ABI::IRandomAccessStream*>::AwaitAsyncObject(async1, fileStream);
											if (SUCCEEDED(hr))
											{
												hr = fileStream->put_Size(size);
												if (SUCCEEDED(hr))
												{
													ABI::IOutputStream* outStream{ nullptr };
													fileStream->QueryInterface(__uuidof(outStream), to_void_pp(outStream));
													ABI::IAsyncOperation<bool>* async2{ nullptr };
													hr = outStream->FlushAsync(&async2);
													if (SUCCEEDED(hr))
													{
														boolean flushResult{};
														hr = AsyncOperationUtils<bool>::AwaitAsyncObject(async2, flushResult);
														if (SUCCEEDED(hr))
														{
															hr = fileStream->Seek(0);
															if (SUCCEEDED(hr))
															{
																auto* const buffer{ new StreamReaderBuffer<16 * 1024>{  } };
																if (buffer)
																{
																	ABI::IInputStream* inputStream{ nullptr };
																	stream->QueryInterface(__uuidof(inputStream), to_void_pp(inputStream));
																	const UINT64 times{ size / (16 * 1024) + 1 };
																	for (UINT64 i{ 0 }; i < times; ++i)
																	{
																		ABI::IAsyncOperationWithProgress<ABI::IBuffer*, UINT32>* async3{ nullptr };
																		hr = inputStream->ReadAsync(buffer, 16 * 1024, ABI::InputStreamOptions_ReadAhead, &async3);
																		if (SUCCEEDED(hr))
																		{
																			ABI::IBuffer* dataBuffer{ nullptr };
																			hr = AsyncOperationWithProgressUtils<ABI::IBuffer*, UINT32>::AwaitAsyncObject(async3, dataBuffer);
																			if (SUCCEEDED(hr))
																			{
																				ABI::IAsyncOperationWithProgress<UINT32, UINT32>* async4{ nullptr };
																				hr = outStream->WriteAsync(dataBuffer, &async4);
																				if (SUCCEEDED(hr))
																				{
																					UINT32 wroteResult{};
																					hr = AsyncOperationWithProgressUtils<UINT32, UINT32>::AwaitAsyncObject(async4, wroteResult);
																			        AsyncOperationWithProgressUtils<UINT32, UINT32>::CloseAsyncObject(async4);
																					async4->Release();
																				}
																			}
																			AsyncOperationWithProgressUtils<ABI::IBuffer*, UINT32>::CloseAsyncObject(async3);
																			async3->Release();
																		}
																		if (FAILED(hr))
																		{ break; }
																	}
																	if (SUCCEEDED(hr))
																	{
																		hr = outStream->FlushAsync(&async2);
																		if (SUCCEEDED(hr))
																		{
																			hr = AsyncOperationUtils<bool>::AwaitAsyncObject(async2, flushResult);
																			if (SUCCEEDED(hr))
																			{ result = file; }
																		}
																	}
																	if (FAILED(hr))
																	{
																		ABI::IStorageItem* item{ nullptr };
																		file->QueryInterface(__uuidof(item), to_void_pp(item));
																		ABI::IAsyncAction* async3{ nullptr };
																		if (SUCCEEDED(item->DeleteAsync(ABI::StorageDeleteOption_PermanentDelete, &async3)))
																		{
																			AsyncActionUtils::AwaitAsyncObject(async3);
																			AsyncActionUtils::CloseAsyncObject(async3);
																			async3->Release();
																		}
																	}
																	inputStream->Release();
																	buffer->Release();
																}
																else
																{ hr = E_OUTOFMEMORY; }
															}
														}
														AsyncOperationUtils<bool>::CloseAsyncObject(async2);
														async2->Release();
													}
													outStream->Release();
												}
												TryCloseObject(fileStream);
												fileStream->Release();
											}
										}
									}
									else
									{
										file->AddRef();
										result = file;
									}
								}
								file->Release();
							}
							AsyncOperationUtils<ABI::StorageFile*>::CloseAsyncObject(async0);
							async0->Release();
						}
						TryCloseObject(stream);
						stream->Release();
					}
					folder->Release();
				}
				RoUninitialize();
			}
			LONG status{};
			AcquireSRWLockExclusive(&external.m_Lock);
			if (result)
			{
				external.m_Result = result;
				external.m_ErrorCode = S_OK;
				status = static_cast<LONG>(ABI::AsyncStatus::Completed);
			}
			else if (external.m_CanContinue)
			{
				external.m_ErrorCode = hr;
				status = static_cast<LONG>(ABI::AsyncStatus::Error);
			}
			else
			{
				external.m_ErrorCode = E_CANCELED;
				status = static_cast<LONG>(ABI::AsyncStatus::Canceled);
			}
			external.m_Status = status;
			auto* const handler{ external.m_Completed };
			ReleaseSRWLockExclusive(&external.m_Lock);
			if (handler)
			{
				handler->Invoke(&external, static_cast<ABI::AsyncStatus>(status));
				handler->Release();
				AcquireSRWLockExclusive(&external.m_Lock);
				external.m_Completed = nullptr;
				ReleaseSRWLockExclusive(&external.m_Lock);
			}
			external.Release();
		}
	};

	class AppxPackagePayloadFileOpenAsyncOpWrapper final : public AsyncOperation<ABI::IRandomAccessStream*>
	{
	public:
		AppxPackagePayloadFileOpenAsyncOpWrapper(ABI::IRandomAccessStream* stream) noexcept
		{
			m_Result = stream;
			m_Status = static_cast<LONG>(ABI::AsyncStatus::Completed);
		}

		//IAsyncInfo
		HRESULT STDMETHODCALLTYPE Close()
		{
			HRESULT hr{ S_OK };
			AcquireSRWLockExclusive(&m_Lock);
			const bool opening{ m_IsOpening != 0 };
			if (static_cast<ABI::AsyncStatus>(m_Status) == ABI::AsyncStatus::Started)
			{ hr = E_ILLEGAL_METHOD_CALL; }
			else
			{
				m_IsOpening = false;
			}
			auto* const result{ m_Result };
			m_Result = nullptr;
			ReleaseSRWLockExclusive(&m_Lock);
			if (hr == S_OK && opening)
			{
				if (result)
				{ result->Release(); }
			}
			return hr;
		}
	};

	class AppxPackagePayloadFileOpenSequentialReadAsyncOpWrapper final : public AsyncOperation<ABI::IInputStream*>
	{
	public:
		AppxPackagePayloadFileOpenSequentialReadAsyncOpWrapper(ABI::IInputStream* stream) noexcept
		{
			m_Result = stream;
			m_Status = static_cast<LONG>(ABI::AsyncStatus::Completed);
		}

		//IAsyncInfo
		HRESULT STDMETHODCALLTYPE Close()
		{
			HRESULT hr{ S_OK };
			AcquireSRWLockExclusive(&m_Lock);
			const bool opening{ m_IsOpening != 0 };
			if (static_cast<ABI::AsyncStatus>(m_Status) == ABI::AsyncStatus::Started)
			{ hr = E_ILLEGAL_METHOD_CALL; }
			else
			{
				m_IsOpening = false;
			}
			auto* const result{ m_Result };
			m_Result = nullptr;
			ReleaseSRWLockExclusive(&m_Lock);
			if (hr == S_OK && opening)
			{
				if (result)
				{ result->Release(); }
			}
			return hr;
		}
	};

	class AppxPackagePayloadFileOpenReadAsyncOpWrapper final : public AsyncOperation<ABI::IRandomAccessStreamWithContentType*>
	{
	public:
		AppxPackagePayloadFileOpenReadAsyncOpWrapper(ABI::IRandomAccessStreamWithContentType* stream) noexcept
		{
			m_Result = stream;
			m_Status = static_cast<LONG>(ABI::AsyncStatus::Completed);
		}

		//IAsyncInfo
		HRESULT STDMETHODCALLTYPE Close()
		{
			HRESULT hr{ S_OK };
			AcquireSRWLockExclusive(&m_Lock);
			const bool opening{ m_IsOpening != 0 };
			if (static_cast<ABI::AsyncStatus>(m_Status) == ABI::AsyncStatus::Started)
			{ hr = E_ILLEGAL_METHOD_CALL; }
			else
			{
				m_IsOpening = false;
			}
			auto* const result{ m_Result };
			m_Result = nullptr;
			ReleaseSRWLockExclusive(&m_Lock);
			if (hr == S_OK && opening)
			{
				if (result)
				{ result->Release(); }
			}
			return hr;
		}
	};

	class AppxPackagePayloadFileOpenReadAsyncRandomAccessStreamWithContentType final : public InspectableBase<BaseTrust,
		ABI::IRandomAccessStreamWithContentType,
		ABI::IRandomAccessStream,
		ABI::IClosable,
		ABI::IInputStream,
		ABI::IOutputStream,
		ABI::IContentTypeProvider,
		IAgileObject
	>
	{
	public:
		AppxPackagePayloadFileOpenReadAsyncRandomAccessStreamWithContentType(ABI::IRandomAccessStream* stream, HSTRING contentType) noexcept :
			m_Stream(stream), m_ContentType(contentType)
		{

		}

#pragma region IRandomAccessStream
		HRESULT STDMETHODCALLTYPE get_Size(UINT64* value)
		{ return m_Stream->get_Size(value); }

		HRESULT STDMETHODCALLTYPE put_Size(UINT64 value)
		{ return m_Stream->put_Size(value); }

		HRESULT STDMETHODCALLTYPE GetInputStreamAt(UINT64 position, ABI::IInputStream** stream)
		{ return m_Stream->GetInputStreamAt(position, stream); }

		HRESULT STDMETHODCALLTYPE GetOutputStreamAt(UINT64 position, ABI::IOutputStream** stream)
		{ return m_Stream->GetOutputStreamAt(position, stream); }

		HRESULT STDMETHODCALLTYPE get_Position(UINT64* value)
		{ return m_Stream->get_Position(value); }

		HRESULT STDMETHODCALLTYPE Seek(UINT64 position)
		{ return m_Stream->Seek(position); }

		HRESULT STDMETHODCALLTYPE CloneStream(ABI::IRandomAccessStream** stream)
		{ return m_Stream->CloneStream(stream); }

		HRESULT STDMETHODCALLTYPE get_CanRead(boolean* value)
		{ return m_Stream->get_CanRead(value); }

		HRESULT STDMETHODCALLTYPE get_CanWrite(boolean* value)
		{ return m_Stream->get_CanWrite(value); }
#pragma endregion

#pragma region IClosable
		HRESULT STDMETHODCALLTYPE Close()
		{
			if (InterlockedExchange16(&m_IsNotClosed, false))
			{
				ABI::IClosable* closable{ nullptr };
				m_Stream->QueryInterface(__uuidof(closable), to_void_pp(closable));
				closable->Close();
				closable->Release();
				m_Stream->Release();
				WindowsDeleteString(m_ContentType);
			}
			return S_OK;
		}
#pragma endregion

#pragma region IInputStream
		HRESULT STDMETHODCALLTYPE ReadAsync(ABI::IBuffer* buffer, UINT32 count, ABI::InputStreamOptions options, ABI::IAsyncOperationWithProgress<ABI::IBuffer*, UINT32>** operation)
		{
			ABI::IInputStream* inputStream{ nullptr };
			m_Stream->QueryInterface(__uuidof(inputStream), to_void_pp(inputStream));
			const HRESULT hr{ inputStream->ReadAsync(buffer, count, options, operation) };
			inputStream->Release();
			return hr;
		}
#pragma endregion

#pragma region IOutputStream
		HRESULT STDMETHODCALLTYPE WriteAsync(ABI::IBuffer* buffer, ABI::IAsyncOperationWithProgress<UINT32, UINT32>** operation)
		{
			ABI::IOutputStream* outputStream{ nullptr };
			m_Stream->QueryInterface(__uuidof(outputStream), to_void_pp(outputStream));
			const HRESULT hr{ outputStream->WriteAsync(buffer, operation) };
			outputStream->Release();
			return hr;
		}

		HRESULT STDMETHODCALLTYPE FlushAsync(ABI::IAsyncOperation<bool>** operation)
		{
			ABI::IOutputStream* outputStream{ nullptr };
			m_Stream->QueryInterface(__uuidof(outputStream), to_void_pp(outputStream));
			const HRESULT hr{ outputStream->FlushAsync(operation) };
			outputStream->Release();
			return hr;
		}
#pragma endregion

#pragma region IContentTypeProvider
		HRESULT STDMETHODCALLTYPE get_ContentType(HSTRING* value)
		{
			if (InterlockedCompareExchange16(&m_IsNotClosed, false, false))
			{ return WindowsDuplicateString(m_ContentType, value); }
			else
			{ return RO_E_CLOSED; }
		}
#pragma endregion

#pragma region IInspectable
		HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className)
		{ return WindowsCreateString(L"Windows.Storage.Streams.IRandomAccessStreamWithContentType", 58, className); }
#pragma endregion

	private:
		ABI::IRandomAccessStream* const m_Stream{ nullptr };
		const HSTRING m_ContentType{ nullptr };
		short m_IsNotClosed{ true };
	};
}

namespace ABI::AppxUtils
{
#pragma region AppxPackagePayloadFileBase
	AppxPackagePayloadFileBase::AppxPackagePayloadFileBase(IAppxFile*& internalFile) noexcept : m_AppxPayloadFile(internalFile)
	{

	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::GetStream(ABI::IRandomAccessStream*& rdmStream)
	{
		HRESULT hr{ S_OK };
		IStream* stream{ nullptr };
		hr = m_AppxPayloadFile->GetStream(&stream);
		if (SUCCEEDED(hr))
		{ hr = CreateRandomAccessStreamOverStream(stream, BSOS_DEFAULT, __uuidof(rdmStream), to_void_pp(rdmStream)); }
		return hr;
	}

#pragma region IAppxPackagePayloadFile
	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::get_CompressionOption(AppxPackagePayloadFileCompressionOption* value)
	{
		short local{ InterlockedCompareExchange16(&m_HasCompressionOption, false, false) };
		if (local == false)
		{
			AcquireSRWLockExclusive(&m_Lock);
			HRESULT hr{ S_OK };
			if (!m_HasCompressionOption)
			{
				hr = m_AppxPayloadFile->GetCompressionOption(reinterpret_cast<APPX_COMPRESSION_OPTION*>(&m_CompressionOption));
				if (SUCCEEDED(hr))
				{ m_HasCompressionOption = true; }
			}
			ReleaseSRWLockExclusive(&m_Lock);
			if (FAILED(hr))
			{ return hr; }
		}

		*value = m_CompressionOption;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::get_RelativePath(HSTRING* value)
	{
		HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_RelativePath), nullptr, nullptr)) };
		if (local == nullptr)
		{
			AcquireSRWLockExclusive(&m_Lock);
			HRESULT hr{ S_OK };
			if (!m_RelativePath)
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
			ReleaseSRWLockExclusive(&m_Lock);
			if (FAILED(hr))
			{ return hr; }
			local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_RelativePath), nullptr, nullptr));
		}

		return WindowsDuplicateString(local, value);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::get_UncompressedSize(UINT64* value)
	{
		short local{ InterlockedCompareExchange16(&m_HasUncompressedSize, false, false) };
		if (local == false)
		{
			AcquireSRWLockExclusive(&m_Lock);
			HRESULT hr{ S_OK };
			if (!m_HasUncompressedSize)
			{
				hr = m_AppxPayloadFile->GetSize(&m_UncompressedSize);
				if (SUCCEEDED(hr))
				{ m_HasUncompressedSize = true; }
			}
			ReleaseSRWLockExclusive(&m_Lock);
			if (FAILED(hr))
			{ return hr; }
		}

		*value = m_UncompressedSize;
		return S_OK;
	}
#pragma endregion

#pragma region Windows.Storage.IStorageFile
	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::get_FileType(HSTRING* value)
	{
		HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_FileType), nullptr, nullptr)) };
		if (local == nullptr)
		{
			AcquireSRWLockExclusive(&m_Lock);
			HRESULT hr{ S_OK };
			if (!m_FileType)
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
			ReleaseSRWLockExclusive(&m_Lock);
			if (FAILED(hr))
			{ return hr; }
			local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_FileType), nullptr, nullptr));
		}

		return WindowsDuplicateString(local, value);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::get_ContentType(HSTRING* value)
	{
		HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_ContentType), nullptr, nullptr)) };
		if (local == nullptr)
		{
			AcquireSRWLockExclusive(&m_Lock);
			HRESULT hr{ S_OK };
			if (!m_ContentType)
			{
				LPWSTR contentType{ nullptr };
				hr = m_AppxPayloadFile->GetContentType(&contentType);
				if (SUCCEEDED(hr))
				{
					hr = WindowsCreateString(contentType, WStringLength(contentType), &m_ContentType);
					CoTaskMemFree(contentType);
				}
			}
			ReleaseSRWLockExclusive(&m_Lock);
			if (FAILED(hr))
			{ return hr; }
			local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_ContentType), nullptr, nullptr));
		}

		return WindowsDuplicateString(local, value);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::OpenAsync(ABI::FileAccessMode accessMode, ABI::IAsyncOperation<ABI::IRandomAccessStream*>** operation)
	{
		if (accessMode == ABI::FileAccessMode_Read)
		{
			HRESULT hr{ S_OK };
			IStream* stream{ nullptr };
			hr = m_AppxPayloadFile->GetStream(&stream);
			if (SUCCEEDED(hr))
			{
				ABI::IRandomAccessStream* rdmStream{ nullptr };
				hr = CreateRandomAccessStreamOverStream(stream, BSOS_DEFAULT, __uuidof(rdmStream), to_void_pp(rdmStream));
				stream->Release();
				if (SUCCEEDED(hr))
				{
					auto* const instance{ new AppxPackagePayloadFileOpenAsyncOpWrapper{ rdmStream } };
					if (instance)
					{
						*operation = instance;
						return S_OK;
					}
					else
					{
						hr = E_OUTOFMEMORY;
						ABI::IClosable* closable{ nullptr };
						rdmStream->QueryInterface(__uuidof(closable), to_void_pp(closable));
						closable->Close();
						closable->Release();
						rdmStream->Release();
					}
				}
			}
			return hr;
		}
		else
		{ return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED); }
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::OpenTransactedWriteAsync(ABI::IAsyncOperation<ABI::StorageStreamTransaction*>** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::CopyOverloadDefaultNameAndOptions(ABI::IStorageFolder* destinationFolder, ABI::IAsyncOperation<ABI::StorageFile*>** operation)
	{
		if (destinationFolder)
		{
			HRESULT hr{ S_OK };
			HSTRING defaultName{ nullptr };
			hr = get_Name(&defaultName);
			if (SUCCEEDED(hr))
			{
				AppxPackagePayloadFileCopyAsyncOp* instance{ nullptr };
				hr = AppxPackagePayloadFileCopyAsyncOp::CreateInstance(this, destinationFolder, defaultName, ABI::NameCollisionOption_FailIfExists, instance);
				if (SUCCEEDED(hr))
				{
					hr = instance->LaunclAsyncTask();
					if (SUCCEEDED(hr))
					{ *operation = instance; }
					else
					{
						instance->ReleaseResourcesInternal();
						instance->Release();
					}
				}
				WindowsDeleteString(defaultName);
			}
			return hr;
		}
		else
		{ return E_INVALIDARG; }
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::CopyOverloadDefaultOptions(ABI::IStorageFolder* destinationFolder, HSTRING desiredNewName, ABI::IAsyncOperation<ABI::StorageFile*>** operation)
	{
		if (destinationFolder && desiredNewName)
		{
			HRESULT hr{ S_OK };
			AppxPackagePayloadFileCopyAsyncOp* instance{ nullptr };
			hr = AppxPackagePayloadFileCopyAsyncOp::CreateInstance(this, destinationFolder, desiredNewName, ABI::NameCollisionOption_FailIfExists, instance);
			if (SUCCEEDED(hr))
			{
				hr = instance->LaunclAsyncTask();
				if (SUCCEEDED(hr))
				{
					*operation = instance;
					return hr;
				}
				else
				{
					instance->ReleaseResourcesInternal();
					instance->Release();
				}
			}
			return hr;
		}
		else
		{ return E_INVALIDARG; }
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::CopyOverload(ABI::IStorageFolder* destinationFolder, HSTRING desiredNewName, ABI::NameCollisionOption option, ABI::IAsyncOperation<ABI::StorageFile*>** operation)
	{
		if (destinationFolder && desiredNewName)
		{
			HRESULT hr{ S_OK };
			AppxPackagePayloadFileCopyAsyncOp* instance{ nullptr };
			hr = AppxPackagePayloadFileCopyAsyncOp::CreateInstance(this, destinationFolder, desiredNewName, option, instance);
			if (SUCCEEDED(hr))
			{
				hr = instance->LaunclAsyncTask();
				if (SUCCEEDED(hr))
				{
					*operation = instance;
					return hr;
				}
				else
				{
					instance->ReleaseResourcesInternal();
					instance->Release();
				}
			}
			return hr;
		}
		else
		{ return E_INVALIDARG; }
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::CopyAndReplaceAsync(ABI::IStorageFile* fileToReplace, ABI::IAsyncAction** operation)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::MoveOverloadDefaultNameAndOptions(ABI::IStorageFolder* destinationFolder, ABI::IAsyncAction** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::MoveOverloadDefaultOptions(ABI::IStorageFolder* destinationFolder, HSTRING desiredNewName, ABI::IAsyncAction** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::MoveOverload(ABI::IStorageFolder* destinationFolder, HSTRING desiredNewName, ABI::NameCollisionOption option, ABI::IAsyncAction** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::MoveAndReplaceAsync(ABI::IStorageFile* fileToReplace, ABI::IAsyncAction** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}
#pragma endregion

#pragma region Windows.Storage.IStorageItem
	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::RenameAsyncOverloadDefaultOptions(HSTRING desiredName, ABI::IAsyncAction** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::RenameAsync(HSTRING desiredName, ABI::NameCollisionOption option, ABI::IAsyncAction** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::DeleteAsyncOverloadDefaultOptions(ABI::IAsyncAction** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::DeleteAsync(ABI::StorageDeleteOption option, ABI::IAsyncAction** operation)
	{
		return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::GetBasicPropertiesAsync(ABI::IAsyncOperation<ABI::FileProperties::BasicProperties*>** operation)
	{
		// Cannot construct BasicProperties, this method is unsupported.
		return CO_E_NOT_SUPPORTED;
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::get_Name(HSTRING* value)
	{
		HSTRING local{ reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Name), nullptr, nullptr)) };
		if (local == nullptr)
		{
			AcquireSRWLockExclusive(&m_Lock);
			HRESULT hr{ S_OK };
			if (!m_Name)
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
			ReleaseSRWLockExclusive(&m_Lock);
			if (FAILED(hr))
			{ return hr; }
			local = reinterpret_cast<HSTRING>(InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_Name), nullptr, nullptr));
		}

		return WindowsDuplicateString(local, value);
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::get_Path(HSTRING* value)
	{
		// AppX payload files has no absolute file system path, this property will return null.
		*value = nullptr;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::get_Attributes(ABI::FileAttributes* value)
	{
		*value = ABI::FileAttributes_ReadOnly;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::get_DateCreated(ABI::DateTime* value)
	{
		// IAppxFile interface has no DateCreated property, so return a placeholder timestamp.
		value->UniversalTime = 116444736000000000LL; // Jan 1, 1970 00:00:00
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::IsOfType(ABI::StorageItemTypes type, boolean* value)
	{
		*value = type == ABI::StorageItemTypes_File;
		return S_OK;
	}
#pragma endregion

#pragma region Windows.Storage.IStorageFilePropertiesWithAvalibility
	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::get_IsAvailable(boolean* value)
	{
		*value = true;
		return S_OK;
	}
#pragma endregion

#pragma region Windows.Storage.Streams.IInputStreamReference
	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::OpenSequentialReadAsync(ABI::IAsyncOperation<ABI::IInputStream*>** operation)
	{
		HRESULT hr{ S_OK };
		IStream* stream{ nullptr };
		hr = m_AppxPayloadFile->GetStream(&stream);
		if (SUCCEEDED(hr))
		{
			ABI::IRandomAccessStream* rdmStream{ nullptr };
			hr = CreateRandomAccessStreamOverStream(stream, BSOS_DEFAULT, __uuidof(rdmStream), to_void_pp(rdmStream));
			stream->Release();
			if (SUCCEEDED(hr))
			{
				ABI::IInputStream* inputStream{ nullptr };
				hr = rdmStream->GetInputStreamAt(0, &inputStream);
				ABI::IClosable* closable{ nullptr };
				rdmStream->QueryInterface(__uuidof(closable), to_void_pp(closable));
				closable->Close();
				closable->Release();
				rdmStream->Release();
				if (SUCCEEDED(hr))
				{
					auto* const instance{ new AppxPackagePayloadFileOpenSequentialReadAsyncOpWrapper(inputStream) };
					if (instance)
					{
						*operation = instance;
						return S_OK;
					}
					else
					{
						hr = E_OUTOFMEMORY;
						ABI::IClosable* closable{ nullptr };
						inputStream->QueryInterface(__uuidof(closable), to_void_pp(closable));
						closable->Close();
						closable->Release();
						inputStream->Release();
					}
				}
			}
		}
		return hr;
	}
#pragma endregion

#pragma region Windows.Storage.Streams.IRandomAccessStreamReference
	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::OpenReadAsync(ABI::IAsyncOperation<ABI::IRandomAccessStreamWithContentType*>** operation)
	{
		HRESULT hr{ S_OK };
		IStream* stream{ nullptr };
		hr = m_AppxPayloadFile->GetStream(&stream);
		if (SUCCEEDED(hr))
		{
			ABI::IRandomAccessStream* rdmStream{ nullptr };
			hr = CreateRandomAccessStreamOverStream(stream, BSOS_DEFAULT, __uuidof(rdmStream), to_void_pp(rdmStream));
			stream->Release();
			if (SUCCEEDED(hr))
			{
				HSTRING contentType{ nullptr };
				hr = get_ContentType(&contentType);
				if (SUCCEEDED(hr))
				{
					auto* const instance{ new AppxPackagePayloadFileOpenReadAsyncRandomAccessStreamWithContentType{ rdmStream, contentType } };
					if (instance)
					{
						auto* const operationInst{ new AppxPackagePayloadFileOpenReadAsyncOpWrapper{ instance } };
						if (operationInst)
						{
							*operation = operationInst;
							return S_OK;
						}
						else
						{
							instance->Close();
							instance->Release();
							return E_OUTOFMEMORY;
						}
					}
					else
					{
						hr = E_OUTOFMEMORY;
						WindowsDeleteString(contentType);
					}
				}
				if (FAILED(hr))
				{
					ABI::IClosable* closable{ nullptr };
					rdmStream->QueryInterface(__uuidof(closable), to_void_pp(closable));
					closable->Close();
					closable->Release();
					rdmStream->Release();
				}
			}
		}
		return hr;
	}
#pragma endregion

#pragma region IAppxPackagePayloadFileInterop
	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::get_AppxPayloadFile(IAppxFile** value)
	{
		m_AppxPayloadFile->AddRef();
		*value = m_AppxPayloadFile;
		return S_OK;
	}
#pragma endregion

#pragma region IInspectable
	HRESULT STDMETHODCALLTYPE AppxPackagePayloadFileBase::GetRuntimeClassName(HSTRING* className)
	{ return WindowsCreateString(L"AppxUtils.AppxPackagePayloadFile", 32, className); }
#pragma endregion

	//Destructor
	AppxPackagePayloadFileBase::~AppxPackagePayloadFileBase() noexcept
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
	}
#pragma endregion

#pragma region AppxPackagePayloadFileDedicated
#pragma region IUnknown
	ULONG STDMETHODCALLTYPE AppxPackagePayloadFileDedicated::Release()
	{
		const ULONG cRef{ InterlockedDecrement(&m_RefCount) };
		if (!cRef)
		{ delete this; }
		return cRef;
	}
#pragma endregion
#pragma endregion

#pragma region AppxPackagePayloadFile
#pragma region IUnknown
	ULONG STDMETHODCALLTYPE AppxPackagePayloadFile::Release()
	{
		const ULONG cRef{ InterlockedDecrement(&m_RefCount) };
		if (!cRef)
		{ this->~AppxPackagePayloadFile(); }
		return cRef;
	}
#pragma endregion
#pragma endregion
}
