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
 * Filename:    HTTP.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#include "stdafx.h"
#include "HTTP.h"
#include "HTTPDownload.h"
 
HTTP::HTTP()
{
	m_hInternet = ::InternetOpen(L"Pandion", INTERNET_OPEN_TYPE_PRECONFIG, 
		L"", INTERNET_INVALID_PORT_NUMBER, 0);
}
HTTP::~HTTP()
{
	::InternetCloseHandle(m_hInternet);
}
STDMETHODIMP HTTP::ShareFile(BSTR filename, BSTR URL, DWORD* retCookie)
{
	return S_OK;
}
STDMETHODIMP HTTP::UnShareFile(DWORD sfCookie)
{
	return S_OK;
}
 
STDMETHODIMP HTTP::Listen(unsigned short port)
{
	return S_OK;
}
STDMETHODIMP HTTP::StopListening(unsigned short port)
{
	return S_OK;
}
 
STDMETHODIMP HTTP::Get(
	BSTR filename,
	BSTR URI,
	DWORD offset,
	DWORD len,
	BSTR address,
	USHORT port,
	DWORD* sessionID)
{
	*sessionID = GetNewSessionID();
	m_Downloads[*sessionID] = 
		new HTTPDownload(m_hInternet, &m_EventRouter, *sessionID);
	m_Downloads[*sessionID]->Get(filename, URI, offset, len, address, port);

	return S_OK;
}
STDMETHODIMP HTTP::Post(BSTR filename, BSTR URI, DWORD offset, DWORD len, BSTR address, USHORT port, DWORD* sessionID)
{
	return S_OK;
}
STDMETHODIMP HTTP::SetProxyInfo(BSTR proxyAddress, USHORT proxyPort, BSTR proxyUName, BSTR proxyPass)
{
	return S_OK;
}
STDMETHODIMP HTTP::Subscribe(IDispatch* wnd)
{
	return m_EventRouter.Subscribe(wnd);
}
STDMETHODIMP HTTP::Unsubscribe(IDispatch* wnd)
{
	return m_EventRouter.Unsubscribe(wnd);
}
 
STDMETHODIMP HTTP::GetProgress(DWORD sessionID, DWORD* retval)
{
	*retval = m_Downloads[sessionID-1]->GetProgress();
	return S_OK;
}
STDMETHODIMP HTTP::GetLength(DWORD sessionID, DWORD* retval)
{
	*retval = m_Downloads[sessionID-1]->GetLength();
	return S_OK;
}
STDMETHODIMP HTTP::Abort(DWORD sessionID)
{
	m_Downloads[sessionID-1]->Abort();
	return S_OK;
}

DWORD HTTP::GetNewSessionID()
{
	static DWORD sID = 0;
	return ++sID;
}
