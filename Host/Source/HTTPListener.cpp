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
 * Filename:    HTTPListener.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#include "stdafx.h"
#include "HTTPListener.h"

HTTPListener::HTTPListener(HTTPEventRouter* eventRouter) :
	m_EventRouter(eventRouter)
{
}
HTTPListener::~HTTPListener()
{
}

void HTTPListener::ShareFile(BSTR filename, BSTR URL, DWORD sessionID)
{
}
void HTTPListener::UnShareFile(DWORD sessionID)
{
}
 
void HTTPListener::Listen(unsigned short port)
{
}
void HTTPListener::StopListening(unsigned short port)
{
}