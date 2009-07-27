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

class MainWnd : 
	public CPdnWnd
{
private:
	UINT m_uTaskbarRestart;
	CNotifyIcon* m_pNotIc;

public:
	MainWnd();
	~MainWnd();
	void Init(PdnModule* pModule);
	void GetNotifyIcon(VARIANT* pDisp);

	virtual LRESULT OnNotifyIcon(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnCopyData(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnTaskbarRestart(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnFinalMessage(HWND hWnd, UINT uMsg, WPARAM wParam,
		LPARAM lParam);

	// IPdnWnd
	STDMETHOD(close)();

	static HWND	GetMainWindow();
	static LPCWSTR GetMainWindowClassname();
};