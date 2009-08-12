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
 * Filename:    HTTP.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#pragma once

#include "DispInterfaceImpl.h"
#include "HTTPEventRouter.h"
#include "HTTPListener.h"

class HTTPSession;
class HTTPDownload;
 
class HTTP :
	public DispInterfaceImpl<IHTTP>
{
private:
	HINTERNET m_hInternet;
	std::map<int, HTTPSession*> m_Sessions;

	HTTPEventRouter m_EventRouter;
	HTTPListener    m_Listener;
public:
	HTTP();
	~HTTP();
  
	/* IHTTP */
	STDMETHOD(ShareFile)(BSTR filename, BSTR URL, DWORD* sessionID);
	STDMETHOD(UnShareFile)(DWORD sessionID);
 
	STDMETHOD(Listen)(USHORT port);
	STDMETHOD(StopListening)(USHORT port);
 
	STDMETHOD(Get)(BSTR filename, BSTR URI, DWORD offset, DWORD len,
		BSTR address, USHORT port, DWORD* sessionID);
	STDMETHOD(Post)(BSTR filename, BSTR URI, DWORD offset, DWORD len,
		BSTR address, USHORT port, DWORD* sessionID);
 
	STDMETHOD(Subscribe)(IDispatch* wnd);
	STDMETHOD(Unsubscribe)(IDispatch* wnd);
 
	STDMETHOD(GetProgress)(DWORD sessionID, DWORD* retval);
	STDMETHOD(GetLength)(DWORD sessionID, DWORD* retval);
 
	STDMETHOD(Abort)(DWORD sessionID);

private:
	DWORD GetNewSessionID();
}; 