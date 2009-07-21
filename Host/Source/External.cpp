#include "stdafx.h"
#include "External.h"
#include "PopupMenu.h"
#include "NotifyIcon.h"
#include "MainWnd.h"
#include "File.h"
#include "Directory.h"
#include "XMPP.h"
#include "UTF8.h"
#include "shortcut.h"
#include "comctrlwrapper.h"
#include "module.h"
#include "Hash.h"

#include "pdnwnd.h"

CExternal::CExternal() : m_pWnd( 0 ), m_pModule( 0 ), m_pComCtrl( 0 )
{
}
CExternal::~CExternal()
{
	m_pComCtrl->Release();
	m_pWnd->Release();
}

// IExternal
HRESULT CExternal::Init( void* pWnd, void* pModule )
{
	m_pWnd = (CPdnWnd *) pWnd;
	m_pWnd->AddRef();

	m_pModule = (CPandionModule *) pModule;

	(new CComObject<CComCtrl>)->QueryInterface( &m_pComCtrl );
	m_pComCtrl->SetWnd( m_pWnd );

	return S_OK;
}

STDMETHODIMP CExternal::get_wnd( VARIANT* pDisp )
{
	m_pWnd->QueryInterface( IID_IDispatch, (void**)&pDisp->pdispVal );
	pDisp->vt = VT_DISPATCH;
	return S_OK;
}
STDMETHODIMP CExternal::get_mainWnd( VARIANT* pDisp )
{
	CMainWnd *pMainWnd;

	m_pModule->GetMainWnd( &pMainWnd );
	pMainWnd->QueryInterface( IID_IDispatch, (void**)&pDisp->pdispVal );
	pMainWnd->Release();

	pDisp->vt = VT_DISPATCH;
	return S_OK;
}
STDMETHODIMP CExternal::get_windows( VARIANT* pDisp )
{
	ScrRun::IDictionary *pWindows;

	m_pModule->GetWindows( &pWindows );
	pWindows->QueryInterface( IID_IDispatch, (void**)&pDisp->pdispVal );
	pWindows->Release();

	pDisp->vt = VT_DISPATCH;
	return S_OK;
}
STDMETHODIMP CExternal::get_globals( VARIANT* pDisp )
{
	ScrRun::IDictionary *pGlobals;
	
	m_pModule->GetGlobals( &pGlobals );
	pGlobals->QueryInterface( IID_IDispatch, (void**)&pDisp->pdispVal );
	pGlobals->Release();

	pDisp->vt = VT_DISPATCH;
	return S_OK;
}
STDMETHODIMP CExternal::get_ComCtrl( VARIANT* pDisp )
{
	m_pComCtrl->QueryInterface( IID_IDispatch, (void**)&pDisp->pdispVal );
    pDisp->vt = VT_DISPATCH;
	return S_OK;
}
STDMETHODIMP CExternal::get_HTTPEngine( VARIANT* pDisp )
{
	IHTTP *pHTTP;

	m_pModule->GetHTTP( &pHTTP );
	pHTTP->QueryInterface( IID_IDispatch, (void**)&pDisp->pdispVal );
	pHTTP->Release();

	pDisp->vt = VT_DISPATCH;
	return S_OK;
}
STDMETHODIMP CExternal::get_SASL( VARIANT* pDisp )
{
	ISASL *pSASL;

	m_pModule->GetSASL( &pSASL );
	pSASL->QueryInterface( IID_IDispatch, (void**)&pDisp->pdispVal );
	pSASL->Release();

	pDisp->vt = VT_DISPATCH;
	return S_OK;
}
STDMETHODIMP CExternal::createWindow( BSTR name, BSTR file, VARIANT* params, BOOL bPopUnder, VARIANT* pDisp )
{
	RECT rc = { 300, 300, rc.left + 500, rc.top + 300 };

	CComPtr< CPdnWnd > spNewWnd = new CComObject< CPdnWnd >;

	CCreateParams *p = new CCreateParams( name, file, params, m_pModule );
    spNewWnd->Create( 0, &rc, NULL, 0, 0, 0U, p );
	delete p;

	spNewWnd->QueryInterface( IID_IDispatch, (void**)&pDisp->pdispVal );
	pDisp->vt = VT_DISPATCH;
	return S_OK;
}
STDMETHODIMP CExternal::shellExec( BSTR verb, BSTR file, BSTR params, BSTR dir, DWORD nShowCmd )
{
	USES_CONVERSION;
	ShellExecute( GetDesktopWindow(), W2T( verb ), W2T( file ), W2T( params ), W2T( dir ), nShowCmd );
	return S_OK;
}
STDMETHODIMP CExternal::get_cursorX( VARIANT* retVal )
{
	POINT pt;
	::GetCursorPos( &pt );
	retVal->intVal = pt.x;
	retVal->vt = VT_I4;
	return S_OK;
}
STDMETHODIMP CExternal::get_cursorY( VARIANT* retVal )
{
	POINT pt;
	::GetCursorPos( &pt );
	retVal->intVal = pt.y;
	retVal->vt = VT_I4;
	return S_OK;
}
STDMETHODIMP CExternal::get_notifyIcon( VARIANT* pDisp )
{
	CMainWnd *pMain;

	m_pModule->GetMainWnd( &pMain );
	pMain->GetNotifyIcon( pDisp );
	pMain->Release();

	return S_OK;
}
STDMETHODIMP CExternal::get_newPopupMenu( VARIANT* pDisp )
{
	(new CComObject< CPopupMenu >)->QueryInterface( IID_IDispatch, (void**)&(pDisp->pdispVal) );
	pDisp->vt = VT_DISPATCH;
	return S_OK;
}
STDMETHODIMP CExternal::get_IPs( VARIANT* pStr )
{
	pStr->vt = VT_BSTR;
	pStr->bstrVal = SysAllocString( L"error" );

	char strHostName[1024];
	if( gethostname( strHostName, 1024 ) == SOCKET_ERROR ) return S_FALSE;
	hostent* pHost = gethostbyname( strHostName );

	TCHAR strAddr[20];
	_bstr_t retStr = "";
	for( int i = 0; pHost->h_addr_list[i] != 0; i++ )
	{
		_stprintf( strAddr, TEXT("%u.%u.%u.%u"), (UCHAR) pHost->h_addr_list[i][0], (UCHAR) pHost->h_addr_list[i][1],
			(UCHAR) pHost->h_addr_list[i][2], (UCHAR) pHost->h_addr_list[i][3] );
		retStr += strAddr;
		retStr += TEXT(";");
	}

	SysFreeString( pStr->bstrVal );
	pStr->bstrVal = retStr.Detach();

	return S_OK;
}
STDMETHODIMP CExternal::get_CmdLine( VARIANT* pStr )
{
	pStr->vt = VT_BSTR;
	pStr->bstrVal = SysAllocString( GetCommandLineW() );
	return S_OK;
}
STDMETHODIMP CExternal::sleep( DWORD dwMilliseconds )
{
	::Sleep( dwMilliseconds );
	return S_OK;
}
STDMETHODIMP CExternal::File( BSTR path, VARIANT *pDisp )
{
	pDisp->vt = VT_DISPATCH;
	(new CComObject<CFile>)->QueryInterface( &pDisp->pdispVal );
    
	((IPdnFile*)pDisp->pdispVal)->Create( path, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, OPEN_ALWAYS );
	
	return S_OK;
}
STDMETHODIMP CExternal::FileExists( BSTR path, BOOL *bExists )
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile( CW2T(path), &FindFileData );

	if( hFind != INVALID_HANDLE_VALUE )
	{
		FindClose( hFind );
		*bExists = TRUE;
	}
	else
		*bExists = FALSE;
    return S_OK;
}
STDMETHODIMP CExternal::get_Directory( VARIANT *pDisp )
{
	pDisp->vt = VT_DISPATCH;
	(new CComObject< CDirectory >)->QueryInterface( &pDisp->pdispVal );

	return S_OK;
}
STDMETHODIMP CExternal::get_XMPP( VARIANT *pDisp )
{
	IXMPP *pXMPP;
	
	m_pModule->GetXMPP( &pXMPP );
	pXMPP->QueryInterface( IID_IDispatch, (void **) &pDisp->pdispVal );
	pXMPP->Release();

	pDisp->vt = VT_DISPATCH;
	return S_OK;
}
STDMETHODIMP CExternal::StringToSHA1(BSTR str, BSTR *strSHA1)
{
	LPSTR szUTF8Buf = CW2UTF8(str);
	unsigned char digest[20];
	
	Hash::SHA1((unsigned char*)szUTF8Buf, strlen(szUTF8Buf), digest);

	wchar_t hexHash[20*2+1];
	hexHash[20*2] = 0;
	for( unsigned i = 0; i < 20; i++ )
	{
		hexHash[2*i] = (digest[i] >> 4) + (((digest[i] >> 4) < 0xA) ? L'0' : (L'a' - 0xA));
		hexHash[2*i+1] = (digest[i] & 0x0F) +  (((digest[i] & 0x0F) < 0xA) ? L'0' : (L'a' - 0xA));
	}
	*strSHA1 = _bstr_t(hexHash).Detach();
	return S_OK;
}
STDMETHODIMP CExternal::GetSpecialFolder( int nFolder, BSTR *Path )
{
	TCHAR szPath[MAX_PATH];
	if( SUCCEEDED( SHGetFolderPath( NULL, nFolder, NULL, SHGFP_TYPE_CURRENT, szPath ) ) )
		*Path = SysAllocString( CT2W(szPath) );
	else
        *Path = SysAllocString( L"" );
	return S_OK;
}
CRegKey CExternal::Crawl( CRegKey RegKey, BSTR strKey )
{
	CRegKey retVal;
	if( RegKey )
	{
		LPWSTR pSlash = wcsstr( strKey, L"\\" );
		if( pSlash )
		{
			*pSlash = 0;
			retVal.Open( RegKey, CW2T(strKey), KEY_READ );
			if( pSlash + 1 != 0 )
                retVal = Crawl( retVal, pSlash + 1 );
			*pSlash = '\\';
		}
		else
			retVal.Open( RegKey, CW2T(strKey), KEY_READ );
		return retVal;
	}
	else
		return CRegKey();
}
STDMETHODIMP CExternal::RegRead( BSTR strHKey, BSTR strKey, BSTR value, VARIANT* vRetVal )
{
	HRESULT hr = S_OK;
	CRegKey RegKey;
	if( wcsstr( strHKey, L"HKEY_CLASSES_ROOT" ) )
		RegKey.Attach( HKEY_CLASSES_ROOT );
	else   if( wcsstr( strHKey, L"HKEY_CURRENT_USER" ) )
		RegKey.Attach( HKEY_CURRENT_USER );
	else   if( wcsstr( strHKey, L"HKEY_LOCAL_MACHINE" ) )
		RegKey.Attach( HKEY_LOCAL_MACHINE );
	else   if( wcsstr( strHKey, L"HKEY_USERS" ) )
		RegKey.Attach( HKEY_USERS );
	else   if( wcsstr( strHKey, L"HKEY_PERFORMANCE_DATA" ) )
		RegKey.Attach( HKEY_PERFORMANCE_DATA );
	else   if( wcsstr( strHKey, L"HKEY_PERFORMANCE_TEXT" ) )
		RegKey.Attach( HKEY_PERFORMANCE_TEXT );
	else   if( wcsstr( strHKey, L"HKEY_PERFORMANCE_NLSTEXT" ) )
		RegKey.Attach( HKEY_PERFORMANCE_NLSTEXT );
	else
		return E_FAIL;

	DWORD dwType = 0;
	ULONG nBytes = 8192;
	void *pData  = new BYTE[nBytes];
	CRegKey key = Crawl( RegKey, strKey );
	if( key )
	{
		LONG err = key.QueryValue( CW2T(value), &dwType, pData, &nBytes );
		if( err == ERROR_SUCCESS )
			switch( dwType )
			{
			case REG_DWORD:
				CComVariant(*(DWORD*)pData).Detach( vRetVal );
				break;
			case REG_SZ:
				CComVariant(CComBSTR(CT2W((LPTSTR)pData))).Detach( vRetVal );
				break;
			}
		else
		{
			hr =  E_FAIL;
		}
	}
	else
	{
		hr = E_FAIL;
	}
	delete pData;
	return hr;
}
STDMETHODIMP CExternal::get_Shortcut( VARIANT *pDisp )
{
	pDisp->vt = VT_DISPATCH;
	return (new CComObject< Shortcut >)->QueryInterface( &pDisp->pdispVal );
}
STDMETHODIMP CExternal::UnZip( BSTR path, BSTR targetDir, int *nSuccess )
{
	unzFile pFile = unzOpen( CW2A(path) );
	*nSuccess = 0;

	if( pFile )
	{
		if( unzGoToFirstFile( pFile ) == UNZ_OK )
		{
			IPdnFile *target_file;
			int nRead = 0;
			DWORD len = 4096;
			BYTE *buf = new BYTE[len];
			char file_name[MAX_PATH] = "";
			char file_path[2*MAX_PATH] = "";
			unz_file_info_s file_info;
			do
			{
				if( unzOpenCurrentFile( pFile ) == UNZ_OK )
				{
					unzGetCurrentFileInfo( pFile, &file_info, file_name, MAX_PATH, 0, 0, 0, 0 );

					StringCchCopyA( file_path, 2*MAX_PATH, CW2A(targetDir) );
					PathAppendA( file_path, file_name );

					(new CComObject<CFile>)->QueryInterface( &target_file );
    				if( target_file->Create( CA2W(file_path), GENERIC_WRITE, FILE_SHARE_READ, OPEN_ALWAYS ) == S_OK )
					{
						target_file->Seek( file_info.uncompressed_size, 0, FILE_BEGIN );
						target_file->SetEOF();
						target_file->Seek( 0, 0, FILE_BEGIN );
						do
						{						
							nRead = unzReadCurrentFile( pFile, buf, len );
							target_file->Write( buf, nRead );
						}
						while( nRead > 0 );
					}
					else
					{
						*nSuccess = -1;
					}
					target_file->Release();
					unzCloseCurrentFile( pFile );
				}
				else
				{
					*nSuccess = -1;
				}
			}
			while( unzGoToNextFile( pFile ) == UNZ_OK );

			delete buf;
		}
		else
		{
			*nSuccess = -1;
		}
		unzClose( pFile );
	}
	else
	{
        *nSuccess = -1;
	}
	return S_OK;
}
STDMETHODIMP CExternal::Base64ToString( BSTR b64String, BSTR *UTF16String )
{
	unsigned int b64bufLen = strlen( CW2A( b64String ) ) + 1;
	LPSTR b64buf = new char[ b64bufLen ];
	StringCchCopyA( b64buf, b64bufLen, CW2A( b64String ) );

	int reqLen = ATL::Base64DecodeGetRequiredLength( strlen( b64buf ) );
	BYTE* decodebuf = new BYTE[reqLen+1];

	ATL::Base64Decode( b64buf, strlen( b64buf ), decodebuf, &reqLen );
	decodebuf[reqLen] = 0;
	*UTF16String = SysAllocString( CUTF82W( (LPSTR) decodebuf ) );

	delete decodebuf;
	delete b64buf;

	return S_OK;
}
STDMETHODIMP CExternal::StringToBase64( BSTR UTF16String, BSTR *b64String )
{
	unsigned int UTF8StringLen = strlen( CW2UTF8( UTF16String ) ) + 1;
	char* UTF8String = new char[ UTF8StringLen ];
	StringCchCopyA( UTF8String, UTF8StringLen, CW2UTF8( UTF16String ) );

	int iReqLen = ATL::Base64EncodeGetRequiredLength( strlen( UTF8String ) );
	LPSTR pB64Buffer = new char[iReqLen+1];

	if( ATL::Base64Encode( (BYTE*)UTF8String, strlen( UTF8String ), pB64Buffer, &iReqLen, ATL_BASE64_FLAG_NOCRLF ) )
	pB64Buffer[iReqLen] = 0;
	*b64String = SysAllocString( CA2W(pB64Buffer) );

	delete pB64Buffer;
	delete UTF8String;

	return S_OK;
}
BOOL CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam )
{
	static int nFullScreen = 0;
	RECT WindowRect;
	GetWindowRect( hwnd, &WindowRect );

	if( WindowRect.left == 0 && WindowRect.top == 0 && WindowRect.right == GetSystemMetrics( SM_CXSCREEN ) && WindowRect.bottom == GetSystemMetrics( SM_CYSCREEN ) )
	{
/*		char strTitle[256];
		GetWindowText( hwnd, strTitle, 256 );
		FILE *f = fopen( "c:\\pandiondebug.log", "a" );
		fprintf( f, "%s\t%d:%d:%d:%d\n", strTitle, WindowRect.left, WindowRect.top, WindowRect.right, WindowRect.bottom );
		fclose( f );
*/		nFullScreen++;
		if( nFullScreen > 2 )
			*((bool *)lParam) = true;
	}
	return true;
}
STDMETHODIMP CExternal::Fullscreen( BOOL *bFullscreen )
{
/*	SYSTEMTIME st;
	GetSystemTime( &st );

	FILE *f = fopen( "c:\\pandiondebug.log", "a" );
	fprintf( f, "Enumerating window sizes at %d:%d:%d\n", st.wHour, st.wMinute, st.wSecond );
	fclose( f );
*/
	*bFullscreen = false;
	EnumWindows( EnumWindowsProc, (LPARAM) bFullscreen );
	return S_OK;
}