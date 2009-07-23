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
 * Filename:    ClientSession.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#pragma once
#include "Session.h"

class CProxyInfo;
class CPile;
class CFile;
class Socket;
class CHTTPResponse;
class CTransfer;

struct CCSInfo
{
	CProxyInfo*	pProxyInfo;
	WCHAR		address[128];
	USHORT		port;
	
	DWORD		method;
	WCHAR		URI[1024];

	IPdnFile*	pFile;
	DWORD		offset;
	DWORD		len;
};

class CClientSession :
	public CSession
{
private:
	Socket* m_pSocket;
	
	CCSInfo*       m_pCSInfo;
	CTransfer*     m_pTransfer;
public:
	CClientSession(CPile* pPile, IStream* pERStream, CCSInfo* pCSInfo);
	~CClientSession();

	DWORD ClientConnect();

	void PreCommand();
	void PostCommand();
	void Command();

	DWORD GetProgress();
	DWORD GetLength();
	void  Terminate();
};