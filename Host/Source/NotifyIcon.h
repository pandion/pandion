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
 * Filename:    NotifyIcon.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#pragma once

class CIcon
{
	HICON m_hIcon;
public:
	CIcon();
	CIcon(BSTR strIcon, int nIndex);
	CIcon(CIcon &);
	~CIcon();

	HICON getHandle();
};

class CNotifyIcon :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<INotifyIcon>
{
	/* Describes the owner */
	HWND	m_hWnd;
	UINT	m_CBMsg;

	/* Describes the icon */
	CComBSTR m_text;
	CAtlMap<CComBSTR,CIcon>	 m_icons;
	CComBSTR                 m_currentIcon;

	/* The callback function to send events to the JScript */
    CComBSTR m_handler;
public:
	/* Do not register this class in the registry */
	DECLARE_NO_REGISTRY()

	/* QueryInterface implementation for INotifyIcon and IDispatch */
	BEGIN_COM_MAP(CNotifyIcon)
		COM_INTERFACE_ENTRY2(IDispatch, INotifyIcon)
	END_COM_MAP()

	/* Constructor and Destructor */
	CNotifyIcon();
	~CNotifyIcon();
	STDMETHOD(init)(HWND hWnd, UINT CBMsg);

	/* INotifyIcon implementation */
	STDMETHOD(show)();
	STDMETHOD(update)();
	STDMETHOD(remove)();
	STDMETHOD(shellNotify)(DWORD dwMessage);

	STDMETHOD(setText)(BSTR text);
	STDMETHOD(setIcon)(BSTR strIcon, int nIndex);

	STDMETHOD(setHandler)(BSTR handler);

	BSTR getHandler();
};