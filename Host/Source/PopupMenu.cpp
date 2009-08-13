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
 * Filename:    PopupMenu.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "MainWnd.h"
#include "PopupMenu.h"

CPopupMenu::CPopupMenu()
{
	m_Handle = CreatePopupMenu();
	m_Position = 1;
	m_Choice = 0;
}
CPopupMenu::~CPopupMenu()
{
}

STDMETHODIMP CPopupMenu::AddItem(BOOL bEnabled, BOOL bChecked, BOOL bRadio, BOOL bDefault,
	DWORD hSubMenu, BSTR label, DWORD ID)
{
	MENUITEMINFO nfo;
	nfo.cbSize     = sizeof(MENUITEMINFO);
	nfo.fMask      = MIIM_STATE | MIIM_TYPE | MIIM_ID |
					 (hSubMenu ? MIIM_SUBMENU : 0);
	nfo.fState     = (bEnabled ? MFS_ENABLED : MFS_DISABLED) | 
					 ((bRadio || bChecked) ? MFS_CHECKED : MFS_UNCHECKED) |
					 (bDefault ? MFS_DEFAULT : 0);
	nfo.fType      = MFT_STRING | 
			         (bRadio ? MFT_RADIOCHECK : 0);
	nfo.wID        = ID;
	nfo.hSubMenu   = (HMENU) hSubMenu;
	nfo.cch        = wcslen(label) + 1;
	nfo.dwTypeData = label;

	return InsertMenuItem(m_Handle, m_Position++, true, &nfo) ? 
		S_OK : GetLastError();
}

/* Allow the TPM_LAYOUTRTL flag for Windows 2000 */
#if(_WIN32_WINNT == 0x0500)
	#define TPM_LAYOUTRTL       0x8000L
#endif /* _WIN32_WINNT >= 0x0500 */

STDMETHODIMP CPopupMenu::Show(DWORD x, DWORD y, BOOL rightToLeft, BOOL fromTray)
{
	if(fromTray)
		SetForegroundWindow(MainWnd::GetMainWindow());
	m_Choice = TrackPopupMenu(m_Handle,
		TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD |
		TPM_RIGHTBUTTON | TPM_NONOTIFY |
		(rightToLeft ? TPM_LAYOUTRTL : 0x0L),
		x, y, NULL, MainWnd::GetMainWindow(), NULL);
	if(fromTray)
		PostMessage(MainWnd::GetMainWindow(), WM_NULL, 0, 0);

	DestroyMenu(m_Handle);
	if(m_Choice)
		return S_OK;
	return S_FALSE;
}

STDMETHODIMP CPopupMenu::Clear()
{
	DestroyMenu(m_Handle);
	m_Handle = CreatePopupMenu();
	m_Position = 0;
	m_Choice = 0;

	return S_OK;
}

STDMETHODIMP CPopupMenu::AddSeparator()
{
	MENUITEMINFOW nfo;
	nfo.cbSize = sizeof(MENUITEMINFO);
	nfo.fMask = MIIM_TYPE;
	nfo.fType = MFT_SEPARATOR;

	if(InsertMenuItem(m_Handle, m_Position, TRUE, &nfo))
	{
		m_Position++;
		return S_OK;
	}
	else 
		return S_FALSE;
}

STDMETHODIMP CPopupMenu::get_Handle(VARIANT* pVal)
{
	pVal->uintVal = (long)m_Handle;
	pVal->vt = VT_UI4;
	return S_OK;
}

STDMETHODIMP CPopupMenu::get_Choice(VARIANT* pVal)
{
	pVal->intVal = (long)m_Choice;
	pVal->vt = VT_I4;
	return S_OK;
}