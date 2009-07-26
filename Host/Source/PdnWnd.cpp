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
	m_bPopUnder(false), m_External(*this), 
	m_ActiveXHost(dynamic_cast<IPdnWnd*>(this))
{
	m_COMCannotSelfDelete = false;
	m_COMReferenceCounter = 0;

	m_WindowClass.cbSize = sizeof(WNDCLASSEX);
	m_WindowClass.style = CS_DBLCLKS;
	m_WindowClass.lpfnWndProc = StartWindowProc;
	m_WindowClass.cbClsExtra = 0;
	m_WindowClass.cbWndExtra = 0;
	m_WindowClass.hInstance = ::GetModuleHandle(NULL);
	m_WindowClass.hIcon = NULL;
	m_WindowClass.hCursor = NULL;
	m_WindowClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE);
	m_WindowClass.lpszMenuName = NULL;
	m_WindowClass.lpszClassName = L"Pandion Window";
	m_WindowClass.hIconSm = NULL;
	m_hWnd = NULL;
	m_hWndFocus = NULL;
	m_hWndActiveWindow = NULL;
	m_hWndLastFocusedWindow = NULL;

	m_TaskbarRestartMessage = -1;

	m_External.DisableSelfDelete();
	m_minSize.x  = 160;
	m_minSize.y  = 200;
	_CrtMemCheckpoint(&state);
}
CPdnWnd::~CPdnWnd()
{
	_CrtMemDumpAllObjectsSince(&state);
}

int CPdnWnd::Create(RECT& rect, std::wstring Name, std::wstring URL,
	_variant_t& windowParams, PdnModule* Module)
{
	m_Name          = Name;
	m_URL           = URL;
	m_windowParams  = windowParams;
	m_Module        = Module;

	::RegisterClassEx(&m_WindowClass);
	m_hWnd = ::CreateWindowEx(0, m_WindowClass.lpszClassName, L"", 
		WS_OVERLAPPEDWINDOW,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		0, 0, GetModuleHandle(NULL), this);

	/* Init COM in case window is created from a different thread */
	::OleInitialize(NULL);

	/* Create Internet Security Manager Object */
	CoCreateInstance(CLSID_InternetSecurityManager, NULL, CLSCTX_INPROC_SERVER, IID_IInternetSecurityManager, (void**)&m_pSecurityMgr);
	m_pSecurityMgr->AddRef();

	/* Create the External object */
	m_External.Init(m_Module);

	/* Create the menu bar */
	(new CMenuBar)->QueryInterface(__uuidof(IMenuBar), (LPVOID*)&m_pMenuBar);
	VARIANT handle;
	m_pMenuBar->get_Handle(&handle);
	::SetMenu(m_hWnd, (HMENU)handle.uintVal);
	m_pMenuBar->put_hwnd((DWORD)m_hWnd);

	/* Add the window to the global window dictionary */
	ScrRun::IDictionaryPtr pWindows = m_Module->GetWindows();
	pWindows->Add(&_variant_t(m_Name.c_str()),
		&_variant_t(dynamic_cast<IDispatch*>(this)));

	/* Create the Internet Explorer control */
	ContainerCreate();

	return 0;
}

LRESULT CALLBACK CPdnWnd::StartWindowProc(HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_CREATE)
	{
		CREATESTRUCT* cs = (CREATESTRUCT*) lParam;
		::SetWindowLong(hWnd, GWL_USERDATA, (LONG) cs->lpCreateParams);
	}
	CPdnWnd *pandionWindow = (CPdnWnd*) ::GetWindowLong(hWnd, GWL_USERDATA);
	if(pandionWindow != NULL)
	{
		return pandionWindow->WindowProc(hWnd, uMsg, wParam, lParam);
	}
	else
	{
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT CPdnWnd::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bHandled;
	if(uMsg == WM_CREATE)
	{
		m_hWnd = hWnd;
		return OnCreate(uMsg, wParam, lParam, bHandled);
	}
	else if(uMsg == WM_CLOSE)
	{
		return OnClose(uMsg, wParam, lParam, bHandled);
	}
	else if(uMsg == WM_JSINVOKE)
	{
		return OnJSInvoke(uMsg, wParam, lParam, bHandled);
	}
	else if(uMsg == WM_GETMINMAXINFO)
	{
		return OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled);
	}
	else if(uMsg == WM_SIZE)
	{
		OnSize(uMsg, wParam, lParam, bHandled);
		m_ActiveXHost.OnSize(uMsg, wParam, lParam);
		return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	}
	else if(uMsg == WM_COMMAND)
	{
		return OnCommand(uMsg, wParam, lParam, bHandled);
	}
	else if(uMsg == WM_ENDSESSION)
	{
		return OnEndSession(uMsg, wParam, lParam, bHandled);
	}
	else if(uMsg == WM_NCDESTROY)
	{
		OnFinalMessage(m_hWnd);
		return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	}
	else if(uMsg == m_TaskbarRestartMessage)
	{
		return OnTaskbarRestart(uMsg, wParam, lParam, bHandled);
	}
	else if(uMsg == WM_COPYDATA)
	{
		return OnCopyData(uMsg, wParam, lParam, bHandled);
	}
	else if(uMsg == WM_NOTIFYICON)
	{
		return OnNotifyIcon(uMsg, wParam, lParam, bHandled);
	}
	else
	{
		return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	}
}
void CPdnWnd::OnFinalMessage(HWND hWnd)
{
	/* Remove the window from the global window dictionary */
	ScrRun::IDictionaryPtr pWindows = m_Module->GetWindows();
	pWindows->Remove(&_variant_t(m_Name.c_str()));

	/* free the browser reference */
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
	return 0;
}
HRESULT CPdnWnd::ContainerCreate()
{
	// Create the control
	IOleObject* activeXControl = 
		m_ActiveXHost.Create(m_hWnd, L"Shell.Explorer.2");

	// Get the webbrowser control
	HRESULT hr = activeXControl->QueryInterface(IID_IWebBrowser2,
		(void**) &m_pBrowser);
	if(FAILED(hr))
		return hr;

	// initialize the browser
	m_pBrowser->put_RegisterAsBrowser(VARIANT_FALSE);
	m_pBrowser->put_RegisterAsDropTarget(VARIANT_TRUE);
	m_pBrowser->put_Silent(VARIANT_FALSE);
	m_pBrowser->Navigate(_bstr_t(m_URL.c_str()), 0, 0, 0, 0);

	return S_OK;
}

LRESULT CPdnWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_sCloseHandler.length())
	{
		FireEvent(m_sCloseHandler, 0, 0);
		::DestroyWindow(m_hWnd);
	}
	else
	{
		::DestroyWindow(m_hWnd);
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
	::GetWindowRect(m_hWnd, &windowRect);
	::GetClientRect(m_hWnd, &clientRect);
	DWORD deltaWidth  = (windowRect.right - windowRect.left) - (clientRect.right - clientRect.left);
	DWORD deltaHeight = (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top);

	((MINMAXINFO*)lParam)->ptMinTrackSize.x = m_minSize.x /*+ deltaWidth*/ + deltaHeight - 
		GetSystemMetrics(SM_CYCAPTION);
	((MINMAXINFO*)lParam)->ptMinTrackSize.y = m_minSize.y + deltaHeight;
	return 0;
}
LRESULT CPdnWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rect;
	::GetClientRect(m_hWnd, &rect);
	m_ActiveXHost.SetWindowPos(HWND_TOP, rect.left, rect.top, 
		rect.right - rect.left, rect.bottom - rect.top, 0);
	if(wParam == SIZE_MINIMIZED && m_sMinHandler.length())
	{
        FireEvent(m_sMinHandler, 0, 0);
		return 0;
	}
	else if(wParam == SIZE_RESTORED && m_sRestoreHandler.length())
	{
		FireEvent(m_sRestoreHandler, 0, 0);
		return 0;
	}
	else
	{
		return 1;
	}
}
LRESULT CPdnWnd::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	VARIANT parameters[1];
	parameters[0].vt      = VT_UI4;
	parameters[0].uintVal = wParam;

	if(m_sMenuHandler.length())
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
LRESULT CPdnWnd::OnNotifyIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 1;
}
LRESULT CPdnWnd::OnCopyData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 1;
}
LRESULT CPdnWnd::OnTaskbarRestart(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 1;
}

// DWebBrowserEvents2 Implementation
STDMETHODIMP_(void) CPdnWnd::WindowClosing(VARIANT_BOOL IsChildWindow,
										   VARIANT_BOOL* Cancel)
{
	Cancel = new VARIANT_BOOL;
	*Cancel = false;
	close();
}
STDMETHODIMP_(void) CPdnWnd::NavigateComplete2(IDispatch* pDisp, VARIANT* URL)
{
	IDispatch* pDocDisp;
	m_pBrowser->get_Document(&pDocDisp);
	ICustomDoc* pCustomDoc;
	pDocDisp->QueryInterface(IID_ICustomDoc, (LPVOID*) &pCustomDoc);
	pDocDisp->Release();
	pCustomDoc->SetUIHandler(this);
	pCustomDoc->Release();
}
/* IDocHostUIHandler */
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
/* IDocHostShowUI */
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

/* IServiceProvider */
STDMETHODIMP CPdnWnd::QueryService(REFGUID guidService, REFIID riid, void **ppv)
{
	*ppv = this;
	return S_OK;
}

/* IInternetSecurityManager */
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

/* IPdnWnd */
STDMETHODIMP CPdnWnd::setTitle(BSTR Title)
{
	::SetWindowText(m_hWnd, Title);

	return S_OK;
}
STDMETHODIMP CPdnWnd::setAOT(BOOL b)
{
	::SetWindowPos(m_hWnd, b ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOACTIVATE);
	return S_OK;
}
STDMETHODIMP CPdnWnd::minimize()
{
	::ShowWindow(m_hWnd, SW_MINIMIZE);
	return S_OK;
}
STDMETHODIMP CPdnWnd::maximize()
{
	::ShowWindow(m_hWnd, SW_MAXIMIZE);
	return S_OK;
}
STDMETHODIMP CPdnWnd::restore()
{
	::ShowWindow(m_hWnd, SW_RESTORE);
	return S_OK;
}
STDMETHODIMP CPdnWnd::close()
{
	::PostMessage(m_hWnd, WM_CLOSE, 0, 0);

	return S_OK;
}

STDMETHODIMP CPdnWnd::setSize(DWORD width, DWORD height)
{
	RECT windowRect, clientRect;
	::GetWindowRect(m_hWnd, &windowRect);
	::GetClientRect(m_hWnd, &clientRect);
	DWORD deltaWidth  = (windowRect.right - windowRect.left) - (clientRect.right - clientRect.left);
	DWORD deltaHeight = (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top);
	windowRect.right = windowRect.left + width + deltaWidth;
	windowRect.bottom = windowRect.top + height + deltaHeight;

	::SetWindowPos(m_hWnd, 0, windowRect.left, windowRect.top,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	return S_OK;
}
STDMETHODIMP CPdnWnd::setPos(int left, int top)
{
	::SetWindowPos(m_hWnd, 0, left, top, 0, 0, 
		SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
	return S_OK;
}
STDMETHODIMP CPdnWnd::setIcon(BSTR IconPath)
{
	HICON hIcon16 = (HICON) ::LoadImage(0, IconPath, IMAGE_ICON, 16, 16,
		LR_LOADFROMFILE | LR_DEFAULTCOLOR);
	HICON hIcon32 = (HICON) ::LoadImage(0, IconPath, IMAGE_ICON, 32, 32,
		LR_LOADFROMFILE | LR_DEFAULTCOLOR);

	if(!hIcon16 || !hIcon32)
		return S_FALSE;

	::PostMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM) hIcon32);
	::PostMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM) hIcon16);

	return S_OK;
}
STDMETHODIMP CPdnWnd::hide(BOOL b)
{
	ShowWindow(m_hWnd, b ? SW_HIDE : SW_SHOWNA);
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
	HWND FGWindow = ::GetForegroundWindow();
	if(FGWindow == m_hWnd)
		return S_FALSE;

	FLASHWINFO fwi;
	fwi.cbSize = sizeof(FLASHWINFO);
	fwi.hwnd = m_hWnd;
	fwi.dwTimeout = 0;
	fwi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
    fwi.uCount = u;

	::FlashWindowEx(&fwi);

	::SetForegroundWindow(FGWindow);
	
	return S_OK;
}
STDMETHODIMP CPdnWnd::resizeable(BOOL b)
{
	if(!b)
	{
		::SetWindowLong(m_hWnd, GWL_STYLE, ::GetWindowLong(m_hWnd, GWL_STYLE) &
			!(WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX) |
				WS_CAPTION | WS_SYSMENU );
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, ::GetWindowLong(m_hWnd, GWL_EXSTYLE) | 
			WS_EX_WINDOWEDGE);
	}
	else
	{
        ::SetWindowLong(m_hWnd, GWL_STYLE, ::GetWindowLong(m_hWnd, GWL_STYLE) |
			WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION | WS_SYSMENU);
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, ::GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_WINDOWEDGE);
	}
	
	::SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
	return S_OK;

}
STDMETHODIMP CPdnWnd::showMinBox(BOOL b)
{
	if(!b)
        ::SetWindowLong(m_hWnd, GWL_STYLE, ::GetWindowLong(m_hWnd, GWL_STYLE) & !WS_MINIMIZEBOX);
	else
		::SetWindowLong(m_hWnd, GWL_STYLE, ::GetWindowLong(m_hWnd, GWL_STYLE) | WS_MINIMIZEBOX);

	return S_OK;
}
STDMETHODIMP CPdnWnd::showMaxBox(BOOL b)
{
	return E_NOTIMPL;
}
STDMETHODIMP CPdnWnd::showTitlebar(BOOL b)
{
	RECT rc;
	::GetClientRect(m_hWnd, &rc);
	if(!b)
	{
		::SetWindowLong(m_hWnd, GWL_STYLE, ::GetWindowLong(m_hWnd, GWL_STYLE) & !(WS_OVERLAPPEDWINDOW));
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, (::GetWindowLong(m_hWnd, GWL_EXSTYLE) &
			!(WS_EX_APPWINDOW)) | WS_EX_TOOLWINDOW);
	}
	else
	{
		::SetWindowLong(m_hWnd, GWL_STYLE, ::GetWindowLong(m_hWnd, GWL_STYLE) | (WS_OVERLAPPEDWINDOW));
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, (::GetWindowLong(m_hWnd, GWL_EXSTYLE) &
			!(WS_EX_TOOLWINDOW)) | WS_EX_APPWINDOW);
	}
	
	::SetWindowPos(m_hWnd, 0, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
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
				::SetWindowLong(m_hWnd, GWL_EXSTYLE, ::GetWindowLong(m_hWnd, GWL_EXSTYLE) & !WS_EX_LAYERED);
			}
			else
			{
				::SetWindowLong(m_hWnd, GWL_EXSTYLE, ::GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
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
		::PostMessage(m_hWnd, WM_JSINVOKE, (WPARAM)jsi, (LPARAM)par);
	}
	return S_OK;
}
STDMETHODIMP CPdnWnd::get_params(VARIANT* retVal)
{
	::VariantCopy(retVal, &m_windowParams.GetVARIANT());
	return S_OK;
}
STDMETHODIMP CPdnWnd::focus()
{
	return 	SetForegroundWindow(m_hWnd) ? S_OK : S_FALSE;
}
STDMETHODIMP CPdnWnd::get_left(VARIANT* retVal)
{
	RECT rc;
	::GetWindowRect(m_hWnd, &rc);
	retVal->intVal = rc.left;
	retVal->vt = VT_I4;
	return S_OK;
}
STDMETHODIMP CPdnWnd::get_top(VARIANT* retVal)
{
	RECT rc;
	::GetWindowRect(m_hWnd, &rc);
	retVal->intVal = rc.top;
	retVal->vt = VT_I4;
	return S_OK;
}
STDMETHODIMP CPdnWnd::get_width(VARIANT* retVal)
{
	RECT rc;
	::GetClientRect(m_hWnd, &rc);
	retVal->intVal = rc.right - rc.left;
	retVal->vt = VT_I4;
	return S_OK;
}
STDMETHODIMP CPdnWnd::get_height(VARIANT* retVal)
{
	RECT rc;
	::GetClientRect(m_hWnd, &rc);
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
		if(m_sActivationHandler.length())
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
	*pBool = ::IsIconic(m_hWnd);
	return S_OK;
}
STDMETHODIMP CPdnWnd::get_isMaximized(BOOL* pBool)
{
	*pBool = ::IsZoomed(m_hWnd);
	return S_OK;
}
STDMETHODIMP CPdnWnd::get_isHidden(BOOL* pBool)
{
	*pBool = !(WS_VISIBLE & ::GetWindowLong(m_hWnd, GWL_STYLE));
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
	IHTMLDocument2* pHTDoc;
	HRESULT hr;
	hr = m_pBrowser->get_Document((IDispatch**) &pHTDoc);
	if(SUCCEEDED(hr))
	{
		IHTMLWindow2* pHTWnd = NULL;
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
					pHTWnd->Release();
					pHTDoc->Release();
					return S_OK;
				}
			}
			pHTWnd->Release();
		}
		pHTDoc->Release();
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

/*
 * IUnknown::QueryInterface() implementation
 */
STDMETHODIMP CPdnWnd::QueryInterface(REFIID riid, void** ppvObject)
{
	if(ppvObject == NULL)
		return E_POINTER;
	else
		*ppvObject = NULL;
	
	if(::IsEqualGUID(riid, IID_IUnknown))
		*ppvObject = dynamic_cast<IPdnWnd*>(this);
	else if(::IsEqualGUID(riid, IID_IDispatch))
		*ppvObject = dynamic_cast<IDispatch*>(this);
	else if(::IsEqualGUID(riid, __uuidof(IPdnWnd)))
		*ppvObject = dynamic_cast<IPdnWnd*>(this);
	else if(::IsEqualGUID(riid, IID_IServiceProvider))
		return E_NOINTERFACE;
	else if(::IsEqualGUID(riid, IID_IDocHostUIHandler))
		*ppvObject = dynamic_cast<IDocHostUIHandler*>(this);
	else if(::IsEqualGUID(riid, IID_IDocHostShowUI))
		*ppvObject = dynamic_cast<IDocHostShowUI*>(this);
	else
		return E_NOINTERFACE;

	AddRef();
	return S_OK;
}

/*
 * IUnknown::AddRef() implementation
 */
STDMETHODIMP_(ULONG) CPdnWnd::AddRef()
{
	return ++m_COMReferenceCounter;
}

/*
 * IUnknown::Release() implementation
 */
STDMETHODIMP_(ULONG) CPdnWnd::Release()
{
	if(m_COMReferenceCounter> 1)
	{
		return --m_COMReferenceCounter;
	}
	else
	{
		if(!m_COMCannotSelfDelete)
		{
			delete this;
		}
		return 0;
	}
}
//
///*
// * IDispatch::GetTypeInfoCount() implementation
// */
//STDMETHODIMP CPdnWnd::GetTypeInfoCount(UINT* pctinfo)
//{
//	if(pctinfo == NULL)
//	{
//		return E_POINTER;
//	}
//	*pctinfo = 1;
//	return S_OK;
//}
//
///*
// * IDispatch::GetTypeInfo() implementation
// */
//STDMETHODIMP CPdnWnd::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
//{
//	if(ppTInfo == NULL)
//	{
//		return E_POINTER;
//	}
//	m_TypeInfo->AddRef();
//	*ppTInfo = m_TypeInfo;
//	return S_OK;
//}
//
///*
// * IDispatch::GetIDsOfNames() implementation
// */
//STDMETHODIMP CPdnWnd::GetIDsOfNames(REFIID riid,
//	LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
//{
//	return DispGetIDsOfNames(m_TypeInfo, rgszNames, cNames, rgDispId);
//}
//
///*
// * IDispatch::Invoke() implementation
// */
//STDMETHODIMP CPdnWnd::Invoke(
//	DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, 
//	DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, 
//	UINT* puArgErr)
//{
//	HRESULT hr = DispInvoke(this, m_TypeInfo, dispidMember, wFlags,
//		pDispParams, pVarResult, pExcepInfo, puArgErr);
//	return hr;
//}