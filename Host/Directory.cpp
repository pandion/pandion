#include "stdafx.h"
#include "Directory.h"

#undef FindFirstFile
HANDLE FindFirstFile( LPWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData )
{
	if( ( GetVersion() & 0x80000000 ) )
	{
		WIN32_FIND_DATAA wfd;
		HANDLE h = FindFirstFileA( CW2A( lpFileName ), &wfd );
		StringCchCopyW( lpFindFileData->cAlternateFileName, 14, CA2W(wfd.cAlternateFileName) );
		StringCchCopyW( lpFindFileData->cFileName, 260, CA2W(wfd.cFileName) );
		lpFindFileData->dwFileAttributes = wfd.dwFileAttributes;
		lpFindFileData->dwReserved0 = wfd.dwReserved0;
		lpFindFileData->dwReserved1 = wfd.dwReserved1;
		lpFindFileData->ftCreationTime = wfd.ftCreationTime;
		lpFindFileData->ftLastAccessTime = wfd.ftLastAccessTime;
		lpFindFileData->ftLastWriteTime = wfd.ftLastWriteTime;
		lpFindFileData->nFileSizeHigh = wfd.nFileSizeHigh;
		lpFindFileData->nFileSizeLow = wfd.nFileSizeLow;
		return h;
	}
	else
		return FindFirstFileW( lpFileName, lpFindFileData );
}
#undef FindNextFile
BOOL FindNextFile( HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData )
{
	if( ( GetVersion() & 0x80000000 ) )
	{
		WIN32_FIND_DATAA wfd;
		BOOL b = FindNextFileA( hFindFile, &wfd );
		StringCchCopyW( lpFindFileData->cAlternateFileName, 14, CA2W(wfd.cAlternateFileName) );
		StringCchCopyW( lpFindFileData->cFileName, 260, CA2W(wfd.cFileName) );
		lpFindFileData->dwFileAttributes = wfd.dwFileAttributes;
		lpFindFileData->dwReserved0 = wfd.dwReserved0;
		lpFindFileData->dwReserved1 = wfd.dwReserved1;
		lpFindFileData->ftCreationTime = wfd.ftCreationTime;
		lpFindFileData->ftLastAccessTime = wfd.ftLastAccessTime;
		lpFindFileData->ftLastWriteTime = wfd.ftLastWriteTime;
		lpFindFileData->nFileSizeHigh = wfd.nFileSizeHigh;
		lpFindFileData->nFileSizeLow = wfd.nFileSizeLow;
		return b;
	}
	else
		return FindNextFileW( hFindFile, lpFindFileData );
}
#undef CreateDirectory
BOOL CreateDirectory( LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes )
{
	if( ( GetVersion() & 0x80000000 ) )
		return CreateDirectoryA( CW2A( lpPathName ), lpSecurityAttributes );
	else
		return CreateDirectoryW( lpPathName, lpSecurityAttributes );
}
#undef GetCurrentDirectory
DWORD GetCurrentDirectory( DWORD nBufferLength, LPWSTR lpBuffer )
{
	if( ( GetVersion() & 0x80000000 ) )
	{
		LPSTR lpBufferA = new char[nBufferLength];
		DWORD dw = GetCurrentDirectoryA( nBufferLength, lpBufferA );
		StringCchCopyW( lpBuffer, nBufferLength, CA2W( lpBufferA ) );
		delete lpBufferA;
		return dw;
	}
	else
		return GetCurrentDirectoryW( nBufferLength, lpBuffer );
}
#undef SetCurrentDirectory
BOOL SetCurrentDirectory( LPCWSTR lpPathName )
{
	if( ( GetVersion() & 0x80000000 ) )
		return SetCurrentDirectoryA( CW2A( lpPathName ) );
	else
		return SetCurrentDirectoryW( lpPathName );
}

CListEntry::CListEntry()
{
}
CListEntry::~CListEntry()
{
}

void CListEntry::SetData( WIN32_FIND_DATAW *pData )
{
	memcpy( (void *) &m_Data, (void *) pData, sizeof(WIN32_FIND_DATAW) );
}

STDMETHODIMP CListEntry::get_IsArchive( BOOL *b )
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsCompressed( BOOL *b )
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsDirectory( BOOL *b )
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsEncrypted( BOOL *b )
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsHidden( BOOL *b )
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsNormal( BOOL *b )
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsOffline( BOOL *b )
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_OFFLINE;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsReadOnly( BOOL *b )
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_READONLY;
	return S_OK;
}
STDMETHODIMP CListEntry::get_HasReparsePoint( BOOL *b )
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsSparseFile( BOOL *b )
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsSystem( BOOL *b )
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM;
	return S_OK;
}
STDMETHODIMP CListEntry::get_IsTemporary( BOOL *b )
{
	*b = m_Data.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY;
	return S_OK;
}

STDMETHODIMP CListEntry::get_CreationTime( BSTR *str )
{
	SYSTEMTIME st;
	FileTimeToSystemTime( &m_Data.ftCreationTime, &st );
	WCHAR Buffer[8192];
	StringCbPrintfW( Buffer, 8192, L"%u-%u-%u %u:%u:%u:%u", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );
    *str = SysAllocString( Buffer );
	return S_OK;
}
STDMETHODIMP CListEntry::get_LastAccessTime( BSTR *str )
{
	SYSTEMTIME st;
	FileTimeToSystemTime( &m_Data.ftCreationTime, &st );
	WCHAR Buffer[8192];
	StringCbPrintfW( Buffer, 8192, L"%u-%u-%u %u:%u:%u:%u", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );
    *str = SysAllocString( Buffer );
	return S_OK;
}
STDMETHODIMP CListEntry::get_LastWriteTime( BSTR *str )
{
	SYSTEMTIME st;
	FileTimeToSystemTime( &m_Data.ftCreationTime, &st );
	WCHAR Buffer[8192];
	StringCbPrintfW( Buffer, 8192, L"%u-%u-%u %u:%u:%u:%u", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );
    *str = SysAllocString( Buffer );
	return S_OK;
}

STDMETHODIMP CListEntry::get_FileSizeHigh( DWORD *dw )
{
	*dw = m_Data.nFileSizeHigh;
	return S_OK;
}
STDMETHODIMP CListEntry::get_FileSizeLow( DWORD *dw )
{
	*dw = m_Data.nFileSizeLow;
	return S_OK;
}

STDMETHODIMP CListEntry::get_ReparseTag( DWORD *dw )
{
	*dw = m_Data.dwReserved0;
	return S_OK;
}
STDMETHODIMP CListEntry::get_Reserved1( DWORD *dw )
{
	*dw = m_Data.dwReserved1;
	return S_OK;
}

STDMETHODIMP CListEntry::get_Name( BSTR *str )
{
	*str = SysAllocString( m_Data.cFileName );
	return S_OK;
}
STDMETHODIMP CListEntry::get_AlternateName( BSTR *str )
{
	*str = SysAllocString( m_Data.cAlternateFileName );
	return S_OK;
}

CDirectory::CDirectory()
{
}
CDirectory::~CDirectory()
{
}
/* dwFlag: 0 -> all
 * dwFlag: 1 -> files only
 * dwFlag: 2 -> dirs only
 */
STDMETHODIMP CDirectory::List( BSTR path, DWORD dwFlag, VARIANT *list )
{
	WCHAR Buffer[MAX_PATH];
	StringCchCopyW( Buffer, MAX_PATH, path );
	PathAddBackslashW( Buffer );
	Buffer[wcslen(Buffer)+1] = 0;
	Buffer[wcslen(Buffer)] = '*';

	WIN32_FIND_DATAW FindFileData;
	HANDLE hFind = FindFirstFile( Buffer, &FindFileData );

	CComSafeArray<VARIANT> array;
	array.Create();

	if( hFind != INVALID_HANDLE_VALUE )
	{
		do
		{
			if( wcscmp( FindFileData.cFileName, L".." ) && wcscmp( FindFileData.cFileName, L"." ) )
			{
				if( dwFlag == 0 || 
					( dwFlag == 1 && ! ( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ) ||
					( dwFlag == 2 && ( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ) )
				{
					CListEntry *pLE = (CListEntry *) new CComObject<CListEntry>;
					pLE->SetData( &FindFileData );
					array.Add( CComVariant( (IDispatch *) pLE ) );
				}
			}
		}
		while( FindNextFile( hFind, &FindFileData ) );
	}
	FindClose( hFind );

	list->parray = array.Detach();
	list->vt = VT_ARRAY | VT_VARIANT;
	return S_OK;
}
int CDirectory::FindSlash( BSTR path )
{
	for( int i = wcslen( path ); i != 0; i-- )
		if( path[i] == '\\' || path[i] == '/' )
			return i;
	return -1;
}
STDMETHODIMP CDirectory::Create( BSTR path )
{
	WCHAR CleanPath[MAX_PATH];
	PathCanonicalizeW( CleanPath, path );
	PathRemoveBackslashW( CleanPath );

	BOOL bAlreadyExists;
	Exists( CleanPath, &bAlreadyExists );

	if( !bAlreadyExists )
	{
		WCHAR Parent[MAX_PATH];
		StringCchCopyW( Parent, MAX_PATH, CleanPath );
		Parent[FindSlash(CleanPath)] = 0;

		Exists( Parent, &bAlreadyExists );
		if( !bAlreadyExists )
		{
			return Create( Parent );
		}
		else
		{
			if( CreateDirectory( CleanPath, NULL ) )
				return S_OK;
			else
				return GetLastError();
		}
	}
	else
	{
		return ERROR_ALREADY_EXISTS;
	}

/*	int len = FindSlash( path );
	if( len != -1 && path[0] != '\\' && wcslen( path ) != len )
	{
		WCHAR ThisComponent[MAX_PATH];
		memcpy( ThisComponent, path, len * sizeof(WCHAR) );
		ThisComponent[len] = 0;

		BOOL bAlreadyExists;
		Exists( ThisComponent, &bAlreadyExists );

		if( !bAlreadyExists )
		{
			if( CreateDirectory( ThisComponent, NULL ) )
			{
				SetCurrentDirectory( ThisComponent );
				HRESULT retVal = Create( path + len );
				SetCurrentDirectory( CurrentDir );
				return retVal;
			}
			else
				return E_FAIL;
		}
		else
		{
			SetCurrentDirectory( ThisComponent );
			HRESULT retVal = Create( path + len );
			SetCurrentDirectory( CurrentDir );
			return retVal;
		}
	}
	else
	{
		BOOL bAlreadyExists;
		Exists( path, &bAlreadyExists );
		if( !bAlreadyExists )
		{
			if( CreateDirectory( path, NULL ) )
				return S_OK;
			else
				return E_FAIL;
		}
		else
			return E_FAIL;
	}*/
}
STDMETHODIMP CDirectory::Exists( BSTR path, BOOL *bExists )
{
	WCHAR searchPath[MAX_PATH];
	StringCchCopyW( searchPath, MAX_PATH, path );
	PathAddBackslashW( searchPath );
	int len = wcslen( searchPath );
	searchPath[len] = '*';
	searchPath[len+1] = 0;

	WIN32_FIND_DATAW FindFileData;
	HANDLE hFind = FindFirstFile( searchPath, &FindFileData );

	if( hFind != INVALID_HANDLE_VALUE )
	{
		FindClose( hFind );
		*bExists = TRUE;
	}
	else
		*bExists = FALSE;
    return S_OK;
}
STDMETHODIMP CDirectory::ListDirs( BSTR path, VARIANT *list )
{
	return List( path, 2, list );
}
STDMETHODIMP CDirectory::ListFiles( BSTR path, VARIANT *list )
{
	return List( path, 1, list );
}
STDMETHODIMP CDirectory::Delete( BSTR path )
{
	Clear( path );
	return S_OK;
}
void CDirectory::Clear( BSTR path )
{
	VARIANT vList;
	List( path, 0, &vList );

	CComSafeArray<VARIANT> array;
	array.Attach( vList.parray );

	for( UINT i = 0; i < array.GetCount(); i++ )
	{
		IListEntry * pLE;
		HRESULT hr = (CComVariant( (VARIANT) array.GetAt(i) ).pdispVal)->QueryInterface( IID_IListEntry, (void **) &pLE );

		if( SUCCEEDED( hr ) )
		{
			BSTR name;
			pLE->get_Name( &name );

			TCHAR newPath[MAX_PATH];
			StringCchCopy( newPath, MAX_PATH, CW2T(path) );
			PathAppend( newPath, CW2T(name) );

			BOOL bDir;
			pLE->get_IsDirectory( &bDir );

			if( bDir )
				Clear( CT2W(newPath) );
			else
				DeleteFile( newPath );
			pLE->Release();
		}
	}
	RemoveDirectory( CW2T(path) );
}
