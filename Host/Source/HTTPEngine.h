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
 * Filename:    HTTPEngine.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#pragma once

class CPile;
class CProxyInfo;
class CListenThread;

class CHTTP :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IHTTP>
{
private:	
	CPile* m_pPile;
	CProxyInfo* m_pProxyInfo;
	CListenThread* m_pLT;
	IEventRouter* m_pER;
public:
	CHTTP();
	~CHTTP();

BEGIN_COM_MAP(CHTTP)
	COM_INTERFACE_ENTRY(IHTTP)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IHTTP
	STDMETHOD(ShareFile)(BSTR filename, BSTR URL, DWORD* retCookie);
	STDMETHOD(UnShareFile)(DWORD sfCookie);
	
	STDMETHOD(Listen)(USHORT port);
	STDMETHOD(StopListening)(USHORT port);

	STDMETHOD(Get)(BSTR filename, BSTR URI, DWORD offset, DWORD len, BSTR address, USHORT port, DWORD* sessionID);
	STDMETHOD(Post)(BSTR filename, BSTR URI, DWORD offset, DWORD len, BSTR address, USHORT port, DWORD* sessionID);

	STDMETHOD(SetProxyInfo)(BSTR proxyAddress, USHORT proxyPort, BSTR proxyUName, BSTR proxyPass);

	STDMETHOD(Subscribe)(IDispatch* wnd);
	STDMETHOD(Unsubscribe)(IDispatch* wnd);

	STDMETHOD(GetProgress)(DWORD sessionID, DWORD* retval);
	STDMETHOD(GetLength)(DWORD sessionID, DWORD* retval);

	STDMETHOD(Abort)(DWORD sessionID);
}; 