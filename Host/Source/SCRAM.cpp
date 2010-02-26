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
 * Filename:    SCRAM.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This file implements some helper methods for authenticating
 *              using the Salted Challenge Response (SCRAM) SASL mechanism.
 *              See http://tools.ietf.org/html/draft-ietf-sasl-scram-10
 */
#include "stdafx.h"
#include "SCRAM.h"
#include "Hash.h"
#include "UTF.h"
#include "Base64.h"

SCRAM::SCRAM()
{
	m_CannotSelfDelete = true;
}
SCRAM::~SCRAM()
{
}

STDMETHODIMP SCRAM::Initialize(BSTR ClientUsername, BSTR ClientPassword)
{
	try
	{
		m_ClientUsername = 
			m_sprep.SASLPrep(UTF::utf16to8(ClientUsername), false);
		m_ClientPassword = 
			m_sprep.SASLPrep(UTF::utf16to8(ClientPassword), false);

		if(m_ClientUsername.length() == 0 || m_ClientPassword.length() == 0)
		{
			return E_FAIL;
		}
	}
	catch(StringPrepException e)
	{
		return E_FAIL;
	}

	GenerateNewClientNonce();
	return S_OK;
}

STDMETHODIMP SCRAM::GenerateClientFirstMessage(BSTR* ClientFirstMessage)
{
	m_ClientFirstMessage.clear();
	/* Channel bindings unsupported */
	m_ClientFirstMessage += "n,,";

	/* Client username */
	m_ClientFirstMessage += "n=";
	m_ClientFirstMessage += m_ClientUsername;
	m_ClientFirstMessage += ",";

	/* Client nonce */
	m_ClientFirstMessage += "r=";
	m_ClientFirstMessage += m_ClientNonce;

	*ClientFirstMessage = 
		::SysAllocString(Base64::Encode(m_ClientFirstMessage, false).c_str());

	return S_OK;
}

STDMETHODIMP SCRAM::GenerateClientFinalMessage(BSTR* ClientFinalMessage)
{
	m_ClientFinalMessage.clear();
	/* GS2 Header */
	m_ClientFinalMessage += "c=biwsCg==,";

	/* Client/Server nonce */
	m_ClientFinalMessage += "r=";
	m_ClientFinalMessage += m_ClientNonce;
	m_ClientFinalMessage += m_ServerNonce;

	/* Auth Message */
	m_AuthMessage.clear();
	m_AuthMessage += m_ClientFirstMessage.substr(3);
	m_AuthMessage += ",";
	m_AuthMessage += m_ServerFirstMessage;
	m_AuthMessage += ",";
	m_AuthMessage += m_ClientFinalMessage;

	/* Client Proof */
	GenerateClientProof();

	m_ClientFinalMessage += ",p=";
	m_ClientFinalMessage += 
		UTF::utf16to8(Base64::Encode(m_ClientProof, false));

	*ClientFinalMessage =
		::SysAllocString(Base64::Encode(m_ClientFinalMessage, false).c_str());
	return S_OK;
}


STDMETHODIMP SCRAM::ValidateServerFirstMessage(BSTR ServerFirstMessage)
{
	ByteVector v = 
		Base64::Decode(UTF::utf16to8(ServerFirstMessage));
	m_ServerFirstMessage = UTF8String(v.begin(), v.end());
	std::map<char, UTF8String> parsedMessage = 
		ParseSCRAMMessage(m_ServerFirstMessage);

	m_Salt = Base64::Decode(parsedMessage['s']);
	m_IterationCount = atoi(parsedMessage['i'].c_str());

	if(parsedMessage['r'].find(m_ClientNonce) == 0)
	{
		m_ServerNonce = parsedMessage['r'].substr(m_ClientNonce.length());
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

STDMETHODIMP SCRAM::ValidateServerFinalMessage(BSTR ServerFinalMessage)
{
	ByteVector v = Base64::Decode(UTF::utf16to8(ServerFinalMessage));
	m_ServerFinalMessage = UTF8String(v.begin(), v.end());
	std::map<char, UTF8String> parsedMessage = 
		ParseSCRAMMessage(m_ServerFinalMessage);

	ByteVector ServerSignature =
		Base64::Decode(parsedMessage['v']);

	if(ServerSignature.size() == m_ServerSignature.size())
	{
		for(unsigned i = 0; i < m_ServerSignature.size(); i++)
		{
			if(m_ServerSignature[i] != ServerSignature[i])
			{
				return E_FAIL;
			}
		}
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

ByteVector SCRAM::Hi(
	const std::string str,
	const ByteVector salt,
	const unsigned i)
{
	const ByteVector key(str.begin(), str.end());
	ByteVector result(20, 0x00);
	ByteVector u(salt.begin(), salt.end());
	u.push_back(0);
	u.push_back(0);
	u.push_back(0);
	u.push_back(1);

	HMAC_SHA1 hmac(key);
	
	for(unsigned j = 0; j < i; j++)
	{
		u = hmac.Calculate(u);
		for(unsigned k = 0; k < u.size(); k += 4)
		{
			*(unsigned*)(&result[0] + k) ^= *(unsigned*)(&u[0] + k);
		}
	}

	return result;
}

void SCRAM::GenerateNewClientNonce()
{
	HCRYPTPROV provider;
	unsigned char randomBytes[24];
	::CryptAcquireContext(&provider, NULL, NULL, PROV_RSA_FULL, 0);
	::CryptGenRandom(provider, 24, randomBytes);
	::CryptReleaseContext(provider, 0);
	m_ClientNonce = UTF::utf16to8(Base64::Encode(randomBytes, 24, false));
}

void SCRAM::GenerateClientProof()
{
	/* Proof of identity */
	ByteVector saltedPassword =
		Hi(m_ClientPassword, m_Salt, m_IterationCount);
	ByteVector clientKey = HMAC_SHA1(saltedPassword).Calculate("Client Key");
	ByteVector storedKey(20);
	Hash::SHA1(&clientKey[0], clientKey.size(), &storedKey[0]);
	ByteVector clientSignature = HMAC_SHA1(storedKey).Calculate(m_AuthMessage);

	m_ClientProof.clear();
	for(unsigned j = 0; j < storedKey.size(); ++j)
	{
		m_ClientProof.push_back(clientKey[j] ^ clientSignature[j]);
	}

	/* Calculate the server signature */
	ByteVector serverKey = HMAC_SHA1(saltedPassword).Calculate("Server Key");
	m_ServerSignature = HMAC_SHA1(serverKey).Calculate(m_AuthMessage);
}

UTF8String SCRAM::EscapeString(UTF8String str)
{
	UTF8String escapedString;
	for(UTF8String::const_iterator i = str.begin(); i != str.end(); ++i)
	{
		if(*i == '=')
		{
			escapedString += "=3D";
		}
		else if(*i == ',')
		{
			escapedString += "=2C";
		}
		else
		{
			escapedString += *i;
		}
	}
	return escapedString;
}

UTF8String SCRAM::UnescapeString(UTF8String str)
{
	UTF8String unescapedString;
	UTF8String::const_iterator i = str.begin();
	while(i + 2 != str.end())
	{
		if(*i == '=' && *(i + 1) == '3' && *(i + 2) == 'D')
		{
			unescapedString += "=";
			i += 3;
		}
		else if(*i == '=' && *(i + 1) == '2' && *(i + 2) == 'C')
		{
			unescapedString += ",";
			i += 3;
		}
		else
		{
			unescapedString += *i;
			i += 1;
		}
	}
	unescapedString += *i++;
	unescapedString += *i++;
	return unescapedString;
}

std::map<char, UTF8String> SCRAM::ParseSCRAMMessage(UTF8String message)
{
	std::map<char, UTF8String> parsedData;
	size_t pos = message.find(','), begin = 0;

	while(pos != UTF8String::npos)
	{
		parsedData.insert(ParseSCRAMField(message.substr(begin, pos - begin)));
		begin = pos + 1;
		pos = message.find(',', begin);
	}
	parsedData.insert(ParseSCRAMField(message.substr(begin)));

	return parsedData;
}

std::pair<char, UTF8String> SCRAM::ParseSCRAMField(UTF8String field)
{
	std::pair<char, UTF8String> invalidField(0, "");

	UTF8String::const_iterator i = field.begin();
	if(field.length() > 2)
	{
		char fieldName = *i;
		if(*++i != '=')
		{
			return invalidField;
		}
		UTF8String value = field.substr(2, field.length());
		return std::pair<char, UTF8String>(fieldName, UnescapeString(value));
	}
	else
	{
		return invalidField;
	}

}

void SCRAM::Error(std::wstring location,
				  std::wstring whenCalling,
				  unsigned errorCode)
{
	LPTSTR errorMessage;
	::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL, errorCode, 0, (LPTSTR)&errorMessage, 0, NULL);
	std::wostringstream dbgMsg;
	dbgMsg << L"SCRAM error in " << location <<
		L" when calling " << whenCalling <<
		std::hex <<	std::setw(8) <<	std::setfill(L'0') << 
		L": (ERROR 0x" << errorCode << L") " <<
		errorMessage <<	std::endl;
	OutputDebugString(dbgMsg.str().c_str());
	::LocalFree(errorMessage);
}

HMAC_SHA1::HMAC_SHA1(const ByteVector key)
{
	const static unsigned char ipad[] = 
		"\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36"
		"\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36"
		"\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36"
		"\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36";
	const static unsigned char opad[] =
		"\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C"
		"\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C"
		"\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C"
		"\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C\x5C";
	unsigned char internalKey[] =
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

	if(key.size() > B)
	{
		Hash::SHA1(reinterpret_cast<const unsigned char*>(&key[0]),
			key.size(), internalKey);
	}
	else
	{
		for(unsigned int i = 0; i < key.size(); i++)
		{
			internalKey[i] = key[i];
		}
	}

	for(unsigned int i = 0; i < B; i += 4)
	{
		*(unsigned *)(keyXORipad + i) =
			*(unsigned *)(internalKey + i) ^ *(unsigned *)(ipad + i);
		*(unsigned *)(keyXORopad + i) =
			*(unsigned *)(internalKey + i) ^ *(unsigned *)(opad + i);
	}
}

ByteVector HMAC_SHA1::Calculate(const UTF8String text)
{
	return Calculate(ByteVector(text.begin(), text.end()));
}

ByteVector HMAC_SHA1::Calculate(const ByteVector text)
{
	unsigned char innerDigest[L];
	ByteVector innerMessage(keyXORipad, keyXORipad + B);
	innerMessage.insert(innerMessage.end(), text.begin(), text.end());
	Hash::SHA1(&innerMessage[0], innerMessage.size(), innerDigest);

	unsigned char outerDigest[L];
	ByteVector outerMessage(keyXORopad, keyXORopad + B);
	outerMessage.insert(outerMessage.end(), innerDigest, innerDigest + L);
	Hash::SHA1(&outerMessage[0], outerMessage.size(), outerDigest);

	return ByteVector(outerDigest, outerDigest + L);
}