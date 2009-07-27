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
 * Filename:    HTTPEventRouter.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#pragma once

class HTTPEventRouter
{
private:
	std::vector<IPdnWnd*> m_Subscribers;
public:
	HTTPEventRouter();
	~HTTPEventRouter();

	STDMETHOD(Subscribe)(IDispatch* pDisp);
	STDMETHOD(Unsubscribe)(IDispatch* pDisp);
	STDMETHOD(UnsubscribeAll)();

	STDMETHOD(OnListening)(USHORT Port);
	STDMETHOD(OnAccept)(DWORD sessionID, BSTR remHost, USHORT remPort, USHORT localPort);
	STDMETHOD(OnConnected)(DWORD sessionID, BSTR Server, USHORT Port, USHORT LocalPort);

	STDMETHOD(OnGetRequest)(DWORD sessionID, DWORD fileID, BSTR URI, BSTR localPath);
	STDMETHOD(OnPostRequest)(DWORD sessionID, BSTR URI, BSTR* saveAs);

	STDMETHOD(OnTimeout)(DWORD sessionID);
	STDMETHOD(OnSockErr)(DWORD sessionID, DWORD err);
	STDMETHOD(OnHTTPErr)(DWORD sessionID, DWORD err);
	STDMETHOD(OnFileErr)(DWORD sessionID, DWORD err);

	STDMETHOD(OnTransferAborted)(DWORD sessionID);
	STDMETHOD(OnTransferComplete)(DWORD sessionID);
};