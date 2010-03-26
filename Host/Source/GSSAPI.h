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

class GSSAPIException
{
private:
	std::wstring m_Caller;
	std::wstring m_Callee;
	std::wstring m_DebugInfo;
	unsigned m_ErrorCode;
public:
	GSSAPIException(
		std::wstring caller,
		std::wstring callee,
		std::wstring debuginfo,
		unsigned errorCode);
	std::wstring ToString();
	unsigned GetErrorCode();
};

class GSSAPI :
	public DispInterfaceImpl<IGSSAPI>
{
private:
	unsigned		m_MaxTokenSize;

	bool			m_NewConversation;
	bool			m_InitComplete;

	CredHandle		m_CredHandle;
	bool			m_CredHandleValid;

	SecHandle		m_CtxtHandle;
	bool			m_CtxtHandleValid;

	std::wstring	m_ServerName;
	std::wstring    m_ServicePrincipalName;
	std::wstring    m_LastErrorMessage;
public:
	GSSAPI();
	~GSSAPI();

	STDMETHOD(Reset)();
	STDMETHOD(GenerateResponse)(BSTR ServerName, BSTR Challenge,
		BSTR *Response);
	STDMETHOD(GetLastErrorMessage)(BSTR* ErrorMessage);

private:
	void SetLastErrorMessage(std::wstring ErrorMessage);
	std::wstring GenerateDebugInfo();

	void AcquireCredentials();
	std::vector<BYTE> Initialize(std::vector<BYTE> decodedChalenge,
		BSTR ServerName);
	std::vector<BYTE> PostInitialize(std::vector<BYTE> decodedChalenge);
	std::wstring GenerateServicePrincipalName(std::wstring ServerName);
	std::wstring GetDomainName();
};
