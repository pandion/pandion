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
 * Filename:    PopupMenu.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#pragma once
#include "DispInterfaceImpl.h"

class CMainWnd;

class CPopupMenu :
	public DispInterfaceImpl<IPopupMenu>
{
private:
	HMENU m_Handle;
	DWORD m_Position;
	DWORD m_Choice;
public:
	CPopupMenu();
	~CPopupMenu();

	/* IPopupMenu */
	STDMETHOD(AddItem)(BOOL bEnabled, BOOL bChecked, BOOL bRadio, BOOL bDefault, DWORD hSubMenu, BSTR label, DWORD ID);
	STDMETHOD(Show)(DWORD x, DWORD y, BOOL fromTray = FALSE);
	STDMETHOD(get_Handle)(VARIANT* pVal);
	STDMETHOD(get_Choice)(VARIANT* pVal);
	STDMETHOD(Clear)();
	STDMETHOD(AddSeparator)();
};