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
#include "DispInterfaceImpl.h"

class CIcon
{
	HICON m_hIcon;
public:
	CIcon();
	CIcon(BSTR strIcon, int nIndex);
	CIcon(const CIcon &);
	~CIcon();

	HICON getHandle() const;
};

class CNotifyIcon :
	public DispInterfaceImpl<INotifyIcon>
{
	/* Describes the owner */
	HWND	m_hWnd;
	UINT	m_CBMsg;

	/* Describes the icon */
	std::wstring                m_text;
	HICON						m_hIcon;

	/* The callback function to send events to the JScript */
    _bstr_t m_handler;
public:

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