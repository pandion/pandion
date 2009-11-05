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
 * Filename:    SASL.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"

#include "SASL.h"
#include "UTF8.h"
#include "Hash.h"
#include "Base64.h"

SASL::SASL()
{
	m_fNewConversation = TRUE;
	m_fHaveCredHandle  = FALSE;
	m_fHaveCtxtHandle  = FALSE;
	SSPIReset();
}
SASL::~SASL()
{
}
STDMETHODIMP SASL::PlainGenerateResponse(BSTR jid, BSTR username,
	BSTR password, BSTR* strBase64)
{
	std::stringstream stringBuffer;
	stringBuffer <<	CW2UTF8(jid) << '\0' << CW2UTF8(username) << '\0' <<
		CW2UTF8(password);
	*strBase64 = ::SysAllocString(Base64::Base64Encode(stringBuffer.str().c_str(),
		stringBuffer.str().length(), false).c_str());
	return S_OK;
}
STDMETHODIMP SASL::DigestGenerateResponse(BSTR username, BSTR realm,
	BSTR password, BSTR nonce, BSTR cnonce, BSTR digest_uri, BSTR nc,
	BSTR qop, BSTR *strDigest)
{
	/* MD5-Session algorithm:
	var X	= Response('username') + ':' + Response('realm') + ':' +
		Response('password');
	var Y	= md5.digest(X); // This should be binary data instead of the 
							 // hexadecimal string
	var A1	= Y + ':' + Response('nonce') + ':' +
		Response('cnonce'); // + ':' + Response('authzid')
	var A2	= 'AUTHENTICATE:' + Response('digest-uri');
	var HA1	= md5.digest(A1);
	var HA2	= md5.digest(A2);
	var KD	= HA1 + ':' + Response('nonce') + ':' + Response('nc') + ':' + 
		Response('cnonce') + ':' + Response('qop') + ':' + HA2;
	var Z	= md5.digest(KD);
	*/
	std::string colon = std::string(":");

	/* calculate X */
	std::string X = std::string(CW2UTF8(username)) + colon +
		std::string(CW2UTF8(realm)) + colon + std::string(CW2UTF8(password));

	/* calculate Y */
	unsigned char binaryY[16];
	Hash::MD5((unsigned char*)X.c_str(), X.length(), binaryY);

	/* calculate A1 */
	std::string A1 = "1234567890123456" + colon + std::string(CW2UTF8(nonce)) +
		colon + std::string(CW2UTF8(cnonce));
	std::copy(binaryY, binaryY + 16, &A1[0]);

	/* calculate A2 */
	std::string A2 = std::string("AUTHENTICATE:") + 
		std::string(CW2UTF8(digest_uri));

	/* calculate HA1 */
	unsigned char binaryHA1[16];
	Hash::MD5((unsigned char*)A1.c_str(), A1.length(), binaryHA1);
	std::string HA1(2*16, '\0');
	HexString(binaryHA1, &HA1[0], 16);

	/* calculate HA2 */
	unsigned char binaryHA2[16];
	Hash::MD5((unsigned char*)A2.c_str(), A2.length(), binaryHA2);
	std::string HA2(2*16, '\0');
	HexString(binaryHA2, &HA2[0], 16);

    /* calculate KD */
	std::string KD = HA1 + colon + std::string(CW2UTF8(nonce)) + colon +
		std::string(CW2UTF8(nc)) + colon + std::string(CW2UTF8(cnonce)) + 
		colon +	std::string(CW2UTF8(qop)) + colon + HA2;

	/* calculate Z */
	unsigned char binaryZ[16];
	Hash::MD5((unsigned char*) KD.c_str(), KD.length(), binaryZ);
	std::string Z(2*16, '\0');
	HexString(binaryZ, &Z[0], 16);

	*strDigest = ::SysAllocString(CUTF82W(Z.c_str()));

	return S_OK;
}

STDMETHODIMP SASL::SSPIReset()
{
	/* Query Security Package Information */
	PSecPkgInfo SecurityPackage;
	SECURITY_STATUS ss = 
		::QuerySecurityPackageInfo(PACKAGE_NAME, &SecurityPackage);
	if (ss != SEC_E_OK)
	{
		SSPIError(L"SSPIReset()",
			L"QuerySecurityPackageInfo()",
			L"Unknown Error.");
		return E_FAIL;
	}
	else
	{
		m_dwMaxTokenSize = SecurityPackage->cbMaxToken;
		FreeContextBuffer(SecurityPackage);
	}

	/* Clean up existing handles */
	if(m_fHaveCredHandle)
	{
		FreeCredentialsHandle(&m_hCred);
	}

	/* Reset flags */
	m_fNewConversation = TRUE;
	m_fHaveCredHandle  = FALSE;
	m_fHaveCtxtHandle  = FALSE;
	return S_OK;
}
STDMETHODIMP SASL::SSPIGenerateResponse(BSTR Challenge, BOOL *Continue, BSTR *Response)
{
	/* Acquire a credentials handle if this is the first response */
	if(m_fNewConversation == TRUE)
	{
		TimeStamp		Lifetime;

		SECURITY_STATUS	ss = AcquireCredentialsHandle(
						NULL,	// principal
						PACKAGE_NAME,
						SECPKG_CRED_OUTBOUND,
						NULL,	// LOGON id
						NULL,	// auth data
						NULL,	// get key fn
						NULL,	// get key arg
						&m_hCred,
						&Lifetime);

		switch(ss)
		{
		case SEC_E_OK:
			m_fHaveCredHandle = TRUE;
			break;
		case SEC_E_UNKNOWN_CREDENTIALS:
			SSPIError(L"SSPIGenerateReponse()",
				L"AcquireCredentialsHandle()",
				L"The credentials supplied to the package were not recognized.");
			return E_FAIL;
		case SEC_E_NO_CREDENTIALS:
			SSPIError(L"SSPIGenerateReponse()",
				L"AcquireCredentialsHandle()",
				L"No credentials are available in the security package.");
			return E_FAIL;
		case SEC_E_NOT_OWNER:
			SSPIError(L"SSPIGenerateReponse()",
				L"AcquireCredentialsHandle()",
				L"The caller of the function does not have the necessary credentials.");
			return E_FAIL;
		case SEC_E_INSUFFICIENT_MEMORY:
			SSPIError(L"SSPIGenerateReponse()",
				L"AcquireCredentialsHandle()",
				L"Not enough memory is available to complete the request.");
			return E_FAIL;
		case SEC_E_INTERNAL_ERROR:
			SSPIError(L"SSPIGenerateReponse()",
				L"AcquireCredentialsHandle()",
				L"The Local Security Authority (LSA) cannot be contacted.");
			return E_FAIL;
		case SEC_E_SECPKG_NOT_FOUND:
			SSPIError(L"SSPIGenerateReponse()",
				L"AcquireCredentialsHandle()",
				L"The requested security package does not exist.");
			return E_FAIL;
		default:
			SSPIError(L"SSPIGenerateReponse()",
				L"AcquireCredentialsHandle()",
				L"Unknown Error.");
			return E_FAIL;
		}
	}

	/* Decode the Challenge */
	DWORD DecodedChallengeLength;
	::CryptStringToBinary(Challenge, ::SysStringLen(Challenge),
		CRYPT_STRING_BASE64, NULL, &DecodedChallengeLength, NULL, NULL);
	std::vector<BYTE> DecodedChallenge(DecodedChallengeLength > 0 ? 
		DecodedChallengeLength : 1);
	::CryptStringToBinary(Challenge, ::SysStringLen(Challenge),
		CRYPT_STRING_BASE64, &DecodedChallenge[0], &DecodedChallengeLength,
		NULL, NULL);

	/* prepare input buffer */
	SecBuffer		InSecBuff;
	SecBufferDesc	InBuffDesc;

	if(m_fNewConversation == FALSE)
	{
		InBuffDesc.ulVersion = SECBUFFER_VERSION;
		InBuffDesc.cBuffers  = 1;
		InBuffDesc.pBuffers  = &InSecBuff;

		InSecBuff.cbBuffer   = DecodedChallengeLength;
		InSecBuff.BufferType = SECBUFFER_TOKEN;
		InSecBuff.pvBuffer   = &DecodedChallenge[0];
	}

	/* prepare output buffer */
	SecBuffer			OutSecBuff;
	SecBufferDesc		OutBuffDesc;
	std::vector<BYTE>	outputBuffer(m_dwMaxTokenSize);

	OutBuffDesc.ulVersion = SECBUFFER_VERSION;
	OutBuffDesc.cBuffers  = 1;
	OutBuffDesc.pBuffers  = &OutSecBuff;

	OutSecBuff.cbBuffer   = m_dwMaxTokenSize;
	OutSecBuff.BufferType = SECBUFFER_TOKEN;
	OutSecBuff.pvBuffer   = &outputBuffer[0];

	ULONG			ContextAttributes;
	TimeStamp		Lifetime;

	SECURITY_STATUS	ss = InitializeSecurityContext(
					&m_hCred,
					m_fNewConversation ? NULL : &m_hCtxt,
					TEXT("Coversant.SSPI"),
					0,	// context requirements
					0,	// reserved1
					SECURITY_NATIVE_DREP,
					m_fNewConversation ? NULL : &InBuffDesc,
					0,	// reserved2
					&m_hCtxt,
					&OutBuffDesc,
					&ContextAttributes,
					&Lifetime
					);

	switch(ss)
	{
	case SEC_E_INVALID_HANDLE:
		SSPIError(L"SSPIGenerateReponse()",
			L"InitializeSecurityContext()",
			L"The handle passed to the function is invalid.");
		return E_FAIL;
	case SEC_E_TARGET_UNKNOWN:
		SSPIError(L"SSPIGenerateReponse()",
			L"InitializeSecurityContext()",
			L"The target was not recognized.");
		return E_FAIL; 
	case SEC_E_LOGON_DENIED:
		SSPIError(L"SSPIGenerateReponse()", 
			L"InitializeSecurityContext()", 
			L"The logon failed.");
		return E_FAIL; 
	case SEC_E_INTERNAL_ERROR:
		SSPIError(L"SSPIGenerateReponse()", 
			L"InitializeSecurityContext()", 
			L"The Local Security Authority cannot be contacted.");
		return E_FAIL; 
	case SEC_E_NO_CREDENTIALS:
		SSPIError(L"SSPIGenerateReponse()", 
			L"InitializeSecurityContext()", 
			L"No credentials are available in the security package.");
		return E_FAIL; 
	case SEC_E_NO_AUTHENTICATING_AUTHORITY:
		SSPIError(L"SSPIGenerateReponse()", 
			L"InitializeSecurityContext()", 
			L"No authority could be contacted for authentication. The domain name of the authenticating party could be wrong, the domain could be unreachable, or there might have been a trust relationship failure.");
		return E_FAIL; 
	case SEC_E_INSUFFICIENT_MEMORY:
		SSPIError(L"SSPIGenerateReponse()", 
			L"InitializeSecurityContext()", 
			L"Not enough memory is available to complete the request.");
		return E_FAIL; 
	case SEC_E_INVALID_TOKEN:
		SSPIError(L"SSPIGenerateReponse()", 
			L"InitializeSecurityContext()", 
			L"The error is due to a malformed input token, such as a token corrupted in transit, a token of incorrect size, or a token passed into the wrong security package. Passing a token to the wrong package can happen if the client and server did not negotiate the proper security package.");
		return E_FAIL; 
	case SEC_E_UNSUPPORTED_FUNCTION:
		SSPIError(L"SSPIGenerateReponse()", 
			L"InitializeSecurityContext()", 
			L"An invalid context attribute flag (ISC_REQ_DELEGATE or ISC_REQ_PROMPT_FOR_CREDS) was specified in the fContextReq parameter.");
		return E_FAIL; 
	case SEC_E_WRONG_PRINCIPAL:
		SSPIError(L"SSPIGenerateReponse()", 
			L"InitializeSecurityContext()", 
			L"The principal that received the authentication request is not the same as the one passed into the pszTargetName parameter. This indicates a failure in mutual authentication.");
		return E_FAIL; 
	default:
		m_fHaveCtxtHandle = TRUE;
	}

	// Complete token -- if applicable
	if((SEC_I_COMPLETE_NEEDED == ss) || (SEC_I_COMPLETE_AND_CONTINUE == ss))  
	{
		ss = CompleteAuthToken(&m_hCtxt, &OutBuffDesc);
		switch(ss)
		{
		case SEC_E_INVALID_HANDLE:
			SSPIError(L"SSPIGenerateReponse()", 
				L"CompleteAuthToken()", 
				L" The handle passed to the function is invalid.");
			return E_FAIL;
		case SEC_E_INVALID_TOKEN:
			SSPIError(L"SSPIGenerateReponse()", 
				L"CompleteAuthToken()", 
				L" The token passed to the function is invalid.");
			return E_FAIL;
		case SEC_E_OUT_OF_SEQUENCE:
			SSPIError(L"SSPIGenerateReponse()", 
				L"CompleteAuthToken()", 
				L" The client's security context was located, but the message number is incorrect. This error return is used with the Microsoft Digest SSP.");
			return E_FAIL;
		case SEC_E_MESSAGE_ALTERED:
			SSPIError(L"SSPIGenerateReponse()", 
				L"CompleteAuthToken()", 
				L" The client's security context was located, but the client's message has been tampered with. This error return is used with the Microsoft Digest SSP.");
			return E_FAIL;
		case SEC_E_INTERNAL_ERROR:
			SSPIError(L"SSPIGenerateReponse()", 
				L"CompleteAuthToken()", 
				L" An unidentified internal error has occurred.");
			return E_FAIL;
		default:
			break;
		}
	}

	m_fNewConversation = FALSE;

	DWORD ResponseLen;
	::CryptBinaryToString((BYTE *) OutBuffDesc.pBuffers->pvBuffer,
		OutBuffDesc.pBuffers->cbBuffer,	CRYPT_STRING_BASE64,
		NULL, &ResponseLen);
	std::wstring EncodedResponse(ResponseLen, L'\0');
	::CryptBinaryToString((BYTE *) OutBuffDesc.pBuffers->pvBuffer,
		OutBuffDesc.pBuffers->cbBuffer,	CRYPT_STRING_BASE64,
		&EncodedResponse[0], &ResponseLen);
	*Response = ::SysAllocString(EncodedResponse.c_str());

	*Continue = 
		((SEC_I_CONTINUE_NEEDED == ss) || (SEC_I_COMPLETE_AND_CONTINUE == ss));
	return S_OK;
}
void SASL::HexString(const unsigned char *binaryData, char *hexString, int n)
{
	for(int i = 0; i < n; i++)
	{
		hexString[2 * i] =
			(binaryData[i] >> 4) + 
			(((binaryData[i] >> 4) < 0xA) ? '0' : ('a' - 0xA));
		hexString[2 * i + 1] = (binaryData[i] & 0x0F) + 
			(((binaryData[i] & 0x0F) < 0xA) ? '0' : ('a' - 0xA));
	}
	hexString[2*n] = '\0';
}
void SASL::SSPIError(LPWSTR Where, LPWSTR WhenCalling, LPWSTR ErrorMessage)
{
	std::wostringstream dbgMsg;
	dbgMsg << L"SSPI error in " << Where <<" when calling " << WhenCalling <<
		": " <<	ErrorMessage << std::endl;
	OutputDebugString(dbgMsg.str().c_str());
}