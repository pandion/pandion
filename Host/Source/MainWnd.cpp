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
 * Filename:    MainWnd.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "NotifyIcon.h"
#include "Module.h"
#include "MainWnd.h"

CMainWnd::CMainWnd() : CPdnWnd(), m_pNotIc( NULL )
{
	m_uTaskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));
}
CMainWnd::~CMainWnd()
{
	m_pNotIc->Release();
	m_pNotIc = NULL;
}

void CMainWnd::GetNotifyIcon( VARIANT* pDisp )
{
	if( m_pNotIc )
	{
		m_pNotIc->QueryInterface( IID_IDispatch, (void **)&pDisp->pdispVal );
		pDisp->vt = VT_DISPATCH;
	}
	else
		ATLASSERT( 0 );
}

LRESULT CMainWnd::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	m_pNotIc = new CComObject< CNotifyIcon >;
	m_pNotIc->AddRef();
	m_pNotIc->init( m_hWnd, WM_NOTIFYICON );

	bHandled = FALSE;
	return FALSE;
}	
LRESULT CMainWnd::OnNotifyIcon( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if( lParam == WM_MOUSEMOVE || !lParam )
		return 1;

	VARIANT* v = new _variant_t( (long) lParam );
	FireEvent( m_pNotIc->getHandler(), v, 1 );
	delete v;
	return false;
}
LRESULT CMainWnd::OnCopyData( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	COPYDATASTRUCT* pCDS = (COPYDATASTRUCT*) lParam;
	if( pCDS->dwData == COPYDATA_CMDLINE )
	{
		CComVariant v( (OLECHAR *) pCDS->lpData );

		if( m_sCmdLineHandler.Length() )
			FireEvent( m_sCmdLineHandler, &v, 1 );

		return bHandled = true;
	}
	return bHandled = false;
}
LRESULT CMainWnd::OnTaskbarRestart( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	m_pNotIc->show();
	return 0;
}

LRESULT CMainWnd::OnClose( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	PostQuitMessage(0);
	
	return bHandled = false;
}

STDMETHODIMP CMainWnd::close()
{
//	PostMessage(WM_CLOSE);

	VARIANT* pvWndItems = new VARIANT;
	VARIANT* pvElements;
	ScrRun::IDictionary *pWindows;
	
	m_pModule->GetWindows( &pWindows );
	pWindows->Items( pvWndItems );
	pWindows->Release();

	SafeArrayLock( pvWndItems->parray );

	SafeArrayAccessData( pvWndItems->parray, (void**) &pvElements );
	for( int i = pvWndItems->parray->rgsabound->cElements-1; i >= 0; i-- )
	{
		((CPdnWnd*)pvElements[i].pdispVal)->CPdnWnd::close();
		pvElements[i].pdispVal->Release();
		pvElements[i].pdispVal = NULL;
	}
	SafeArrayUnlock( pvWndItems->parray );

	delete pvWndItems;
	return S_OK;
}
HWND CMainWnd::GetMainWindow()
{
	LPCTSTR s = GetWndClassName();
	HWND h = FindWindow( s, 0 );
	delete s;
	return h;
}
ATL::CWndClassInfo& CMainWnd::GetWndClassInfo()
{
	// Find the memory leak ;-)
	static ATL::CWndClassInfo wc =
	{
		{ sizeof(WNDCLASSEX), CS_DBLCLKS, StartWindowProc,
		0, 0, NULL, NULL, NULL, (HBRUSH)(COLOR_BTNFACE), NULL, GetWndClassName(), NULL },
		NULL, NULL, IDC_ARROW, TRUE, 0, _T("")
	};
	return wc;
}
LPTSTR CMainWnd::GetWndClassName()
{
	TCHAR* strClass = new TCHAR[MAX_PATH+20];
	GetModuleFileName( NULL, strClass, MAX_PATH );
	StringCchCat( strClass, MAX_PATH+20, TEXT(" Main Window Class") );
	return strClass;
}