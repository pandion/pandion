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
 * Filename:    Session.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#pragma once
#include "BasicThread.h"

class CPile;

class CSession : public CBasicThread
{
protected:
	DWORD         m_SessionID;
	IEventRouter *m_pER;
	CPile        *m_pPile;
public:
	CSession( CPile* p, IStream *pERStream );
	virtual ~CSession();

	DWORD GetSessionID();

	virtual void PreCommand();
	virtual void Command();
	virtual void PostCommand();

	virtual DWORD GetProgress() = 0;
	virtual DWORD GetLength() = 0;
	virtual void  Terminate() = 0;
private:
	static DWORD GetNewCookie();
};
