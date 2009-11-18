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
 * Filename:    Directory.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This file defines the Directory and ListEntry classes.
 */
#include "stdafx.h"
#include "Directory.h"

CListEntry::CListEntry(WIN32_FIND_DATA& data)
{
	memcpy((LPVOID) &m_Data, (LPVOID) &data, sizeof(WIN32_FIND_DATA));
}
CListEntry::~CListEntry()
{
}

STDMETHODIMP CListEntry::get_IsArchive(BOOL *b)
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsCompressed(BOOL *b)
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsDirectory(BOOL *b)
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsEncrypted(BOOL *b)
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsHidden(BOOL *b)
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsNormal(BOOL *b)
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsOffline(BOOL *b)
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_OFFLINE;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsReadOnly(BOOL *b)
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_READONLY;
	return S_OK;
}
STDMETHODIMP CListEntry::get_HasReparsePoint(BOOL *b)
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsSparseFile(BOOL *b)
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsSystem(BOOL *b)
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsTemporary(BOOL *b)
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY;
	return S_OK;
}

STDMETHODIMP CListEntry::get_CreationTime(VARIANT *vCreationTime)
{
	SYSTEMTIME st;
	DATE vtime;

	::FileTimeToSystemTime(&m_Data.ftCreationTime, &st);
	::SystemTimeToVariantTime(&st, &vtime);
	*vCreationTime = _variant_t(vtime).Detach();

	return S_OK;
}
STDMETHODIMP CListEntry::get_LastAccessTime(VARIANT *vLastAccessTime)
{
	SYSTEMTIME st;
	DATE vtime;

	::FileTimeToSystemTime(&m_Data.ftLastAccessTime, &st);
	::SystemTimeToVariantTime(&st, &vtime);
	*vLastAccessTime = _variant_t(vtime).Detach();

	return S_OK;
}
STDMETHODIMP CListEntry::get_LastWriteTime(VARIANT *vLastWriteTime)
{
	SYSTEMTIME st;
	DATE vtime;

	::FileTimeToSystemTime(&m_Data.ftLastWriteTime, &st);
	::SystemTimeToVariantTime(&st, &vtime);
	*vLastWriteTime = _variant_t(vtime).Detach();

	return S_OK;
}

STDMETHODIMP CListEntry::get_FileSizeHigh(DWORD *dw)
{
	*dw = m_Data.nFileSizeHigh;
	return S_OK;
}
STDMETHODIMP CListEntry::get_FileSizeLow(DWORD *dw)
{
	*dw = m_Data.nFileSizeLow;
	return S_OK;
}

STDMETHODIMP CListEntry::get_ReparseTag(DWORD *dw)
{
	*dw = m_Data.dwReserved0;
	return S_OK;
}
STDMETHODIMP CListEntry::get_Reserved1(DWORD *dw)
{
	*dw = m_Data.dwReserved1;
	return S_OK;
}

STDMETHODIMP CListEntry::get_Name(BSTR *str)
{
	*str = ::SysAllocString(m_Data.cFileName);
	return S_OK;
}
STDMETHODIMP CListEntry::get_AlternateName(BSTR *str)
{
	*str = ::SysAllocString(m_Data.cAlternateFileName);
	return S_OK;
}

CDirectory::CDirectory()
{
}
CDirectory::~CDirectory()
{
}
STDMETHODIMP CDirectory::Create(BSTR path)
{
	TCHAR cleanPath[MAX_PATH];
	::PathCanonicalize(cleanPath, path);
	::PathRemoveBackslash(cleanPath);

	if(!::PathIsDirectory(cleanPath) && !::PathIsRoot(cleanPath))
	{
		TCHAR parentPath[MAX_PATH];
		::StrCpy(parentPath, cleanPath);
		::PathRemoveBackslash(parentPath);
		::PathRemoveFileSpec(parentPath);

		if(!::PathIsDirectory(parentPath) && !::PathIsRoot(parentPath))
		{
			Create(parentPath);
		}

		if(::CreateDirectory(cleanPath, NULL))
		{
			return S_OK;
		}
		else
		{
			return ::GetLastError();
		}
	}
	else
	{
		return ERROR_ALREADY_EXISTS;
	}
}
STDMETHODIMP CDirectory::Exists(BSTR path, BOOL *bExists)
{
	TCHAR searchPath[MAX_PATH];
	::StringCchCopy(searchPath, MAX_PATH, path);
	::PathAddBackslash(searchPath);
	::PathAppend(searchPath, L"*");

	WIN32_FIND_DATAW FindFileData;
	HANDLE hFind = ::FindFirstFile(searchPath, &FindFileData);

	if(hFind != INVALID_HANDLE_VALUE)
	{
		::FindClose(hFind);
		*bExists = TRUE;
	}
	else
	{
		*bExists = FALSE;
	}
    return S_OK;
}
STDMETHODIMP CDirectory::ListDirs(BSTR path, VARIANT *list)
{
	return List(path, 2, list);
}
STDMETHODIMP CDirectory::ListFiles(BSTR path, VARIANT *list)
{
	return List(path, 1, list);
}
STDMETHODIMP CDirectory::Delete(BSTR path)
{
	Clear(path);
	return S_OK;
}
void CDirectory::Clear(BSTR path)
{
	VARIANT vList;
	List(path, 0, &vList);

	long upperBound;
	::SafeArrayGetUBound(vList.parray, 1, &upperBound);

	for(long i = 0; i < upperBound; i++)
	{
		VARIANT vListEntry;
		::SafeArrayGetElement(vList.parray, &i, &vListEntry);

		IListEntry* listEntry;
		vListEntry.pdispVal->QueryInterface(__uuidof(listEntry),
			(LPVOID*) &listEntry);

		BSTR name;
		listEntry->get_Name(&name);

		TCHAR newPath[MAX_PATH];
		::StringCchCopy(newPath, MAX_PATH, path);
		::PathAppend(newPath, name);
		::SysFreeString(name);

		BOOL bDir;
		listEntry->get_IsDirectory(&bDir);

		if(bDir)
		{
			Clear(newPath);
		}
		else
		{
			::DeleteFile(newPath);
		}

		listEntry->Release();
	}
	::SafeArrayDestroy(vList.parray);
	::RemoveDirectory(path);
}

/* dwFlag: 0 -> all
 * dwFlag: 1 -> files only
 * dwFlag: 2 -> dirs only
 */
STDMETHODIMP CDirectory::List(BSTR path, DWORD dwFlag, VARIANT *list)
{
	WCHAR searchPath[MAX_PATH];
	::StringCchCopyW(searchPath, sizeof(searchPath), path);
	::PathAddBackslashW(searchPath);
	::PathAppend(searchPath, L"*");

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(searchPath, &FindFileData);

	std::vector<VARIANT> results;

	if(hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(wcscmp(FindFileData.cFileName, L"..") &&
				wcscmp(FindFileData.cFileName, L".") &&
				(dwFlag == 0 || 
				(dwFlag == 1 &&	
				!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) ||
				(dwFlag == 2 && 
				(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))))
			{
				VARIANT v;
				v.vt = VT_DISPATCH;
				(new CListEntry(FindFileData))->QueryInterface(
					IID_IDispatch, (LPVOID*) &v.pdispVal);
				results.push_back(v);
			}
		}
		while(FindNextFile(hFind, &FindFileData));
	}
	FindClose(hFind);

	SAFEARRAYBOUND resultsArrayBound;
	resultsArrayBound.cElements = results.size();
	resultsArrayBound.lLbound = 0;
	SAFEARRAY* resultsArray = ::SafeArrayCreate(VT_VARIANT, 1, 
		&resultsArrayBound);

	LONG index = 0;
	for(std::vector<VARIANT>::iterator it = results.begin();
		it != results.end();
		it++)
	{
		::SafeArrayPutElement(resultsArray, &index, &(*it));
		it->pdispVal->Release();
		index++;
	}

	list->parray = resultsArray;
	list->vt = VT_ARRAY | VT_VARIANT;
	return S_OK;
}

int CDirectory::FindLastSlash(LPWSTR path)
{
	for(int i = ::wcslen(path); i != 0; i--)
		if(path[i] == L'\\' || path[i] == L'/')
			return i;
	return -1;
}