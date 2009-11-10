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
 * Filename:    GSSAPI.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#pragma once
#include "DispInterfaceImpl.h"

#define PACKAGE_NAME	TEXT("Kerberos")

class GSSAPI :
	public DispInterfaceImpl<IGSSAPI>
{
private:
	DWORD			m_dwMaxTokenSize;

	BOOL			m_fNewConversation;

	CredHandle		m_hCred;
	BOOL			m_fHaveCredHandle;

	SecHandle		m_hCtxt;
	BOOL			m_fHaveCtxtHandle;
public:
	GSSAPI();
	~GSSAPI();

	STDMETHOD(Reset)();
	STDMETHOD(GenerateResponse)(
		BSTR Challenge, BOOL *Continue, BSTR *Response);

private:
	void Error(LPWSTR Where, LPWSTR WhenCalling, LPWSTR ErrorMessage);
};
