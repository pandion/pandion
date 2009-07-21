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
 * Filename:    MainWnd.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
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