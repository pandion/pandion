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
 * Filename:    HTTPEngine.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "HTTP.h"
 
HTTP::HTTP()
{
}
HTTP::~HTTP()
{
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
 
STDMETHODIMP HTTP::Get(BSTR filename, BSTR URI, DWORD offset, DWORD len, BSTR address, USHORT port, DWORD* sessionID)
{
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
	return S_OK;
}
STDMETHODIMP HTTP::Unsubscribe(IDispatch* wnd)
{
	return S_OK;
}
 
STDMETHODIMP HTTP::GetProgress(DWORD sessionID, DWORD* retval)
{
	return S_OK;
}
STDMETHODIMP HTTP::GetLength(DWORD sessionID, DWORD* retval)
{
	return S_OK;
}
STDMETHODIMP HTTP::Abort(DWORD sessionID)
{
	return S_OK;
}