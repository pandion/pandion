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
 * Filename:    PdnWnd.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "External.h"
#include "MenuBar.h"
#include "Module.h"
#include "PdnWnd.h"


CCreateParams::CCreateParams(BSTR name, BSTR url, VARIANT* pParams, CPandionModule* pModule)
{
	m_name = name;
	m_url = url;
	m_pParams = pParams;
	m_pModule = pModule;
}

class CJSInvoke
{
public:
	BSTR Handler;
	DWORD nParams;
	CJSInvoke(BSTR str, DWORD dw)
	{
		Handler = str;
		nParams = dw;
	}
};

CPdnWnd::CPdnWnd() :
	m_hWndLastFocusedWindow(0), m_hWndActiveWindow(0), m_hWndFocus(0),
	m_bPopUnder(false), m_sMinHandler(""), 	m_sCloseHandler(""),
	m_sMenuHandler(""), m_sCmdLineHandler(""), 
	m_sRestoreHandler(""), m_sName(""), m_sURL(""), m_vParams(0),
	m_External(*this)
{
	m_External.DisableSelfDelete();
	m_minSize.x  = 160;
	m_minSize.y  = 200;
	_CrtMemCheckpoint(&state);
}
CPdnWnd::~CPdnWnd()
{
	_CrtMemDumpAllObjectsSince(&state);
}

void CPdnWnd::OnFinalMessage(HWND hWnd)
{
	/* Remove the window from the global window dictionary */
	CComPtr<ScrRun::IDictionary> pWindows;
	m_pModule->GetWindows(&pWindows);
	pWindows->Remove(&CComVariant(m_sName));

	/* The internet explorer control is automatically destroyed by ATL, we just have to free the reference */
	m_pBrowser->Quit();
	m_pBrowser->Release();

	/* Destroy the menu bar */
	m_pMenuBar->Release();

	/* Free the internet security manager */
	m_pSecurityMgr->Release();
}

// General Functionality
void CPdnWnd::ChangeIcon(int icon)
{
	HICON hIcon16 = (HICON) LoadIcon(::GetModuleHandle(NULL), MAKEINTRESOURCE(icon));
	HICON hIcon32 = (HICON) LoadIcon(::GetModuleHandle(NULL), MAKEINTRESOURCE(icon));

	if(!hIcon16 || !hIcon32) {
		return;
	}

	SendMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM) hIcon32);
	SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM) hIcon16);
}
void CPdnWnd::PopUnder(BOOL b)
{
	m_bPopUnder = b;
}

// Message Handlers
LRESULT CPdnWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CCreateParams *p = (CCreateParams*) ((CREATESTRUCT*)lParam)->lpCreateParams;
	m_sName = p->m_name;
	m_sURL  = p->m_url;
	m_vParams  = *(p->m_pParams);
	m_pModule = p->m_pModule;

	/* Init COM in case window is created from a different thread */
	::OleInitialize(NULL);
	AtlAxWinInit();

	/* Create Internet Security Manager Object */
	CoCreateInstance(CLSID_InternetSecurityManager, NULL, CLSCTX_INPROC_SERVER, IID_IInternetSecurityManager, (void**)&m_pSecurityMgr);
	m_pSecurityMgr->AddRef();

	/* Create the External object */
	m_External.Init(m_pModule);

	/* Create the Internet Explorer control */
	if(SUCCEEDED(ContainerCreate()))
	{
		/* Create the menu bar */
		(new CComObject<CMenuBar>)->QueryInterface(&m_pMenuBar);
		VARIANT handle;
		m_pMenuBar->get_Handle(&handle);
		SetMenu((HMENU)handle.uintVal);
		m_pMenuBar->put_hwnd((DWORD)m_hWnd);

		/* Add the window to the global window dictionary */
		CComPtr<ScrRun::IDictionary> pWindows;
		m_pModule->GetWindows(&pWindows);
		pWindows->Add(&CComVariant(m_sName), &CComVariant((IDispatch*)this));
	}
	else
		return -1;

	return 0;
}
HRESULT CPdnWnd::ContainerCreate()
{
	// Create the control
	CreateControl(L"Shell.Explorer.2");

	// Set the host
	CComPtr<IUnknown> spHostUnk;
	HRESULT hr = AtlAxGetHost(m_hWnd, &spHostUnk);
	CComQIPtr<IAxWinHostWindow> spAxWin(spHostUnk);
	CComQIPtr<IObjectWithSite> pOWS(spAxWin);

	CComPtr<IUnknown> pUnk;
	QueryInterface(IID_IUnknown, (void **) &pUnk);

	if(FAILED(pOWS->SetSite(pUnk)))
		return hr;

	// Get the webbrowser control
	hr = spAxWin->QueryControl(IID_IWebBrowser2, (void**) &m_pBrowser);
	if(FAILED(hr))
		return hr;

	// Set the webbrowser event sink
	CComQIPtr<IWebBrowser2, &IID_IUnknown> pSourceUnk(m_pBrowser);
	hr = Advise(pSourceUnk);
	if(FAILED(hr))
		return hr;

	// initialize the browser
	m_pBrowser->put_RegisterAsBrowser(VARIANT_FALSE);
	m_pBrowser->put_RegisterAsDropTarget(VARIANT_TRUE);
	m_pBrowser->put_Silent(VARIANT_FALSE);
	m_pBrowser->Navigate(m_sURL, 0, 0, 0, 0);

	return S_OK;
}
LRESULT CPdnWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_sCloseHandler.Length())
	{
		FireEvent(m_sCloseHandler, 0, 0);
		DestroyWindow();
	}
	else
	{
		DestroyWindow();
	}
	return 0;
}
LRESULT CPdnWnd::OnJSInvoke(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HRESULT hr = FireEvent(((CJSInvoke *)wParam)->Handler, (VARIANT*)lParam, ((CJSInvoke *)wParam)->nParams);

	::SysFreeString((BSTR) ((CJSInvoke *)wParam)->Handler);
	::VariantClear((VARIANT *) lParam);
	delete (CJSInvoke *)wParam;

    return 0;
}
LRESULT CPdnWnd::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT windowRect, clientRect;
	GetWindowRect(&windowRect);
	GetClientRect(&clientRect);
	DWORD deltaWidth  = (windowRect.right - windowRect.left) - (clientRect.right - clientRect.left);
	DWORD deltaHeight = (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top);

	((MINMAXINFO*)lParam)->ptMinTrackSize.x = m_minSize.x /*+ deltaWidth*/ + deltaHeight - 
		GetSystemMetrics(SM_CYCAPTION);
	((MINMAXINFO*)lParam)->ptMinTrackSize.y = m_minSize.y + deltaHeight;
	return 0;
}
LRESULT CPdnWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(wParam)
	{
	case SIZE_MINIMIZED:
		if(m_sMinHandler.Length())
            FireEvent(m_sMinHandler, 0, 0);
		return 0;
	case SIZE_RESTORED:
		if(m_sRestoreHandler.Length())
			FireEvent(m_sRestoreHandler, 0, 0);
		return 0;
	default:
		bHandled = false;
		return 1;
	}
}
LRESULT CPdnWnd::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	VARIANT parameters[1];
	parameters[0].vt      = VT_UI4;
	parameters[0].uintVal = wParam;

	if(m_sMenuHandler.Length())
	{
		FireEvent(m_sMenuHandler, parameters, 1);
		return 0;
	}
	return 1;
}
LRESULT CPdnWnd::OnEndSession(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam)
        OnClose(WM_CLOSE, 0, 0, bHandled);
	return 0;
}

// IWebBrowserEvents2 Implementation
STDMETHODIMP CPdnWnd::OnWindowClosing(VARIANT_BOOL IsChildWindow, VARIANT_BOOL* &Cancel)
{
	Cancel = new VARIANT_BOOL;
	*Cancel = false;
	close();
	return S_OK;
}
STDMETHODIMP CPdnWnd::OnNavigateComplete2(IDispatch* pDisp, VARIANT URL)
{
	CComPtr<IDispatch> pDocDisp;
	m_pBrowser->get_Document(&pDocDisp);
	CComQIPtr<ICustomDoc, &IID_ICustomDoc> pCustomDoc(pDocDisp);

	return pCustomDoc->SetUIHandler(this);
}
// IDocHostUIHandler implementation
STDMETHODIMP CPdnWnd::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	return (dwID & 0x2) ? S_FALSE : S_OK;
}
STDMETHODIMP CPdnWnd::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	pInfo->cbSize  = sizeof(DOCHOSTUIINFO);
	pInfo->dwFlags = DOCHOSTUIFLAG_DISABLE_HELP_MENU | 
		DOCHOSTUIFLAG_NO3DBORDER | 
		DOCHOSTUIFLAG_SCROLL_NO | 
		DOCHOSTUIFLAG_URL_ENCODING_ENABLE_UTF8 | 
		DOCHOSTUIFLAG_IME_ENABLE_RECONVERSION;
	pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
	pInfo->pchHostCss = NULL;
	pInfo->pchHostNS = NULL;

	return S_OK;
}
STDMETHODIMP CPdnWnd::ShowUI(DWORD dwID, 
	IOleInPlaceActiveObject *pActiveObject, 
	IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, 
	IOleInPlaceUIWindow *pDoc)
{
	return S_OK;
}
STDMETHODIMP CPdnWnd::HideUI()
{
	return S_OK;
}
STDMETHODIMP CPdnWnd::UpdateUI()
{
	return  S_OK;
}
STDMETHODIMP CPdnWnd::EnableModeless(BOOL fEnable)
{
	return S_OK;
}
STDMETHODIMP CPdnWnd::OnDocWindowActivate(BOOL fActivate)
{
	return S_OK;
}
STDMETHODIMP CPdnWnd::OnFrameWindowActivate(BOOL fActivate)
{
	return S_OK;
}
STDMETHODIMP CPdnWnd::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fFrameWindow)
{
	return S_OK;
}
STDMETHODIMP CPdnWnd::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
	BYTE keys[256];

	GetKeyboardState(keys);

	if((keys[VK_F5]> 1) ||                       // Disable F5
		((keys['P']> 1 && keys[VK_CONTROL]> 1) && !(keys['N']> 1 && keys[VK_CONTROL]> 1 && keys[VK_MENU]> 1)) || // Disable Ctrl + P
		((keys['N']> 1 && keys[VK_CONTROL]> 1) && !(keys['N']> 1 && keys[VK_CONTROL]> 1 && keys[VK_MENU]> 1)))  // Disable Ctrl + N
		return S_OK;
	return S_FALSE;
}
STDMETHODIMP CPdnWnd::GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw)
{
	pchKey = NULL;
	return S_FALSE;
}
STDMETHODIMP CPdnWnd::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
	if(pDropTarget)
	{
		*ppDropTarget = pDropTarget;
		pDropTarget->AddRef();
		return S_OK;
	}
	*ppDropTarget = NULL;
	return E_INVALIDARG;
}
STDMETHODIMP CPdnWnd::GetExternal(IDispatch **ppDispatch)
{
	return m_External.QueryInterface(IID_IDispatch, (LPVOID*) ppDispatch);
}
STDMETHODIMP CPdnWnd::TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
{
	*ppchURLOut = NULL;
	return S_FALSE;
}
STDMETHODIMP CPdnWnd::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet)
{
	*ppDORet = NULL;
	return S_FALSE;
}
// IDocHostShowUI
STDMETHODIMP CPdnWnd::ShowMessage(HWND hwnd, LPOLESTR lpstrText,
								  LPOLESTR lpstrCaption, DWORD dwType,
								  LPOLESTR lpstrHelpFile, DWORD dwHelpContext,
								  LRESULT *plResult)
{
	return S_OK;
}
STDMETHODIMP CPdnWnd::ShowHelp(HWND hwnd, LPOLESTR pszHelpFile, UINT uCommand,
							   DWORD dwData, POINT ptMouse,
							   IDispatch *pDispatchObjectHit)
{
	return S_OK;
}

// IInternetSecurityManager
STDMETHODIMP CPdnWnd::SetSecuritySite(IInternetSecurityMgrSite *pSite)
{ 
	return m_pSecurityMgr->SetSecuritySite(pSite);
}
STDMETHODIMP CPdnWnd::GetSecuritySite(IInternetSecurityMgrSite **ppSite)
{
	return m_pSecurityMgr->GetSecuritySite(ppSite);
}
STDMETHODIMP CPdnWnd::MapUrlToZone(LPCWSTR pwszUrl, DWORD* pdwZone, DWORD dwFlags)
{
	*pdwZone = URLZONE_LOCAL_MACHINE;
	return S_OK;
}
STDMETHODIMP CPdnWnd::GetSecurityId(LPCWSTR pwszUrl, BYTE* pbSecurityId, DWORD* pcbSecurityId, DWORD_PTR dwReserved)
{
	return m_pSecurityMgr->GetSecurityId(pwszUrl, pbSecurityId, pcbSecurityId, dwReserved);
}
STDMETHODIMP CPdnWnd::ProcessUrlAction(LPCWSTR pwszUrl, DWORD dwAction, BYTE* pPolicy, DWORD cbPolicy,
	BYTE* pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved)
{
	if(pPolicy && cbPolicy>= 4) *pPolicy = URLPOLICY_ALLOW;
	return S_OK;
}
STDMETHODIMP CPdnWnd::QueryCustomPolicy(LPCWSTR pwszUrl, REFGUID guidKey, BYTE** ppPolicy, DWORD* pcbPolicy,
	BYTE* pContext, DWORD cbContext, DWORD dwReserved)
{
	return m_pSecurityMgr->QueryCustomPolicy(pwszUrl, guidKey, ppPolicy, pcbPolicy, pContext, cbContext, dwReserved);
}
STDMETHODIMP CPdnWnd::SetZoneMapping(DWORD dwZone, LPCWSTR lpszPattern, DWORD dwFlags)
{
	return m_pSecurityMgr->SetZoneMapping(dwZone, lpszPattern, dwFlags);
}
STDMETHODIMP CPdnWnd::GetZoneMappings(DWORD dwZone, IEnumString  **ppenumString, DWORD dwFlags)
{
	return m_pSecurityMgr->GetZoneMappings(dwZone, ppenumString, dwFlags);
}
// IPdnWnd implementation
STDMETHODIMP CPdnWnd::setTitle(BSTR Title)
{
	::SetWindowText(m_hWnd, CW2T(Title));

	return S_OK;
}
STDMETHODIMP CPdnWnd::setAOT(BOOL b)
{
	SetWindowPos(b ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOACTIVATE);
	return S_OK;
}
STDMETHODIMP CPdnWnd::minimize()
{
	ShowWindow(SW_MINIMIZE);
	return S_OK;
}
STDMETHODIMP CPdnWnd::maximize()
{
	ShowWindow(SW_MAXIMIZE);
	return S_OK;
}
STDMETHODIMP CPdnWnd::restore()
{
	ShowWindow(SW_RESTORE);
	return S_OK;
}
STDMETHODIMP CPdnWnd::close()
{
/*	CComQIPtr<IWebBrowser2, &IID_IUnknown> pSourceUnk(m_pBrowser);
	if(FAILED(Unadvise(pSourceUnk)))
		return S_FALSE;

	//m_pBrowser->Quit();

	//DestroyWindow();
*/
	PostMessage(WM_CLOSE);

	return S_OK;
}

STDMETHODIMP CPdnWnd::setSize(DWORD width, DWORD height)
{
	RECT windowRect, clientRect;
	GetWindowRect(&windowRect);
	GetClientRect(&clientRect);
	DWORD deltaWidth  = (windowRect.right - windowRect.left) - (clientRect.right - clientRect.left);
	DWORD deltaHeight = (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top);
	windowRect.right = windowRect.left + width + deltaWidth;
	windowRect.bottom = windowRect.top + height + deltaHeight;

	SetWindowPos(0, &windowRect, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	return S_OK;
}
STDMETHODIMP CPdnWnd::setPos(int left, int top)
{
	SetWindowPos(0, left, top, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
	return S_OK;
}
STDMETHODIMP CPdnWnd::setIcon(BSTR IconPath)
{
	HICON hIcon16 = (HICON) ::LoadImage(0, CW2T(IconPath), IMAGE_ICON, 16, 16,
		LR_LOADFROMFILE | LR_DEFAULTCOLOR);
	HICON hIcon32 = (HICON) ::LoadImage(0, CW2T(IconPath), IMAGE_ICON, 32, 32,
		LR_LOADFROMFILE | LR_DEFAULTCOLOR);

	if(!hIcon16 || !hIcon32)
		return S_FALSE;

	PostMessage(WM_SETICON, ICON_BIG, (LPARAM) hIcon32);
	PostMessage(WM_SETICON, ICON_SMALL, (LPARAM) hIcon16);

	return S_OK;
}
STDMETHODIMP CPdnWnd::hide(BOOL b)
{
	ShowWindow(b ? SW_HIDE : SW_SHOWNA);
	return S_OK;
}
/*
typedef struct {
    UINT  cbSize;
    HWND  hwnd;
    DWORD dwFlags;
    UINT  uCount;
    DWORD dwTimeout;
} FLASHWINFO, *PFLASHWINFO;
*/
typedef BOOL (__stdcall *FWEXPROC)(PFLASHWINFO); 
/*
#define FLASHW_STOP         0
#define FLASHW_CAPTION      0x00000001
#define FLASHW_TRAY         0x00000002
#define FLASHW_ALL          (FLASHW_CAPTION | FLASHW_TRAY)
#define FLASHW_TIMER        0x00000004
#define FLASHW_TIMERNOFG    0x0000000C
*/
STDMETHODIMP CPdnWnd::flash(DWORD u)
{
//	HWND Wnd = ::GetFocus();
//	while(m_hWnd != Wnd && Wnd != ::GetParent(Wnd)) Wnd = ::GetParent(Wnd);
//	if(Wnd == m_hWnd)
//		return S_OK;
	HWND FGWindow = ::GetForegroundWindow();
	if(FGWindow == m_hWnd)
		return S_FALSE;

	//PTC 1/13/2003 - FlashWindowEx is not compatible with Windows95/NT4
	FLASHWINFO fwi;
	fwi.cbSize = sizeof(FLASHWINFO);
	fwi.hwnd = m_hWnd;
	fwi.dwTimeout = 0;
	fwi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
    fwi.uCount = u;

	HMODULE user32_dll = LoadLibrary(TEXT("User32.dll"));

	if(user32_dll) 
	{
		FWEXPROC FlashWindowEx = (FWEXPROC)GetProcAddress(user32_dll, "FlashWindowEx"); 
 
        if(FlashWindowEx)
           		FlashWindowEx(&fwi);

		else
			FlashWindow(true);

		FreeLibrary(user32_dll); 
    } 

	SetForegroundWindow(FGWindow);
	
	return S_OK;
}
STDMETHODIMP CPdnWnd::resizeable(BOOL b)
{
	if(!b)
	{
        SetWindowLong(GWL_STYLE, GetWindowLong(GWL_STYLE) &
			!(WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX) | WS_CAPTION | WS_SYSMENU );
		SetWindowLong(GWL_EXSTYLE, GetWindowLong(GWL_EXSTYLE) | WS_EX_WINDOWEDGE);
	}
	else
	{
        SetWindowLong(GWL_STYLE, GetWindowLong(GWL_STYLE) |
			WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION | WS_SYSMENU);
		SetWindowLong(GWL_EXSTYLE, GetWindowLong(GWL_EXSTYLE) | WS_EX_WINDOWEDGE);
	}
	
	SetWindowPos(0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
	return S_OK;

}
STDMETHODIMP CPdnWnd::showMinBox(BOOL b)
{
	if(!b)
        SetWindowLong(GWL_STYLE, GetWindowLong(GWL_STYLE) & !WS_MINIMIZEBOX);
	else
		SetWindowLong(GWL_STYLE, GetWindowLong(GWL_STYLE) | WS_MINIMIZEBOX);

	return S_OK;
}
STDMETHODIMP CPdnWnd::showMaxBox(BOOL b)
{
	return E_NOTIMPL;
}
STDMETHODIMP CPdnWnd::showTitlebar(BOOL b)
{
	RECT rc;
	GetClientRect(&rc);
	if(!b)
	{
		SetWindowLong(GWL_STYLE, GetWindowLong(GWL_STYLE) & !(WS_OVERLAPPEDWINDOW));
		SetWindowLong(GWL_EXSTYLE, (GetWindowLong(GWL_EXSTYLE) &
			!(WS_EX_APPWINDOW)) | WS_EX_TOOLWINDOW);
	}
	else
	{
		SetWindowLong(GWL_STYLE, GetWindowLong(GWL_STYLE) | (WS_OVERLAPPEDWINDOW));
		SetWindowLong(GWL_EXSTYLE, (GetWindowLong(GWL_EXSTYLE) &
			!(WS_EX_TOOLWINDOW)) | WS_EX_APPWINDOW);
	}
	
	SetWindowPos(0, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
		SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);
	return S_OK;
}

typedef BOOL (__stdcall *SLWAPROC)(HWND, COLORREF, BYTE, DWORD); 

#define LWA_COLORKEY            0x00000001
#define LWA_ALPHA               0x00000002
#define ULW_COLORKEY            0x00000001
#define ULW_ALPHA               0x00000002
#define ULW_OPAQUE              0x00000004
#define WS_EX_LAYERED           0x00080000

STDMETHODIMP CPdnWnd::translucent(DWORD percent)
{
	HMODULE user32_dll = LoadLibrary(TEXT("User32.dll"));
    
	if(user32_dll) 
    { 
        SLWAPROC SetLayeredWindowAttributes = 
			(SLWAPROC)GetProcAddress(user32_dll, "SetLayeredWindowAttributes"); 
 
        if(SetLayeredWindowAttributes)
		{
			if(percent == 100)
			{
           		SetLayeredWindowAttributes(m_hWnd, 0, 255, LWA_ALPHA);
				SetWindowLong(GWL_EXSTYLE, GetWindowLong(GWL_EXSTYLE) & !WS_EX_LAYERED);
			}
			else
			{
				SetWindowLong(GWL_EXSTYLE, GetWindowLong(GWL_EXSTYLE) | WS_EX_LAYERED);
           		SetLayeredWindowAttributes(m_hWnd, 0, (255 * LOBYTE(LOWORD(percent))) / 100, LWA_ALPHA);
			}
		}

		FreeLibrary(user32_dll); 
    } 
	return S_OK;
}
typedef BOOL (__stdcall *AWPROC)(HWND, DWORD, DWORD); 
STDMETHODIMP CPdnWnd::animate(DWORD flags, DWORD duration)
{
	HMODULE user32_dll = LoadLibrary(TEXT("User32.dll"));
    
	if(user32_dll) 
    { 
        AWPROC AnimateWindow = (AWPROC)GetProcAddress(user32_dll, "AnimateWindow"); 
 
        if(AnimateWindow)
		{
			if(!AnimateWindow(m_hWnd, duration, flags))
			{
				hide(false);
			}
		}

		FreeLibrary(user32_dll); 
    } 
	return S_OK;
}
STDMETHODIMP CPdnWnd::Do(BSTR funcName, VARIANT *params, DWORD nParams)
{
	if(m_hWnd)
	{
		VARIANT* par = new VARIANT[nParams];
		for (DWORD i = 0; i <nParams; i++)
		{
			VariantInit(&par[i]);
			VariantCopy(&par[i], &params[i]);
		}
		CJSInvoke *jsi = new CJSInvoke(SysAllocString(funcName), nParams);
		PostMessage(WM_JSINVOKE, (WPARAM)jsi, (LPARAM)par);
	}
	return S_OK;
}
STDMETHODIMP CPdnWnd::get_params(VARIANT* retVal)
{
	::VariantCopy(retVal, &m_vParams);
	return S_OK;
}
STDMETHODIMP CPdnWnd::focus()
{
	return 	SetForegroundWindow(m_hWnd) ? S_OK : S_FALSE;
}
STDMETHODIMP CPdnWnd::get_left(VARIANT* retVal)
{
	RECT rc;
	GetWindowRect(&rc);
	retVal->intVal = rc.left;
	retVal->vt = VT_I4;
	return S_OK;
}
STDMETHODIMP CPdnWnd::get_top(VARIANT* retVal)
{
	RECT rc;
	GetWindowRect(&rc);
	retVal->intVal = rc.top;
	retVal->vt = VT_I4;
	return S_OK;
}
STDMETHODIMP CPdnWnd::get_width(VARIANT* retVal)
{
	RECT rc;
	GetClientRect(&rc);
	retVal->intVal = rc.right - rc.left;
	retVal->vt = VT_I4;
	return S_OK;
}
STDMETHODIMP CPdnWnd::get_height(VARIANT* retVal)
{
	RECT rc;
	GetClientRect(&rc);
	retVal->intVal = rc.bottom - rc.top;
	retVal->vt = VT_I4;
	return S_OK;
}
STDMETHODIMP CPdnWnd::put_minWidth(DWORD minWidth)
{
	m_minSize.x = minWidth;
	return S_OK;
}
STDMETHODIMP CPdnWnd::put_minHeight(DWORD minHeight)
{
	m_minSize.y = minHeight;
	return S_OK;
}

STDMETHODIMP CPdnWnd::isActive(BOOL *b)
{
	if (::GetForegroundWindow() == m_hWnd) 
		*b = TRUE;
	else 
		*b = FALSE;
	return S_OK;
}

LRESULT CPdnWnd::OnActivate (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (LOWORD(wParam) == WA_INACTIVE)
	{
		if (::GetFocus() && m_hWndLastFocusedWindow == NULL)
			m_hWndLastFocusedWindow = ::GetFocus();
	}
	else
	{
		::SetActiveWindow(::GetWindow(m_hWnd, GW_CHILD));
		if (m_hWndLastFocusedWindow)
			::SetFocus(m_hWndLastFocusedWindow);
		if(m_sActivationHandler)
            FireEvent(m_sActivationHandler, 0, 0);
	}
	return 0;
}
STDMETHODIMP CPdnWnd::pushFocus()
{
	m_hWndActiveWindow = ::GetActiveWindow();
	m_hWndFocus = ::GetFocus();
	return S_OK;
}

STDMETHODIMP CPdnWnd::popFocus()
{
	if (::IsWindow(m_hWndActiveWindow))
	{
		::SetActiveWindow(m_hWndActiveWindow);
		::SetFocus(m_hWndFocus);
		m_hWndFocus = NULL;
		m_hWndActiveWindow = NULL;
	}
	return S_OK;
}
//fd: start

//HRESULT CPdnWnd::OnEditCopy(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &fHandled)
//{
//	OutputDebugString("COPY\n");
//	return S_OK;
//}

STDMETHODIMP CPdnWnd::get_isMinimized(BOOL* pBool)
{
	*pBool = IsIconic();
	return S_OK;
}
STDMETHODIMP CPdnWnd::get_isMaximized(BOOL* pBool)
{
	*pBool = IsZoomed();
	return S_OK;
}
STDMETHODIMP CPdnWnd::get_isHidden(BOOL* pBool)
{
	*pBool = !(WS_VISIBLE & GetWindowLong(GWL_STYLE));
	return S_OK;
}
STDMETHODIMP CPdnWnd::setMinHandler(BSTR str)
{
	m_sMinHandler = str;
	return S_OK;
}
STDMETHODIMP CPdnWnd::setCloseHandler(BSTR str)
{
	m_sCloseHandler = str;
	return S_OK;
}
STDMETHODIMP CPdnWnd::setMenuHandler(BSTR str)
{
	m_sMenuHandler = str;
	return S_OK;
}
STDMETHODIMP CPdnWnd::setCmdLineHandler(BSTR str)
{
	m_sCmdLineHandler = str;
	return S_OK;
}
STDMETHODIMP CPdnWnd::setRestoreHandler(BSTR str)
{
	m_sRestoreHandler = str;
	return S_OK;
}
STDMETHODIMP CPdnWnd::setActivationHandler(BSTR str)
{
	m_sActivationHandler = str;
	return S_OK;
}
STDMETHODIMP CPdnWnd::FireEvent(BSTR handler, VARIANT* parameters, DWORD nParams)
{
	CComPtr<IHTMLDocument2> pHTDoc = NULL;
	HRESULT hr;
	hr = m_pBrowser->get_Document((IDispatch**) &pHTDoc);
	if(SUCCEEDED(hr))
	{
		CComPtr<IHTMLWindow2> pHTWnd = NULL;
		hr = pHTDoc->get_parentWindow(&pHTWnd);	
		if(SUCCEEDED(hr))
		{
			UINT errArg;
			DISPID dispid;

			hr = pHTWnd->GetIDsOfNames(IID_NULL, &handler, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
			if(SUCCEEDED(hr))
			{
				DISPPARAMS dParams = { parameters, 0, nParams, 0 };
                hr = pHTWnd->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dParams, 0, 0, &errArg);
				if(SUCCEEDED(hr))
				{
					return S_OK;
				}
			}
		}
	}
    return hr;
}
STDMETHODIMP CPdnWnd::get_menuBar(VARIANT* pMenu)
{
	pMenu->vt = VT_DISPATCH;
	m_pMenuBar->QueryInterface(IID_IDispatch, (void**) &pMenu->pdispVal);
	return S_OK;
}
STDMETHODIMP CPdnWnd::messageBox(BOOL modal, BSTR text, BSTR caption, DWORD type, DWORD* retval)
{
	*retval = ::MessageBoxW(modal ? m_hWnd : 0, text, caption, type);
	return S_OK;
}
STDMETHODIMP CPdnWnd::get_Handle(DWORD *pHandle)
{
	*pHandle = (DWORD) m_hWnd;
	return S_OK;
}
