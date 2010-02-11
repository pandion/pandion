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
 * Filename:    SSPI.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "SSPI.h"
#include "base64.h"
#include "UTF.h"

SSPI::SSPI()
{
	m_CannotSelfDelete = true;
	m_fNewConversation = TRUE;
	m_fHaveCredHandle  = FALSE;
	m_fHaveCtxtHandle  = FALSE;
	Reset();
}
SSPI::~SSPI()
{
}
STDMETHODIMP SSPI::Reset()
{
	/* Query Security Package Information */
	PSecPkgInfo SecurityPackage;
	SECURITY_STATUS ss = 
		::QuerySecurityPackageInfo(L"Negotiate", &SecurityPackage);
	if (ss != SEC_E_OK)
	{
		Error(L"Reset()",
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
STDMETHODIMP SSPI::GenerateResponse(BSTR Challenge, BOOL *Continue, BSTR *Response)
{
	/* Acquire a credentials handle if this is the first response */
	if(m_fNewConversation == TRUE)
	{
		TimeStamp		Lifetime;

		SECURITY_STATUS	ss = AcquireCredentialsHandle(
						NULL,	// principal
						L"Negotiate",
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
			Error(L"GenerateResponse()",
				L"AcquireCredentialsHandle()",
				L"The credentials supplied to the package were not recognized.");
			return E_FAIL;
		case SEC_E_NO_CREDENTIALS:
			Error(L"GenerateResponse()",
				L"AcquireCredentialsHandle()",
				L"No credentials are available in the security package.");
			return E_FAIL;
		case SEC_E_NOT_OWNER:
			Error(L"GenerateResponse()",
				L"AcquireCredentialsHandle()",
				L"The caller of the function does not have the necessary credentials.");
			return E_FAIL;
		case SEC_E_INSUFFICIENT_MEMORY:
			Error(L"GenerateResponse()",
				L"AcquireCredentialsHandle()",
				L"Not enough memory is available to complete the request.");
			return E_FAIL;
		case SEC_E_INTERNAL_ERROR:
			Error(L"GenerateResponse()",
				L"AcquireCredentialsHandle()",
				L"The Local Security Authority (LSA) cannot be contacted.");
			return E_FAIL;
		case SEC_E_SECPKG_NOT_FOUND:
			Error(L"GenerateResponse()",
				L"AcquireCredentialsHandle()",
				L"The requested security package does not exist.");
			return E_FAIL;
		default:
			Error(L"GenerateResponse()",
				L"AcquireCredentialsHandle()",
				L"Unknown Error.");
			return E_FAIL;
		}
	}

	/* Decode the Challenge */
	std::vector<BYTE> DecodedChallenge = 
		Base64::Decode(UTF::utf16to8(Challenge));

	/* prepare input buffer */
	SecBuffer		InSecBuff;
	SecBufferDesc	InBuffDesc;

	if(m_fNewConversation == FALSE)
	{
		InBuffDesc.ulVersion = SECBUFFER_VERSION;
		InBuffDesc.cBuffers  = 1;
		InBuffDesc.pBuffers  = &InSecBuff;

		InSecBuff.cbBuffer   = DecodedChallenge.size();
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
		Error(L"GenerateResponse()",
			L"InitializeSecurityContext()",
			L"The handle passed to the function is invalid.");
		return E_FAIL;
	case SEC_E_TARGET_UNKNOWN:
		Error(L"GenerateResponse()",
			L"InitializeSecurityContext()",
			L"The target was not recognized.");
		return E_FAIL; 
	case SEC_E_LOGON_DENIED:
		Error(L"GenerateResponse()", 
			L"InitializeSecurityContext()", 
			L"The logon failed.");
		return E_FAIL; 
	case SEC_E_INTERNAL_ERROR:
		Error(L"GenerateResponse()", 
			L"InitializeSecurityContext()", 
			L"The Local Security Authority cannot be contacted.");
		return E_FAIL; 
	case SEC_E_NO_CREDENTIALS:
		Error(L"GenerateResponse()", 
			L"InitializeSecurityContext()", 
			L"No credentials are available in the security package.");
		return E_FAIL; 
	case SEC_E_NO_AUTHENTICATING_AUTHORITY:
		Error(L"GenerateResponse()", 
			L"InitializeSecurityContext()", 
			L"No authority could be contacted for authentication. The domain name of the authenticating party could be wrong, the domain could be unreachable, or there might have been a trust relationship failure.");
		return E_FAIL; 
	case SEC_E_INSUFFICIENT_MEMORY:
		Error(L"GenerateResponse()", 
			L"InitializeSecurityContext()", 
			L"Not enough memory is available to complete the request.");
		return E_FAIL; 
	case SEC_E_INVALID_TOKEN:
		Error(L"GenerateResponse()", 
			L"InitializeSecurityContext()", 
			L"The error is due to a malformed input token, such as a token corrupted in transit, a token of incorrect size, or a token passed into the wrong security package. Passing a token to the wrong package can happen if the client and server did not negotiate the proper security package.");
		return E_FAIL; 
	case SEC_E_UNSUPPORTED_FUNCTION:
		Error(L"GenerateResponse()", 
			L"InitializeSecurityContext()", 
			L"An invalid context attribute flag (ISC_REQ_DELEGATE or ISC_REQ_PROMPT_FOR_CREDS) was specified in the fContextReq parameter.");
		return E_FAIL; 
	case SEC_E_WRONG_PRINCIPAL:
		Error(L"GenerateResponse()", 
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
			Error(L"GenerateResponse()", 
				L"CompleteAuthToken()", 
				L" The handle passed to the function is invalid.");
			return E_FAIL;
		case SEC_E_INVALID_TOKEN:
			Error(L"GenerateResponse()", 
				L"CompleteAuthToken()", 
				L" The token passed to the function is invalid.");
			return E_FAIL;
		case SEC_E_OUT_OF_SEQUENCE:
			Error(L"GenerateResponse()", 
				L"CompleteAuthToken()", 
				L" The client's security context was located, but the message number is incorrect. This error return is used with the Microsoft Digest SSP.");
			return E_FAIL;
		case SEC_E_MESSAGE_ALTERED:
			Error(L"GenerateResponse()", 
				L"CompleteAuthToken()", 
				L" The client's security context was located, but the client's message has been tampered with. This error return is used with the Microsoft Digest SSP.");
			return E_FAIL;
		case SEC_E_INTERNAL_ERROR:
			Error(L"GenerateResponse()", 
				L"CompleteAuthToken()", 
				L" An unidentified internal error has occurred.");
			return E_FAIL;
		default:
			break;
		}
	}

	m_fNewConversation = FALSE;

	std::wstring EncodedResponse = Base64::Encode(
		(BYTE*) OutBuffDesc.pBuffers->pvBuffer,
		OutBuffDesc.pBuffers->cbBuffer);
	*Response = ::SysAllocString(EncodedResponse.c_str());

	*Continue = 
		((SEC_I_CONTINUE_NEEDED == ss) || (SEC_I_COMPLETE_AND_CONTINUE == ss));
	return S_OK;
}
void SSPI::Error(LPWSTR Where, LPWSTR WhenCalling, LPWSTR ErrorMessage)
{
	std::wostringstream dbgMsg;
	dbgMsg << L"SSPI error in " << Where <<" when calling " << WhenCalling <<
		": " <<	ErrorMessage << std::endl;
	OutputDebugString(dbgMsg.str().c_str());
}