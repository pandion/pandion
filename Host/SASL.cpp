#include "stdafx.h"

#include "SASL.h"
#include "UTF8.h"
#include "Hash.h"

CSASL::CSASL()
{
	m_fNewConversation = TRUE;
	m_fHaveCredHandle  = FALSE;
	m_fHaveCtxtHandle  = FALSE;
	SSPIReset();
}
CSASL::~CSASL()
{
}
STDMETHODIMP CSASL::PlainGenerateResponse(BSTR jid, 
	BSTR username, BSTR password, BSTR *strBase64)
{
	CComBSTR strRetVal = "";

	DWORD UTF8jidLen = strlen(CW2UTF8(jid));
	DWORD UTF8usernameLen = strlen(CW2UTF8(username));
	DWORD UTF8passwordLen = strlen(CW2UTF8(password));

	DWORD reqBuffersize = UTF8jidLen + UTF8usernameLen + UTF8passwordLen + 2;
	BYTE* pBuffer = new BYTE[reqBuffersize+1];

	memcpy((void*)pBuffer, CW2UTF8(jid), UTF8jidLen);
	*((BYTE *)pBuffer + UTF8jidLen) = '\0';
	memcpy((void*)(pBuffer + UTF8jidLen + 1), CW2UTF8(username), UTF8usernameLen);
	*((BYTE *)pBuffer + UTF8jidLen + 1 + UTF8usernameLen) = '\0';
	memcpy((void*)(pBuffer + UTF8jidLen + 1 + UTF8usernameLen + 1), CW2UTF8(password), UTF8passwordLen);

	int iReqLen = ATL::Base64EncodeGetRequiredLength(reqBuffersize);
	LPSTR pB64Buffer = new char[iReqLen+1];
	if(ATL::Base64Encode(pBuffer, reqBuffersize, pB64Buffer, &iReqLen, ATL_BASE64_FLAG_NOCRLF))
	{
		pB64Buffer[iReqLen] = 0;
		strRetVal = CA2W(pB64Buffer);
	}
	delete pB64Buffer;

	delete pBuffer;
	*strBase64 = strRetVal.Detach();

	return S_OK;
}
STDMETHODIMP CSASL::DigestGenerateResponse(BSTR username, BSTR realm, BSTR password, BSTR nonce, BSTR cnonce, BSTR digest_uri, BSTR nc, BSTR qop, BSTR *strDigest)
{
	/* MD5-Session algorithm:
	var X	= Response('username') + ':' + Response('realm') + ':' + Response('password');
	var Y	= md5.digest(X); // This should be binary data instead of the hexadecimal string
	var A1	= Y + ':' + Response('nonce') + ':' + Response('cnonce'); // + ':' + Response('authzid')
	var A2	= 'AUTHENTICATE:' + Response('digest-uri');
	var HA1	= md5.digest(A1);
	var HA2	= md5.digest(A2);
	var KD	= HA1 + ':' + Response('nonce') + ':' + Response('nc') + ':' + Response('cnonce') + ':' + Response('qop') + ':' + HA2;
	var Z	= md5.digest(KD);
	*/

	/* convert all strings to UTF-8 and store them in new buffers */
	char *U8username     = new char[ strlen(CW2UTF8(username))   + 1 ];
	char *U8realm        = new char[ strlen(CW2UTF8(realm))      + 1 ];
	char *U8password     = new char[ strlen(CW2UTF8(password))   + 1 ];
	char *U8nonce        = new char[ strlen(CW2UTF8(nonce))      + 1 ];
	char *U8cnonce       = new char[ strlen(CW2UTF8(cnonce))     + 1 ];
	char *U8digest_uri   = new char[ strlen(CW2UTF8(digest_uri)) + 1 ];
	char *U8nc           = new char[ strlen(CW2UTF8(nc))         + 1 ];
	char *U8qop          = new char[ strlen(CW2UTF8(qop))        + 1 ];
	StringCchCopyA(U8username,   strlen(CW2UTF8(username))   + 1, CW2UTF8(username));
	StringCchCopyA(U8realm,      strlen(CW2UTF8(realm))      + 1, CW2UTF8(realm));
	StringCchCopyA(U8password,   strlen(CW2UTF8(password))   + 1, CW2UTF8(password));
	StringCchCopyA(U8nonce,      strlen(CW2UTF8(nonce))      + 1, CW2UTF8(nonce));
	StringCchCopyA(U8cnonce,     strlen(CW2UTF8(cnonce))     + 1, CW2UTF8(cnonce));
	StringCchCopyA(U8digest_uri, strlen(CW2UTF8(digest_uri)) + 1, CW2UTF8(digest_uri));
	StringCchCopyA(U8nc,         strlen(CW2UTF8(nc))         + 1, CW2UTF8(nc));
	StringCchCopyA(U8qop,        strlen(CW2UTF8(qop))        + 1, CW2UTF8(qop));

	/* calculate X */
	unsigned int Xlen = strlen(U8username) + 1 + strlen(U8realm) + 1 + strlen(U8password) + 1;
	char *X = new char[ Xlen ];
	StringCbPrintfA(X, Xlen, "%s:%s:%s", U8username, U8realm, U8password);

	/* calculate Y */
	unsigned char Y[16];
	Hash::MD5((unsigned char *) X, strlen(X), Y);

	/* calculate A1 */
	unsigned int A1Len = 16 + 1 + strlen(U8nonce) + 1 + strlen(U8cnonce);
	char *A1 = new char[ A1Len + 1 ];
	memcpy(A1, Y, 16);
	StringCbPrintfA(A1 + 16, A1Len + 1 - 16, ":%s:%s", U8nonce, U8cnonce);

	/* calculate A2 */
	unsigned int A2Len = 13 + strlen(U8digest_uri);
	char *A2 = new char[ A2Len + 1 ];
	StringCbPrintfA(A2, A2Len + 1, "AUTHENTICATE:%s", U8digest_uri);

	/* calculate HA1 */
	unsigned char binaryHA1[16];
	Hash::MD5((unsigned char *) A1, A1Len, binaryHA1);
	char HA1[2*16+1];
	HexString(binaryHA1, HA1, 16);

	/* calculate HA2 */
	unsigned char binaryHA2[16];
	Hash::MD5((unsigned char *) A2, strlen(A2), binaryHA2);
	char HA2[2*16+1];
	HexString(binaryHA2, HA2, 16);

    /* calculate KD */
	unsigned char KDLen = 33 + strlen(U8nonce) + 1 + strlen(U8nc) + 1 + strlen(U8cnonce) + 1 + strlen(U8qop) + 33;
	char *KD = new char[ KDLen + 1 ];
	memcpy(KD, HA1, 32);
	StringCbPrintfA(KD + 32, KDLen + 1 - 32, ":%s:%s:%s:%s:", U8nonce, U8nc, U8cnonce, U8qop);
	memcpy(KD + 32 + strlen(KD + 32), HA2, 33);

	/* calculate Z */
	unsigned char  binaryZ[16];
	Hash::MD5((unsigned char *) KD, KDLen, binaryZ);
	char Z[2*16+1];
	HexString(binaryZ, Z, 16);

	*strDigest = SysAllocString(CA2W(Z));

	/* cleanup */

	delete KD;
	delete A2;
	delete A1;
	delete X;

	delete U8username;
	delete U8realm;
	delete U8password;
	delete U8nonce;
	delete U8cnonce;
	delete U8digest_uri;
	delete U8nc;
	delete U8qop;

	return S_OK;
}
STDMETHODIMP CSASL::HexString(const unsigned char *binaryData, char *hexString, int n)
{
	for(int i = 0; i < n; i++)
	{
		hexString[2*i] = (binaryData[i] >> 4) + (((binaryData[i] >> 4) < 0xA) ? '0' : ('a' - 0xA));
		hexString[2*i+1] = (binaryData[i] & 0x0F) +  (((binaryData[i] & 0x0F) < 0xA) ? '0' : ('a' - 0xA));
	}
	hexString[2*n] = '\0';

	return S_OK;
}
STDMETHODIMP CSASL::SSPIReset()
{
	/* No windows 9x/Me */
	if(!(GetVersion() & 0x80000000))
	{
		/* Query Security Package Information */
		PSecPkgInfo SecurityPackage;
		SECURITY_STATUS ss = QuerySecurityPackageInfo(PACKAGE_NAME,
			&SecurityPackage);
		if (ss != SEC_E_OK)
		{
			SSPIError(L"SSPIReset()", L"QuerySecurityPackageInfo()", L"Unknown Error.");
			return E_FAIL;
		}
		else
		{
			m_dwMaxTokenSize = SecurityPackage->cbMaxToken;
			FreeContextBuffer(SecurityPackage);
		}

		/* Clean up existing handles */
		if(m_fHaveCredHandle)
			FreeCredentialsHandle(&m_hCred);

		/* Reset flags */
		m_fNewConversation = TRUE;
		m_fHaveCredHandle  = FALSE;
		m_fHaveCtxtHandle  = FALSE;
		return S_OK;
	}
	else
		return S_OK;
}
STDMETHODIMP CSASL::SSPIGenerateResponse(BSTR Challenge, BOOL *Continue, BSTR *Response)
{
	/* No windows 9x/Me */
	if(!(GetVersion() & 0x80000000))
	{
		/* Acquire a credentials handle if this is the first response */
		if(m_fNewConversation == TRUE)
		{
			TimeStamp		Lifetime;

			SECURITY_STATUS	ss = AcquireCredentialsHandle (
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
				SSPIError(L"SSPIGenerateReponse()", L"AcquireCredentialsHandle()", L"The credentials supplied to the package were not recognized.");
				return E_FAIL;
			case SEC_E_NO_CREDENTIALS:
				SSPIError(L"SSPIGenerateReponse()", L"AcquireCredentialsHandle()", L"No credentials are available in the security package.");
				return E_FAIL;
			case SEC_E_NOT_OWNER:
				SSPIError(L"SSPIGenerateReponse()", L"AcquireCredentialsHandle()", L"The caller of the function does not have the necessary credentials.");
				return E_FAIL;
			case SEC_E_INSUFFICIENT_MEMORY:
				SSPIError(L"SSPIGenerateReponse()", L"AcquireCredentialsHandle()", L"Not enough memory is available to complete the request.");
				return E_FAIL;
			case SEC_E_INTERNAL_ERROR:
				SSPIError(L"SSPIGenerateReponse()", L"AcquireCredentialsHandle()", L"The Local Security Authority (LSA) cannot be contacted.");
				return E_FAIL;
			case SEC_E_SECPKG_NOT_FOUND:
				SSPIError(L"SSPIGenerateReponse()", L"AcquireCredentialsHandle()", L"The requested security package does not exist.");
				return E_FAIL;
			default:
				SSPIError(L"SSPIGenerateReponse()", L"AcquireCredentialsHandle()", L"Unknown Error.");
				return E_FAIL;
			}
		}

		/* Decode the Challenge */
		DWORD DecodedChallengeLength = ATL::Base64DecodeGetRequiredLength(wcslen(Challenge));
		std::vector<BYTE> DecodedChallenge(DecodedChallengeLength);
		ATL::Base64Decode(CW2A(Challenge), wcslen(Challenge), &DecodedChallenge[0], (int *) &DecodedChallengeLength);

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

		SECURITY_STATUS	ss = InitializeSecurityContext (
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
			SSPIError(L"SSPIGenerateReponse()", L"InitializeSecurityContext()", L"The handle passed to the function is invalid.");
			return E_FAIL;
		case SEC_E_TARGET_UNKNOWN:
			SSPIError(L"SSPIGenerateReponse()", L"InitializeSecurityContext()", L"The target was not recognized.");
			return E_FAIL; 
		case SEC_E_LOGON_DENIED:
			SSPIError(L"SSPIGenerateReponse()", L"InitializeSecurityContext()", L"The logon failed.");
			return E_FAIL; 
		case SEC_E_INTERNAL_ERROR:
			SSPIError(L"SSPIGenerateReponse()", L"InitializeSecurityContext()", L"The Local Security Authority cannot be contacted.");
			return E_FAIL; 
		case SEC_E_NO_CREDENTIALS:
			SSPIError(L"SSPIGenerateReponse()", L"InitializeSecurityContext()", L"No credentials are available in the security package.");
			return E_FAIL; 
		case SEC_E_NO_AUTHENTICATING_AUTHORITY:
			SSPIError(L"SSPIGenerateReponse()", L"InitializeSecurityContext()", L"No authority could be contacted for authentication. The domain name of the authenticating party could be wrong, the domain could be unreachable, or there might have been a trust relationship failure.");
			return E_FAIL; 
		case SEC_E_INSUFFICIENT_MEMORY:
			SSPIError(L"SSPIGenerateReponse()", L"InitializeSecurityContext()", L"Not enough memory is available to complete the request.");
			return E_FAIL; 
		case SEC_E_INVALID_TOKEN:
			SSPIError(L"SSPIGenerateReponse()", L"InitializeSecurityContext()", L"The error is due to a malformed input token, such as a token corrupted in transit, a token of incorrect size, or a token passed into the wrong security package. Passing a token to the wrong package can happen if the client and server did not negotiate the proper security package.");
			return E_FAIL; 
		case SEC_E_UNSUPPORTED_FUNCTION:
			SSPIError(L"SSPIGenerateReponse()", L"InitializeSecurityContext()", L"An invalid context attribute flag (ISC_REQ_DELEGATE or ISC_REQ_PROMPT_FOR_CREDS) was specified in the fContextReq parameter.");
			return E_FAIL; 
		case SEC_E_WRONG_PRINCIPAL:
			SSPIError(L"SSPIGenerateReponse()", L"InitializeSecurityContext()", L"The principal that received the authentication request is not the same as the one passed into the pszTargetName parameter. This indicates a failure in mutual authentication.");
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
				SSPIError(L"SSPIGenerateReponse()", L"CompleteAuthToken()", L" The handle passed to the function is invalid.");
				return E_FAIL;
			case SEC_E_INVALID_TOKEN:
				SSPIError(L"SSPIGenerateReponse()", L"CompleteAuthToken()", L" The token passed to the function is invalid.");
				return E_FAIL;
			case SEC_E_OUT_OF_SEQUENCE:
				SSPIError(L"SSPIGenerateReponse()", L"CompleteAuthToken()", L" The client's security context was located, but the message number is incorrect. This error return is used with the Microsoft Digest SSP.");
				return E_FAIL;
			case SEC_E_MESSAGE_ALTERED:
				SSPIError(L"SSPIGenerateReponse()", L"CompleteAuthToken()", L" The client's security context was located, but the client's message has been tampered with. This error return is used with the Microsoft Digest SSP.");
				return E_FAIL;
			case SEC_E_INTERNAL_ERROR:
				SSPIError(L"SSPIGenerateReponse()", L"CompleteAuthToken()", L" An unidentified internal error has occurred.");
				return E_FAIL;
			default:
				break;
			}
		}

		m_fNewConversation = FALSE;

		DWORD ResponseLen = ATL::Base64EncodeGetRequiredLength(OutBuffDesc.pBuffers->cbBuffer);
		std::vector<char> EncodedResponse(ResponseLen + 1);
		ATL::Base64Encode((BYTE *) OutBuffDesc.pBuffers->pvBuffer, OutBuffDesc.pBuffers->cbBuffer, &EncodedResponse[0], (int *) &ResponseLen, ATL_BASE64_FLAG_NOCRLF);
		EncodedResponse[ResponseLen] = 0;
		*Response = SysAllocString(CA2W(&EncodedResponse[0]));

		*Continue = ((SEC_I_CONTINUE_NEEDED == ss) || (SEC_I_COMPLETE_AND_CONTINUE == ss));
		return S_OK;
	}
	else
		return E_NOTIMPL; /* return error to JScript */
}
void CSASL::SSPIError(LPWSTR Where, LPWSTR WhenCalling, LPWSTR ErrorMessage)
{
	std::wostringstream dbgMsg;
	dbgMsg << L"SSPI error in " << Where << " when calling " << WhenCalling <<
		": " <<	ErrorMessage << std::endl;
	OutputDebugString(CW2T(dbgMsg.str().c_str()));
}