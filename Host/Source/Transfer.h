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
 * Filename:    Transfer.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#pragma once

class CPile;
class CHTTPRequest;
class Socket;

class CTransfer
{
protected:
	CComPtr<IEventRouter>	m_pER;
	CPile*					m_pPile;

	Socket*					m_pSocket;

	DWORD					m_SessionID;
	BOOL					m_bAborted;
protected:
	CComPtr<IPdnFile>		m_pFile;
	BYTE*					m_buf;
	DWORD					m_fp;
public:
	CTransfer(IEventRouter* pER, CPile* pPile, Socket* pSocket, DWORD sessionID);
	~CTransfer();

	virtual DWORD PreCommand() = 0;
	virtual DWORD Command() = 0;
	virtual DWORD PostCommand() = 0;

	virtual DWORD GetProgress() = 0;
	virtual DWORD GetLength() = 0;
};