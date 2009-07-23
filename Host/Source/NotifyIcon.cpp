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
 * Filename:    NotifyIcon.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "NotifyIcon.h"

CIcon::CIcon()
{
	m_hIcon = 0;
}
CIcon::CIcon(BSTR strIcon, int nIndex)
{
	m_hIcon = ::ExtractIcon(GetModuleHandle(NULL), CW2T(strIcon), nIndex);
}
CIcon::CIcon(CIcon& copy)
{
	m_hIcon = copy.getHandle();
}
CIcon::~CIcon()
{
	if(m_hIcon != 0)
		DeleteObject(m_hIcon);
}
HICON CIcon::getHandle()
{
	return m_hIcon;
}


CNotifyIcon::CNotifyIcon() :
	m_text(L""), m_handler(L""), m_currentIcon(L"")
{
	m_icons.InitHashTable(16, true);
}
CNotifyIcon::~CNotifyIcon()
{
}

STDMETHODIMP CNotifyIcon::init(HWND hWnd, UINT CBMsg)
{
	m_hWnd  = hWnd;
	m_CBMsg = CBMsg;
	return S_OK;
}

STDMETHODIMP CNotifyIcon::show()
{
	return shellNotify(NIM_ADD);
}
STDMETHODIMP CNotifyIcon::update()
{
	return shellNotify(NIM_MODIFY);
}
STDMETHODIMP CNotifyIcon::remove()
{
	return shellNotify(NIM_DELETE);
}
STDMETHODIMP CNotifyIcon::shellNotify(DWORD dwMessage)
{
	if(!m_CBMsg || !m_hWnd || (m_text.Length() == 0 && !m_icons.Lookup(m_currentIcon)))
		return S_FALSE;

	NOTIFYICONDATA NotifyIconData;
	NotifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	NotifyIconData.hWnd = m_hWnd;
	NotifyIconData.uID = 0;
	NotifyIconData.uFlags = NIF_MESSAGE;
	NotifyIconData.uCallbackMessage = m_CBMsg;

	if(m_icons.Lookup(m_currentIcon))
	{
        NotifyIconData.hIcon = m_icons[m_currentIcon].getHandle();
		NotifyIconData.uFlags |= NIF_ICON;
	}

	if(m_text.Length())
	{
		StringCchCopy(NotifyIconData.szTip, 64, CW2T(m_text));
		NotifyIconData.uFlags |= NIF_TIP;
	}

	::Shell_NotifyIcon(dwMessage, &NotifyIconData);
	return S_OK;
}
STDMETHODIMP CNotifyIcon::setText(BSTR text)
{
	m_text = text;
	return S_OK;
}
STDMETHODIMP CNotifyIcon::setIcon(BSTR strIcon, int nIndex)
{
	if(m_icons.Lookup(CComBSTR(strIcon)))
	{
		m_currentIcon = strIcon;
	}
	else
	{
        m_icons[strIcon] = CIcon(strIcon, nIndex);
		m_currentIcon = strIcon;
	}
	return S_OK;
}
STDMETHODIMP CNotifyIcon::setHandler(BSTR handler)
{
	m_handler = handler;
	return S_OK;
}
BSTR CNotifyIcon::getHandler()
{
	return m_handler;
}