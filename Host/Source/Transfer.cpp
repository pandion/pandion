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
 * Filename:    Transfer.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "Transfer.h"
#include "File.h"

CTransfer::CTransfer(IEventRouter* pER, CPile* pPile, Socket* pSocket, DWORD sessionID) :
	m_pER(pER), m_pPile(pPile), m_pSocket(pSocket), m_SessionID(sessionID), m_bAborted(TRUE), m_fp(0)
{
	(new CComObject<CFile>)->QueryInterface(&m_pFile);
	m_buf = new BYTE[BLOCK_SIZE];
}
CTransfer::~CTransfer()
{
	delete m_buf;
}
