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
 * Filename:    PdnWnd.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#pragma once

class CExternal;
class CPandionModule;

class CCreateParams
{
public:
	CComBSTR m_name;
	CComBSTR m_url;
	VARIANT *m_pParams;
	CPandionModule *m_pModule;
	CCreateParams( BSTR name, BSTR url, VARIANT* pParams, CPandionModule* pModule );
};


static _ATL_FUNC_INFO OnWindowClosingInfo = { CC_STDCALL, VT_EMPTY, 2, { VT_BOOL, VT_BYREF } };
static _ATL_FUNC_INFO OnNavigateComplete2Info = { CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH, VT_VARIANT } };


class CPdnWnd :
	public CComObjectRootEx< CComSingleThreadModel >,
	public CWindowImpl< CPdnWnd, CAxWindow, CFrameWinTraits >,
	public IDispatchImpl< IPdnWnd >,
	public IDispEventSimpleImpl< 0, CPdnWnd, &__uuidof(SHDocVw::DWebBrowserEvents2) >,
	public IDocHostUIHandler,
	public IDocHostShowUI,
	public IInternetSecurityManager,
	public IServiceProviderImpl< CPdnWnd >
{
protected:
	_CrtMemState state;
	HWND	m_hWndFocus;
	HWND	m_hWndActiveWindow;
	HWND	m_hWndLastFocusedWindow;

	BOOL	m_bPopUnder;

	CComBSTR m_sMinHandler;
	CComBSTR m_sCloseHandler;
	CComBSTR m_sMenuHandler;
	CComBSTR m_sCmdLineHandler;
	CComBSTR m_sRestoreHandler;
	CComBSTR m_sActivationHandler;
	CComBSTR m_sName;
	CComBSTR m_sURL;

	CComVariant m_vParams;
	POINT		m_minSize;

    CPandionModule       *m_pModule;
	IWebBrowser2         *m_pBrowser;
	IExternal            *m_pExternal;
	IMenuBar             *m_pMenuBar;

	IInternetSecurityManager* m_pSecurityMgr;
public:
	CPdnWnd();
	~CPdnWnd();

	DECLARE_NO_REGISTRY()
	DECLARE_NOT_AGGREGATABLE( CPdnWnd );

	BEGIN_COM_MAP( CPdnWnd )
		COM_INTERFACE_ENTRY( IPdnWnd )
        COM_INTERFACE_ENTRY( IDispatch )
		COM_INTERFACE_ENTRY( IDocHostUIHandler )
		COM_INTERFACE_ENTRY( IServiceProvider )
		COM_INTERFACE_ENTRY( IInternetSecurityManager )
	END_COM_MAP()

	void ChangeIcon( int icon );
	void PopUnder( BOOL b );
	HRESULT ContainerCreate();

	// Message Handlers
	BEGIN_MSG_MAP(CPdnWnd)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_JSINVOKE, OnJSInvoke)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_ENDSESSION, OnEndSession)
	END_MSG_MAP()

	/* Handler prototypes:
	 *  LRESULT MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	 *  LRESULT CommandHandler( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	 *  LRESULT NotifyHandler( int idCtrl, LPNMHDR pnmh, BOOL& bHandled );
	 */

    LRESULT OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnClose( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnJSInvoke( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnGetMinMaxInfo( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnActivate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnCommand( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnEndSession( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

	// Webbrowser Events
	BEGIN_SINK_MAP(CPdnWnd)
		SINK_ENTRY_INFO(0, __uuidof(SHDocVw::DWebBrowserEvents2), DISPID_WINDOWCLOSING, OnWindowClosing, &OnWindowClosingInfo )
		SINK_ENTRY_INFO(0, __uuidof(SHDocVw::DWebBrowserEvents2), DISPID_NAVIGATECOMPLETE2, OnNavigateComplete2, &OnNavigateComplete2Info )
	END_SINK_MAP()

	STDMETHOD(OnWindowClosing)( VARIANT_BOOL IsChildWindow, VARIANT_BOOL* &Cancel );
	STDMETHOD(OnNavigateComplete2)( IDispatch* pDisp, VARIANT URL );

	// IDocHostUIHandler implementation
    STDMETHOD(ShowContextMenu)( DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved );
    STDMETHOD(GetHostInfo)( DOCHOSTUIINFO *pInfo );
    STDMETHOD(ShowUI)( DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc );
	STDMETHOD(HideUI)();
    STDMETHOD(UpdateUI)();
    STDMETHOD(EnableModeless)( BOOL fEnable );
    STDMETHOD(OnDocWindowActivate)( BOOL fActivate );
    STDMETHOD(OnFrameWindowActivate)( BOOL fActivate );
    STDMETHOD(ResizeBorder)( LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow );
    STDMETHOD(TranslateAccelerator)( LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID );
    STDMETHOD(GetOptionKeyPath)( LPOLESTR *pchKey, DWORD dw );
	STDMETHOD(GetDropTarget)( IDropTarget *pDropTarget, IDropTarget **ppDropTarget );
    STDMETHOD(GetExternal)( IDispatch **ppDispatch );
    STDMETHOD(TranslateUrl)( DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut );
    STDMETHOD(FilterDataObject)( IDataObject *pDO, IDataObject **ppDORet );

	// IDocHostShowUI implementation
	STDMETHOD(ShowMessage)( HWND hwnd, LPOLESTR lpstrText,
		LPOLESTR lpstrCaption, DWORD dwType, LPOLESTR lpstrHelpFile,
		DWORD dwHelpContext, LRESULT *plResult);
	STDMETHOD(ShowHelp)(HWND hwnd, LPOLESTR pszHelpFile,
		UINT uCommand, DWORD dwData, POINT ptMouse,
		IDispatch *pDispatchObjectHit);
        


	// IServiceProviderImpl
	BEGIN_SERVICE_MAP( CRBSite )
		SERVICE_ENTRY( SID_SInternetSecurityManager )
	END_SERVICE_MAP()

	// IInternetSecurityManager implementation
	STDMETHOD(SetSecuritySite)( IInternetSecurityMgrSite *pSite );
	STDMETHOD(GetSecuritySite)( IInternetSecurityMgrSite **ppSite );
	STDMETHOD(MapUrlToZone)( LPCWSTR pwszUrl, DWORD* pdwZone, DWORD dwFlags );
	STDMETHOD(GetSecurityId)( LPCWSTR pwszUrl, BYTE* pbSecurityId, DWORD* pcbSecurityId, DWORD_PTR dwReserved );
	STDMETHOD(ProcessUrlAction)( LPCWSTR pwszUrl, DWORD dwAction, BYTE* pPolicy, DWORD cbPolicy, BYTE* pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved );
	STDMETHOD(QueryCustomPolicy)( LPCWSTR pwszUrl, REFGUID guidKey, BYTE** ppPolicy, DWORD* pcbPolicy, BYTE* pContext, DWORD cbContext, DWORD dwReserved );
	STDMETHOD(SetZoneMapping)( DWORD dwZone, LPCWSTR lpszPattern, DWORD dwFlags );
	STDMETHOD(GetZoneMappings)( DWORD dwZone, IEnumString  **ppenumString, DWORD dwFlags );

	// IPdnWnd
	STDMETHOD(setTitle)( BSTR );
	STDMETHOD(setAOT)( BOOL );
	STDMETHOD(minimize)();
	STDMETHOD(maximize)();
	STDMETHOD(restore)();
	STDMETHOD(close)();
	void OnFinalMessage( HWND );
	STDMETHOD(setSize)( DWORD, DWORD );
	STDMETHOD(setPos)( int, int );
	STDMETHOD(setIcon)( BSTR );
	STDMETHOD(hide)( BOOL );
	STDMETHOD(flash)( DWORD );
	STDMETHOD(resizeable)( BOOL );
	STDMETHOD(showMinBox)( BOOL );
	STDMETHOD(showMaxBox)( BOOL );
	STDMETHOD(showTitlebar)( BOOL );
	STDMETHOD(translucent)( DWORD );
	STDMETHOD(animate)( DWORD, DWORD );
	STDMETHOD(Do)( BSTR, VARIANT[], DWORD = 1 );
	STDMETHOD(get_params)( VARIANT* );
	STDMETHOD(focus)();
	STDMETHOD(get_left)( VARIANT* );
	STDMETHOD(get_top)( VARIANT* );
	STDMETHOD(get_width)( VARIANT* );
	STDMETHOD(get_height)( VARIANT* );
	STDMETHOD(put_minWidth)( DWORD );
	STDMETHOD(put_minHeight)( DWORD );
	STDMETHOD(isActive)( BOOL* );
	STDMETHOD(get_isMinimized)( BOOL * );
	STDMETHOD(get_isMaximized)( BOOL * );
	STDMETHOD(get_isHidden)( BOOL * );
	STDMETHOD(pushFocus)();
	STDMETHOD(popFocus)();
	STDMETHOD(setMinHandler)( BSTR );
	STDMETHOD(setCloseHandler)( BSTR );
	STDMETHOD(setMenuHandler)( BSTR );
	STDMETHOD(setCmdLineHandler)( BSTR );
	STDMETHOD(setRestoreHandler)( BSTR );
	STDMETHOD(setActivationHandler)( BSTR );
	STDMETHOD(FireEvent)( BSTR , VARIANT[], DWORD );
	STDMETHOD(get_menuBar)( VARIANT* );
	STDMETHOD(messageBox)( BOOL, BSTR, BSTR, DWORD, DWORD* );
	STDMETHOD(get_Handle)( DWORD *pHandle );
};