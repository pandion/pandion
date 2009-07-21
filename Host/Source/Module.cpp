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
 * Filename:    Module.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#include "stdafx.h"
#include "MainWnd.h"
#include "XMPP.h"
#include "HTTPEngine.h"
#include "Module.h"
#include "SASL.h"

CPandionModule::~CPandionModule()
{
}
void CPandionModule::GetMainWnd( CMainWnd **ppMainWnd )
{
	if( m_pMainWnd )
	{
		m_pMainWnd->AddRef();
		*ppMainWnd = m_pMainWnd;
	}
	else
	{
		*ppMainWnd = NULL;
		ATLASSERT( 0 );
	}
}
void CPandionModule::GetHTTP( IHTTP **ppHTTP )
{
	if( m_pHTTP )
		m_pHTTP->QueryInterface( ppHTTP );
	else
	{
		*ppHTTP = NULL;
		ATLASSERT( 0 );
	}
}
void CPandionModule::GetXMPP( IDispatch **ppXMPP )
{
	m_XMPP.QueryInterface( IID_IDispatch, (void**) ppXMPP );
}
void CPandionModule::GetSASL( ISASL **ppSASL )
{
	if( m_pSASL )
		m_pSASL->QueryInterface( ppSASL );
	else
	{
		*ppSASL = NULL;
		ATLASSERT( 0 );
	}
}
void CPandionModule::GetGlobals( ScrRun::IDictionary **ppGlobals )
{
	if( m_spGlobals )
		m_spGlobals->QueryInterface( ppGlobals );
	else
	{
		*ppGlobals = NULL;
		ATLASSERT( 0 );
	}
}
void CPandionModule::GetWindows( ScrRun::IDictionary **ppWindows )
{
	if( m_spWindows )
		m_spWindows->QueryInterface( ppWindows );
	else
	{
		*ppWindows = NULL;
		ATLASSERT( 0 );
	}
}

BOOL CPandionModule::IsRunning()
{
	LPTSTR str = new TCHAR[MAX_PATH];
	int len = GetModuleFileName( NULL, str, MAX_PATH );
	for( int i = 0; i<len; i++)
	{
		if( str[i] == '\\' )
			str[i] = '$';
	}
	_tcslwr( str );
	CreateMutex( 0, 0, str );
	if( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		return true;
	}
	delete str;

	return false;
}
BOOL CPandionModule::IsIEVersionOK()
{
	BOOL bVersionOK = FALSE;

	HINSTANCE hDllInst = LoadLibrary(TEXT("shdocvw.dll"));
	if(NULL != hDllInst)
	{
		DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC) GetProcAddress(hDllInst, "DllGetVersion");
		if(NULL != pDllGetVersion)
		{
			DLLVERSIONINFO dvi;
			ZeroMemory(&dvi, sizeof(dvi));
			dvi.cbSize = sizeof(dvi);
			HRESULT hr = (*pDllGetVersion)(&dvi);

			if( dvi.dwMajorVersion == 5 && dvi.dwMinorVersion >= 50 )
			{
				bVersionOK = TRUE;
			}
			else if( dvi.dwMajorVersion >= 6 )
			{
				bVersionOK = TRUE;
			}
		}
		FreeLibrary(hDllInst);
	}
	return bVersionOK;
}

HRESULT CPandionModule::PreMessageLoop( int nShowCmd )
{
	/* attempt to register the application */
	if( SUCCEEDED( RegisterAppId() ) )
		RegisterServer( TRUE );

	//_CrtSetBreakAlloc(2654);
	_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF );
	/* Let ATL Initialize */
	HRESULT hr = CAtlExeModuleT< CPandionModule >::PreMessageLoop(nShowCmd);
	if( FAILED(hr) )
		return hr;

	/* Check if the software is already running and take action accordingly */
	if( IsRunning() )
	{
		HWND h = CMainWnd::GetMainWindow();

		if( !( WS_VISIBLE & GetWindowLong( h, GWL_STYLE ) ) )
            ShowWindow( h, SW_RESTORE );
		SetForegroundWindow( h );

        COPYDATASTRUCT CDS;
		CDS.dwData = COPYDATA_CMDLINE;
		CDS.lpData = GetCommandLineW();
		CDS.cbData = ( wcslen( (const wchar_t *) CDS.lpData ) + 1 ) * sizeof(wchar_t);
		SendMessage( h, WM_COPYDATA, (WPARAM) 0, (LPARAM) &CDS );

		if( IsWindow( h ) )
			ExitProcess( 0 );
		else
            TerminateProcess( (HANDLE)GetWindowLong( h, GWL_HINSTANCE ), 0 );
	}

	/* Check The Version of Internet Explorer */
	if( !IsIEVersionOK() )
	{
		::MessageBox( NULL, TEXT("This program requires Microsoft Internet Explorer version 5.5 or higher.\nVisit this website to upgrade: http://www.microsoft.com/ie"), TEXT("Error"), MB_OK | MB_ICONERROR );
		::ExitProcess( -1 );
	}

	/* Initialize COM */
	if( FAILED( InitializeCom() ) )
	{
		::MessageBox( NULL, TEXT("Failed to initialize COM."), TEXT("Pandion"), MB_OK | MB_ICONERROR );
		::ExitProcess( -1 );
	}

	/* Initialize ATL ActiveX support */
	AtlAxWinInit();

	/* Initialize globals Dictionary */
	m_spGlobals.CoCreateInstance( OLESTR("Scripting.Dictionary") );

	TCHAR cwd[MAX_PATH];
	GetModuleFileName( NULL, cwd, MAX_PATH );
	PathRemoveFileSpec( cwd );
	PathAppend( cwd, TEXT("src") );
	PathAddBackslash( cwd );
	m_spGlobals->Add( &CComVariant( TEXT("cwd") ), &CComVariant(cwd) );
	::SetCurrentDirectory(cwd);

	/* Initialize Global Classes */
	m_spWindows.CoCreateInstance( OLESTR("Scripting.Dictionary") );

	(new CComObject< CHTTP >)->QueryInterface( &m_pHTTP );
	(new CComObject< CSASL >)->QueryInterface( &m_pSASL );

	/* Create the main window */
	RECT rc = { 100, 100, rc.left + 420, rc.top + 310 };

	m_pMainWnd = (CMainWnd*) new CComObject< CMainWnd >;
	m_pMainWnd->AddRef();

	TCHAR strURL[MAX_PATH];
	GetModuleFileName( NULL, strURL, MAX_PATH );
	PathRemoveFileSpec( strURL );
	PathAppend( strURL, TEXT("src") );
	PathAddBackslash( strURL );
	PathAppend( strURL, TEXT("main.html") );

	CCreateParams *p = new CCreateParams( OLESTR("MainWindow"), CComBSTR(strURL), &_variant_t( 0 ), this );

	((CWindowImplBase*)m_pMainWnd)->Create( 0, &rc, TEXT(""),
		m_pMainWnd->GetWndStyle( 0 ), m_pMainWnd->GetWndExStyle( 0 ), (HMENU)0,
		m_pMainWnd->GetWndClassInfo().Register(&(m_pMainWnd->m_pfnSuperWindowProc)), (void*) p );
	delete p;

	m_XMPP.SetMainWnd( m_pMainWnd );

	return S_OK;
}
BOOL CPandionModule::PreTranslateAccelerator( MSG* pMsg )
{
	if( ( pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST ) &&
		( pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST ) )
		return false;

	HWND TargetWnd = pMsg->hwnd;
	while( HWND parent = GetParent( TargetWnd ) ) TargetWnd = parent;

	// Temporary fix for random crash when user presses escape.
/*	if(pMsg->message == 256 &&
		pMsg->wParam == 27 &&
		pMsg->lParam == 65537)
	{
		if( ::SendMessage( TargetWnd, WM_FORWARDMSG, 0, (LPARAM) pMsg ) )
			return true;
		return false;
	}
	else
	{
*/		if( ::SendMessage( TargetWnd, WM_FORWARDMSG, 0, (LPARAM) pMsg ) )
			return true;
		return false;
/*	}*/
}

void CPandionModule::RunMessageLoop()
{
	MSG msg;
    while( GetMessage( &msg, 0, 0, 0 ) )
	{
        if( !PreTranslateAccelerator( &msg ) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

HRESULT CPandionModule::PostMessageLoop()
{
	/* Release the main window */
	m_pMainWnd->Release();

	m_XMPP.Release();
	m_pHTTP->Release();

	/* Cleanup winsock */
	WSACleanup();

	/* Unload ATL ActiveX support */
	AtlAxWinTerm();

	/* Uninitialize COM */
	UninitializeCom();

	CAtlExeModuleT<CPandionModule>::PostMessageLoop();
	return S_OK;
}

extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	/* Initialize Winsock 2.0 */
	WSADATA wsad;
	if( WSAStartup( MAKEWORD(2,0), &wsad ) )
	{
		::MessageBox( NULL, TEXT("Failed to initialize Winsock 2.0"), TEXT("Pandion"), MB_OK | MB_ICONERROR );
	}

	STARTUPINFO StartupInfo;
	StartupInfo.dwFlags = 0;
	GetStartupInfo( &StartupInfo );
	
	CPandionModule _AtlModule;
    return _AtlModule.WinMain( StartupInfo.dwFlags & STARTF_USESHOWWINDOW ? StartupInfo.wShowWindow : SW_SHOWDEFAULT );
}

