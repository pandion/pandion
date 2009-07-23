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
 * Filename:    ProxyInfo.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#pragma once

class CProxyInfo
{
private:
	CComBSTR m_Address;
	USHORT   m_Port;
	CComBSTR m_Username;
	CComBSTR m_Password;
public:
	CProxyInfo();

	void SetAddress (BSTR address);
	void SetPort    (USHORT port);
	void SetUsername(BSTR username);
	void SetPassword(BSTR password);
	void GetAddress (BSTR *address);
	USHORT GetPort  ();
	void GetUsername(BSTR *username);
	void GetPassword(BSTR *password);
};
