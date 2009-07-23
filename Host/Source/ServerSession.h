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
 * Filename:    ServerSession.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#pragma once
#include "Session.h"

class Socket;
class CPile;
class CTransfer;
class CHTTPRequest;

class CServerSession :
	public CSession
{
private:
	Socket* m_pSocket;
	CHTTPRequest* m_pRequest;

	CTransfer* m_pTransfer;
public:
	CServerSession(CPile* pPile, Socket* sock, IStream *pERStream);
	~CServerSession();

	DWORD RecvRequest();

	void PreCommand();
	void PostCommand();
	void Command();

	DWORD GetProgress();
	DWORD GetLength();
	void  Terminate();
};