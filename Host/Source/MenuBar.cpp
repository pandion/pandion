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
 * Filename:    MenuBar.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "menubar.h"

CMenuBar::CMenuBar() : m_hWnd(0)
{
	m_Handle = CreateMenu();
}
CMenuBar::~CMenuBar()
{
	DestroyMenu(m_Handle);
}

STDMETHODIMP CMenuBar::AddItem(BSTR label, DWORD pos, DWORD ID, IDispatch *subMenu)
{
	if(!subMenu) return S_FALSE;

	IPopupMenu* pPopupMenu = NULL;
	subMenu->QueryInterface(__uuidof(IPopupMenu), (void**) &pPopupMenu);

	VARIANT subHandle;
	VariantInit(&subHandle);
	pPopupMenu->get_Handle(&subHandle);

	pPopupMenu->Release();


	MENUITEMINFOW nfo;
	nfo.cbSize      = sizeof(MENUITEMINFO);
	nfo.fMask       = MIIM_TYPE | MIIM_ID | MIIM_SUBMENU;
	nfo.fType       = MFT_STRING;
	nfo.wID         = ID;
	nfo.hSubMenu    = (HMENU)subHandle.uintVal;
	nfo.cch         = wcslen(label) + 1;
	nfo.dwTypeData  = label;

	return InsertMenuItem(m_Handle, pos, true, &nfo) ? S_OK : GetLastError();
}
STDMETHODIMP CMenuBar::RemItem(DWORD ID)
{
	DeleteMenu(m_Handle, ID, MF_BYCOMMAND);

	return S_OK;
}
STDMETHODIMP CMenuBar::Update()
{
	if(DrawMenuBar(m_hWnd))
		return S_OK;
	else
		return GetLastError();
}
STDMETHODIMP CMenuBar::get_Items(DWORD* nItems)
{
	*nItems = GetMenuItemCount(m_Handle);
	return S_OK;
}
STDMETHODIMP CMenuBar::get_Handle(VARIANT* pHandle)
{
	pHandle->vt = VT_UI4;
	pHandle->uintVal = (UINT) m_Handle;
	return S_OK;
}
STDMETHODIMP CMenuBar::put_hwnd(DWORD hwnd)
{
	if(!m_hWnd)
        m_hWnd = (HWND)hwnd;
	return S_OK;
}