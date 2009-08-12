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
 * Filename:    HTTPServer.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#include "stdafx.h"
#include "HTTPServer.h"
#include "HTTPEventRouter.h"

HTTPServer::HTTPServer(HINTERNET hInternet, HTTPEventRouter* eventRouter,
	DWORD sessionID) :
	m_Progress(0), m_Length(-1), m_Abort(false),
	m_EventRouter(eventRouter), m_SessionID(sessionID)
{
}
HTTPServer::~HTTPServer()
{
}
DWORD HTTPServer::GetProgress()
{
	return m_Progress;
}
DWORD HTTPServer::GetLength()
{
	return m_Length;
}
void HTTPServer::Abort()
{
	m_Abort = true;
}
