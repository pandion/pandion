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

SCRAM::SCRAM()
{
}
SCRAM::~SCRAM()
{
}
STDMETHODIMP SCRAM::GenerateResponse(BSTR Challenge, BSTR *Response)
{
	return E_NOTIMPL;
}

std::vector<unsigned char> SCRAM::HMAC_SHA1(const std::string key,
											const std::string text)
{
	const unsigned int B = 64, L = 20;
	const std::vector<unsigned char> ipad(B, 0x36);
	const std::vector<unsigned char> opad(B, 0x5C);
	std::vector<unsigned char> internalKey(key.begin(), key.end());

	if(internalKey.size() > B)
	{
		unsigned char digest[L];
		Hash::SHA1(reinterpret_cast<const unsigned char*>(&internalKey[0]),
			internalKey.size(), digest); 
		internalKey.assign(digest,
			digest + sizeof(digest) / sizeof(unsigned char));
	}

	for(unsigned int i = internalKey.size(); i < B; i++)
	{
		internalKey.push_back(0x00);
	}

	std::vector<unsigned char> keyXORipad, keyXORopad;
	for(unsigned int i = 0; i < B; i++)
	{
		keyXORipad.push_back(internalKey[i] ^ ipad[i]);
		keyXORopad.push_back(internalKey[i] ^ opad[i]);
	}

	std::vector<unsigned char> innerMessage(keyXORipad);
	for(unsigned int i = 0; i < text.length(); i++)
	{
		innerMessage.push_back(text[i]);
	}

	unsigned char innerDigest[L];
	Hash::SHA1(&innerMessage[0], innerMessage.size(), innerDigest);

	std::vector<unsigned char> outerMessage(keyXORopad);
	for(unsigned int i = 0; i < sizeof(innerDigest); i++)
	{
		outerMessage.push_back(innerDigest[i]);
	}

	unsigned char outerDigest[L];
	Hash::SHA1(&outerMessage[0], outerMessage.size(), outerDigest);

	return std::vector<unsigned char>(outerDigest,
		outerDigest + sizeof(outerDigest) / sizeof(unsigned int));
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