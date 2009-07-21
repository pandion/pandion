#pragma once
#include "PdnWnd.h"

class CNotifyIcon;

class CMainWnd : 
	public CPdnWnd
{
private:
	UINT m_uTaskbarRestart;
	CNotifyIcon* m_pNotIc;

public:
	CMainWnd();
	~CMainWnd();
	void Init( CPandionModule* pModule );
	void GetNotifyIcon( VARIANT* pDisp );

	BEGIN_MSG_MAP( CMainWnd )
		MESSAGE_HANDLER( WM_CREATE, OnCreate )
		MESSAGE_HANDLER( WM_NOTIFYICON, OnNotifyIcon )
		MESSAGE_HANDLER( WM_COPYDATA, OnCopyData )
		MESSAGE_HANDLER( m_uTaskbarRestart, OnTaskbarRestart );
		MESSAGE_HANDLER( WM_CLOSE, OnClose );
		CHAIN_MSG_MAP( CPdnWnd )
	END_MSG_MAP()

	LRESULT OnNotifyIcon( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnCopyData( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnTaskbarRestart( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnClose( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

	// IPdnWnd
	STDMETHOD(close)();

	static HWND GetMainWindow();
	static ATL::CWndClassInfo& GetWndClassInfo();
private:
	static LPTSTR GetWndClassName();
};