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
 * Filename:    ProxyInfo.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "ProxyInfo.h"

CProxyInfo::CProxyInfo()
{
	m_Address = "";
	m_Port = 0;
	m_Username = "";
	m_Password = "";
}

void CProxyInfo::SetAddress( BSTR address )
{
	if( wcslen( address ) < 128 )
		m_Address = address;
}
void CProxyInfo::SetPort( USHORT port )
{
	m_Port = port;
}
void CProxyInfo::SetUsername( BSTR username )
{
	if( wcslen( username ) < 128 )
		m_Username = username;
}
void CProxyInfo::SetPassword( BSTR password )
{
	if( wcslen( password ) < 128 )
		m_Password = password;
}

void CProxyInfo::GetAddress( BSTR *address )
{
	*address = m_Address.Copy();
}
USHORT CProxyInfo::GetPort()
{
	return m_Port;
}
void CProxyInfo::GetUsername( BSTR *username )
{
	*username = m_Username.Copy();
}
void CProxyInfo::GetPassword( BSTR *password )
{
	*password = m_Password.Copy();
}