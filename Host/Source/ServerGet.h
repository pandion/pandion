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
 * Filename:    ServerGet.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#pragma once
#include "Transfer.h"

class Socket;
class CHTTPRequest;
class CPile;
class CFile;

class CServerGet :
	public CTransfer
{
private:
	CHTTPRequest* m_pRequest;

	DWORD m_nBytesToSend;
	DWORD m_nTotalBytesToSend;
public:
	CServerGet(IEventRouter* pER, CPile* pPile, CHTTPRequest* pRequest, Socket* pSocket, DWORD sessionID);
	~CServerGet();

	DWORD PreCommand();
	DWORD Command();
	DWORD PostCommand();

	DWORD GetProgress();
	DWORD GetLength();
};
