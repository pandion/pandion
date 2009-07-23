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
 * Filename:    Session.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "Session.h"
#include "Pile.h"

CSession::CSession(CPile* p, IStream *pERStream) :
	m_pPile(p), m_pER(NULL)
{
	m_SessionID = GetNewCookie();
	HRESULT hr = CoGetInterfaceAndReleaseStream(pERStream, IID_IEventRouter, (void **) &m_pER);
}
CSession::~CSession()
{
}

DWORD CSession::GetNewCookie()
{
	static int lastCookie = 0;
	return ++lastCookie;
}
DWORD CSession::GetSessionID()
{
	return m_SessionID;
}

void CSession::PreCommand()
{
	m_pPile->AddSession(this);
}
void CSession::Command()
{
}
void CSession::PostCommand()
{
	m_pER->Release();
	m_pPile->DelSession(GetSessionID());
}
