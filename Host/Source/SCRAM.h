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
 * Filename:    SCRAM.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: This header defines some helper methods for authenticating
 *              using the Salted Challenge Response (SCRAM) SASL mechanism.
 *              See http://tools.ietf.org/html/draft-ietf-sasl-scram-10
 */

#pragma once
#include "DispInterfaceImpl.h"
#include "StringPrep.h"

typedef std::vector<unsigned char> ByteVector;

class SCRAM :
	public DispInterfaceImpl<ISCRAM>
{
	StringPrep m_sprep;

	unsigned m_IterationCount;

	UTF8String m_AuthMessage;
	UTF8String m_ClientNonce, m_ServerNonce;
	UTF8String m_ClientUsername, m_ClientPassword;
	UTF8String m_ClientFirstMessage, m_ClientFinalMessage;
	UTF8String m_ServerFirstMessage, m_ServerFinalMessage;

	ByteVector m_ClientSignature, m_ServerSignature;
	ByteVector m_ClientProof;
	ByteVector m_Salt;
public:
	SCRAM();
	~SCRAM();

	STDMETHOD(Initialize)(BSTR ClientUsername, BSTR ClientPassword);

	STDMETHOD(GenerateClientFirstMessage)(BSTR* ClientFirstMessage);
	STDMETHOD(GenerateClientFinalMessage)(BSTR *ClientFinalMessage);
	STDMETHOD(ValidateServerFirstMessage)(BSTR ServerFirstMessage);
	STDMETHOD(ValidateServerFinalMessage)(BSTR ServerFinalMessage);

private:
	ByteVector Hi(
		const std::string str,
		const ByteVector salt,
		const unsigned i);

	void GenerateNewClientNonce();
	void GenerateClientProof();

	UTF8String EscapeString(UTF8String str);
	UTF8String UnescapeString(UTF8String str);

	std::map<char, UTF8String> ParseSCRAMMessage(UTF8String message);
	std::pair<char, UTF8String> ParseSCRAMField(UTF8String message);

	void Error(
		std::wstring location,
		std::wstring whenCalling,
		unsigned errorCode);
};

class HMAC_SHA1
{
	static const unsigned int B = 64, L = 20;
	unsigned char keyXORipad[B];
	unsigned char keyXORopad[B];
public:
	HMAC_SHA1(const ByteVector key);

	ByteVector Calculate(const UTF8String text);
	ByteVector Calculate(const ByteVector text);
};