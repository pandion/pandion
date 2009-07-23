/**
 * This file is part of Pandion.
 *
 * Pandion is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pandion is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Pandion.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Filename:    File.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "File.h"
#include "UTF8.h"
#include "Directory.h"
#include "Hash.h"

/*
 * CFile constructor
 */
CFile::CFile() : m_FileHandle(INVALID_HANDLE_VALUE), m_FileName(L""),
	m_HasWriteAccess(false), m_AtEnd(false),
	m_DataBuffer(NULL), m_CurrentPosition(NULL)
{ 
}

/*
 * CFile destructor
 */
CFile::~CFile()
{
	Close();
}

/*
 * Attempts to upgrade the file access rights to write access
 */
HRESULT CFile::GetWriteAccess()
{
	if(m_FileHandle == INVALID_HANDLE_VALUE)
	{
		return ERROR_INVALID_HANDLE;
	}
	else if(m_HasWriteAccess)
	{
		return S_OK;
	}
	else
	{
		DWORD filePointer = SetFilePointer(m_FileHandle, 0, 0, FILE_CURRENT);
		_bstr_t fileName = m_FileName;
		Close();

		if(Create(fileName, GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ, OPEN_EXISTING) == S_OK)
		{
			Seek(filePointer, 0, FILE_BEGIN);
			return S_OK;
		}
		else
		{
			Create(m_FileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
			Seek(filePointer, 0, FILE_BEGIN);
			return STG_E_SHAREVIOLATION;
		}		
	}
}

/*
 * Creates a new empty file on the specified location.
 */
STDMETHODIMP CFile::Create(BSTR strFileName,
	unsigned dwAccess, unsigned dwShare, unsigned dwDisposition)
{
	if(::SysStringLen(strFileName)> MAX_PATH)
	{
		return E_FAIL;
	}
	else
	{
		if(m_FileHandle != INVALID_HANDLE_VALUE)
		{
			Close();
		}
		if(strFileName == _bstr_t(L"?temp"))
		{
			m_FileName = getTempFileName();
		}
		else
		{
			m_FileName = strFileName;
			fixSlashes(m_FileName);
			createFileDirectory(m_FileName);
		}

		m_FileHandle = ::CreateFile(CW2T(strFileName),
			dwAccess, dwShare, NULL, 
			dwDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
		Seek(0, 0, FILE_BEGIN);

		if(m_FileHandle != INVALID_HANDLE_VALUE)
		{
			if(dwAccess & GENERIC_WRITE)
			{
				m_HasWriteAccess = true;
			}
			else
			{
				m_HasWriteAccess = false;
			}
			return S_OK;
		}
		else
		{
			m_FileName = L"";
			return E_FAIL;
		}
	}
}

/*
 * Seeks to a given position in the file.
 */
STDMETHODIMP CFile::Seek(int dwOffsetLow, int dwOffsetHigh, unsigned dwMethod)
{
	if(m_FileHandle == INVALID_HANDLE_VALUE)
	{
        return ERROR_INVALID_HANDLE;
	}
	else
	{
		unsigned err = 0;

		if(dwOffsetHigh)
		{
			err = ::SetFilePointer(m_FileHandle,
				dwOffsetLow, (long*)&dwOffsetHigh, dwMethod);
		}
		else
		{
			err = ::SetFilePointer(m_FileHandle, dwOffsetLow, 0, dwMethod);
		}

		if(err == INVALID_SET_FILE_POINTER)
		{
			return S_FALSE;
		}
		else
		{
			return S_OK;
		}
	}
}

/*
 * Ends the file at the current position of the File Pointer.
 */
STDMETHODIMP CFile::SetEOF()
{
 	if(FAILED(GetWriteAccess()))
	{
        return S_FALSE;
	}
	else
	{
		::SetEndOfFile(m_FileHandle);
		return S_OK;
	}
}

/*
 * Reads data from the file, starting at the current position of the File
 * Pointer. This method moves the File Pointer.
 */
STDMETHODIMP CFile::Read(BYTE* pBuffer, unsigned nRead)
{
	DWORD dwRead;

	if(!::ReadFile(m_FileHandle, pBuffer, nRead, &dwRead, NULL))
	{
		return E_FAIL;
	}
	else if(dwRead == 0)
	{
		return E_FAIL;
	}
	else
	{
		return S_OK;
	}
}

/*
 * Writes data to the file, starting at the current position of the File
 * Pointer. This method moves the File Pointer.
 */
STDMETHODIMP CFile::Write(BYTE* pBuffer, unsigned nWrite)
{
	DWORD dwWritten;

	if(FAILED(GetWriteAccess()))
	{
        return E_FAIL;
	}
	else if(!::WriteFile(m_FileHandle, pBuffer, nWrite, &dwWritten, NULL))
	{
		return E_FAIL;
	}
	else if(dwWritten == 0)
	{
		return E_FAIL;
	}
	else
	{
		return S_OK;
	}
}

/*
 * Read a \n or \r\n delimited line from the file. UTF-8 encoding is assumed. 
 */
STDMETHODIMP CFile::ReadLine(BSTR* strLine)
{
	if(m_DataBuffer == NULL)
	{
		unsigned uFileSize;
		get_Size(&uFileSize);
		m_DataBuffer = new char[uFileSize];
		Read((BYTE *) m_DataBuffer,uFileSize);
		m_CurrentPosition = m_DataBuffer;
	}

	LPSTR tokenPosition = strchr(m_CurrentPosition, '\n');

	if(tokenPosition != NULL)
	{
		*tokenPosition = '\0';
		if(strlen(m_CurrentPosition)> 0 && *(tokenPosition - 1) == '\r')
		{
			*(tokenPosition - 1) = '\0';
		}
		*strLine = SysAllocString(CUTF82WEX<1024>(m_CurrentPosition));
		m_CurrentPosition = tokenPosition + 1;
	}
	else
	{
		m_AtEnd = true;
		*strLine = SysAllocString(CUTF82WEX<1024>(m_CurrentPosition));
	}
	
	SetFilePointer(m_FileHandle, (long)(m_CurrentPosition - m_DataBuffer),
		0, FILE_BEGIN);
	return S_OK;
}

/*
 * Write a \r\n delimited UTF-8 encoded line to a file.
 */
STDMETHODIMP CFile::WriteLine(BSTR strLine)
{
	if(SUCCEEDED(GetWriteAccess()))
	{
		BYTE* newLine = (BYTE*) "\r\n";

		std::string strUTF8 = std::string(CW2UTF8(strLine));

		Seek(0, 0, FILE_END);
		Write((BYTE*) strUTF8.c_str(), strUTF8.length());
		Write(newLine, 2);

		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

/*
 * Read an entire file into a Base64 encode string.
 */
STDMETHODIMP CFile::ReadBase64(BSTR* strBase64)
{
	if(m_FileHandle != INVALID_HANDLE_VALUE)
	{
		DWORD fileSize = ::GetFileSize(m_FileHandle, 0);
		std::vector<BYTE> fileBuffer(fileSize);

		int b64Size = ATL::Base64EncodeGetRequiredLength(fileSize);
		std::string b64Buffer(b64Size + 1, '\0');

		if(SUCCEEDED(Read(&fileBuffer[0], fileSize)) &&
			ATL::Base64Encode(&fileBuffer[0], fileSize, &b64Buffer[0], &b64Size))
		{
			*strBase64 = ::SysAllocString(CA2W(b64Buffer.c_str()));
			return S_OK;
		}
		else
		{
			return E_FAIL;
		}
	}
	else
	{
		return E_FAIL;
	}
}

/*
 * Decode a Base64-encoded string and write it to the file.
 */
STDMETHODIMP CFile::WriteBase64(BSTR strBase64)
{
	if(SUCCEEDED(GetWriteAccess()))
	{
		size_t b64Size = ::SysStringLen(strBase64);
		std::string b64Buffer = std::string(CW2AEX<0x8000>(strBase64));

		int fileBufferSize = ATL::Base64DecodeGetRequiredLength(b64Size);
		std::vector<BYTE> fileBuffer(fileBufferSize);

		ATL::Base64Decode(b64Buffer.c_str(), b64Size, &fileBuffer[0], &fileBufferSize);
		Write(&fileBuffer[0], fileBufferSize);

		return S_OK;
	}
	else
	{
        return E_FAIL;
	}
}

/*
 * Sets the parameter true when the we're at the end of the file, false
 * otherwise.
 */
STDMETHODIMP CFile::get_AtEnd(BOOL* bAtEnd)
{
	unsigned fileSize;
	if(m_FileHandle != INVALID_HANDLE_VALUE && SUCCEEDED(get_Size(&fileSize)))
	{
		if(m_AtEnd)
		{
			*bAtEnd = true;
		}
		else if(fileSize == SetFilePointer(m_FileHandle, 0, 0, FILE_CURRENT))
		{
			*bAtEnd = true;
		}
		else
		{
			*bAtEnd = false;
		}
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

/*
 * Sets the paramter to the size of the file in bytes.
 */
STDMETHODIMP CFile::get_Size(unsigned* nSize)
{
	if(m_FileHandle != INVALID_HANDLE_VALUE)
	{
		*nSize = ::GetFileSize(m_FileHandle, 0);
		return S_OK;
	}
	else
	{
		*nSize = 0;
		return E_FAIL;
	}
}

/*
 * Sets the parameter to the name of the file.
 */
STDMETHODIMP CFile::get_FileName(BSTR* strFileName)
{
	*strFileName = m_FileName.copy();
	return S_OK;
}

/*
 * Sets the parameter to the SHA1 hash of the file.
 */
STDMETHODIMP CFile::get_SHA1(BSTR* strSHA1)
{
	unsigned fileSize;
	if(m_FileHandle != INVALID_HANDLE_VALUE && SUCCEEDED(get_Size(&fileSize)))
	{
		std::vector<BYTE> fileBuffer(fileSize);
		if(SUCCEEDED(Read(&fileBuffer[0], fileSize)))
		{
			unsigned char digest[20];
			Hash::SHA1(&fileBuffer[0], fileSize, digest);

			wchar_t hexHash[20*2+1];
			hexHash[20*2] = 0;
			for(int i = 0; i <20; i++)
			{
				hexHash[2*i] = (digest[i]>> 4) + 
					(((digest[i]>> 4) <0xA) ? L'0' : (L'a' - 0xA));
				hexHash[2*i+1] = (digest[i] & 0x0F) + 
					(((digest[i] & 0x0F) <0xA) ? L'0' : (L'a' - 0xA));
			}
			*strSHA1 = ::SysAllocString(hexHash);
			return S_OK;
		}
		else
		{
			return E_FAIL;
		}
	}
	else
	{
		return E_FAIL;
	}
}

/*
 * Moves the currently opened file to the specified location.
 */
STDMETHODIMP CFile::Move(BSTR strTo)
{
	if(m_FileHandle != INVALID_HANDLE_VALUE && 
		::SysStringLen(strTo) <= MAX_PATH)
	{
		_bstr_t strFrom = m_FileName;
		Close();

		createFileDirectory(_bstr_t(strTo));

		if(::MoveFile(CW2TEX<MAX_PATH>(strFrom), CW2TEX<MAX_PATH>(strTo)))
		{
			Create(strTo, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
				OPEN_EXISTING);
			return S_OK;
		}
		else
		{
			Create(strFrom, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 
				OPEN_EXISTING);
			return E_FAIL;
		}
	}
	else
	{
		return E_FAIL;
	}
}

/*
 * Writes a copy of the file to the specified location.
 */
STDMETHODIMP CFile::Copy(BSTR strTo, BOOL bOverWrite, IPdnFile** retCopy)
{
	createFileDirectory(_bstr_t(strTo));
	if(m_FileHandle != INVALID_HANDLE_VALUE && ::SysStringLen(strTo) <= 
		MAX_PATH)
	{
		_bstr_t strFrom = m_FileName;
		if(::CopyFile(CW2TEX<MAX_PATH>(strFrom), CW2TEX<MAX_PATH>(strTo), 
			!bOverWrite))
		{
			(new CComObject<CFile>)->QueryInterface(retCopy);
			(*retCopy)->Create(strTo, GENERIC_READ, 
				FILE_SHARE_READ|FILE_SHARE_WRITE, OPEN_EXISTING);
			return S_OK;
		}
		else
		{
			return E_FAIL;
		}

	}
	else
	{
		return E_FAIL;
	}
}

/*
 * Deletes the currently opened file.
 */
STDMETHODIMP CFile::Delete()
{
	if(SUCCEEDED(GetWriteAccess()))
	{
		_bstr_t FileName = m_FileName;
		Close();
		::DeleteFile(CW2TEX<MAX_PATH>(FileName));
		return S_OK;
	}
	else
	{
        return E_FAIL;
	}
}

/*
 * Empties the Windows write buffers.
 */
STDMETHODIMP CFile::Flush()
{
	::FlushFileBuffers(m_FileHandle);
	return S_OK;
}

/*
 * Closes the file and resets the object to it's original state.
 */
STDMETHODIMP CFile::Close()
{
	if(m_FileHandle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_FileHandle);
	}
	if(m_DataBuffer)
	{
		delete m_DataBuffer;
		m_DataBuffer = NULL;
		m_CurrentPosition = NULL;
	}

	m_FileHandle = INVALID_HANDLE_VALUE;
	m_FileName = "";
	m_HasWriteAccess = FALSE;
	m_AtEnd = FALSE;

	return S_OK;
}

/*
 * Retrieves the last time a write operation was performed on the file.
 */
STDMETHODIMP CFile::get_LastWriteTime(VARIANT* vLastWriteTime)
{
	FILETIME lastWriteTime;
	SYSTEMTIME sysTime;
	DATE vtime;

	if(m_FileHandle != INVALID_HANDLE_VALUE &&
		::GetFileTime(m_FileHandle, NULL, NULL,	&lastWriteTime) != 0)
	{
		::FileTimeToSystemTime(&lastWriteTime, &sysTime);
		::SystemTimeToVariantTime(&sysTime, &vtime);

		*vLastWriteTime = _variant_t(vtime).Detach();
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

/*
 * This method replaces forward slashes with backward slashes in the
 * specified string.
 */
void CFile::fixSlashes(_bstr_t &str)
{
	for(register unsigned int i = 0; i <str.length(); i++)
	{
		if(str.GetBSTR()[i] == '/')
		{
			str.GetBSTR()[i] = '\\';
		}
	}
}

/*
 * This method generates a filename for a temporary file.
 */
_bstr_t CFile::getTempFileName()
{
	TCHAR NameBuffer[MAX_PATH + 1];
	TCHAR TempPathBuffer[MAX_PATH + 1];
	GetTempPath(MAX_PATH + 1, TempPathBuffer);
	GetTempFileName(TempPathBuffer, TEXT("PDN"), 0, NameBuffer);
	return _bstr_t(NameBuffer);
}

/*
 * This method builds the directories for a given path.
 */
void CFile::createFileDirectory(_bstr_t &str)
{
	WCHAR strDirNameW[MAX_PATH];
	StringCchCopyW(strDirNameW, MAX_PATH, str);
	PathRemoveFileSpecW(strDirNameW);

	IPdnDirectory *pDir;
	(new CDirectory())->QueryInterface(IID_IPdnDirectory, (LPVOID*) &pDir);
	pDir->Create(strDirNameW);
	pDir->Release();
}