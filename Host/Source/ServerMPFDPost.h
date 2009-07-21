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
 * Filename:    ServerMPFDPost.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#pragma once
#include "Transfer.h"

class CFile;
class CPile;
class CHTTPRequest;
class CSocket;

class CServerMPFDPost :
	public CTransfer
{
private:
	BYTE*  m_buf;
	TCHAR   m_boundary[128];

	DWORD  m_fp;
	HANDLE m_hFile;
	CFile* m_pFile;
	CHTTPRequest* m_pRequest;

	DWORD  m_nBytesToRecv;
public:
	CServerMPFDPost( IEventRouter* pER, CPile* pPile, CHTTPRequest* pRequest, Socket* pSocket, DWORD sessionID );
	~CServerMPFDPost();

	bool EatBoundary();

	DWORD PreCommand();
	DWORD Command();
	DWORD PostCommand();

	DWORD GetProgress();
	DWORD GetLength();
};