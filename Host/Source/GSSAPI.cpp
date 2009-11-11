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
 * Filename:    GSSAPI.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "GSSAPI.h"
#include "Base64.h"
#include "UTF8.h"

GSSAPI::GSSAPI() :
	m_fNewConversation(TRUE), m_fHaveCredHandle(FALSE), m_fHaveCtxtHandle(FALSE)
{
	m_CannotSelfDelete = true;
	m_fNewConversation = TRUE;
	m_fHaveCredHandle  = FALSE;
	m_fHaveCtxtHandle  = FALSE;
	Reset();
}
GSSAPI::~GSSAPI()
{
}
STDMETHODIMP GSSAPI::Reset()
{
	/* Query Security Package Information */
	PSecPkgInfo SecurityPackage;
	SECURITY_STATUS ss = 
		::QuerySecurityPackageInfo(TEXT("Kerberos"), &SecurityPackage);
	if (ss != SEC_E_OK)
	{
		Error(L"Reset()", L"QuerySecurityPackageInfo()", ss);
		return HRESULT_FROM_WIN32(ss);
	}
	m_dwMaxTokenSize = SecurityPackage->cbMaxToken;
	::FreeContextBuffer(SecurityPackage);

	/* Clean up existing handles */
	if(m_fHaveCredHandle)
	{
		::FreeCredentialsHandle(&m_hCred);
		m_fHaveCredHandle  = FALSE;
	}
	if(m_fHaveCtxtHandle)
	{
		::DeleteSecurityContext(&m_hCtxt);
		m_fHaveCtxtHandle  = FALSE;
	}
	m_fNewConversation = TRUE;
	return S_OK;
}
STDMETHODIMP GSSAPI::GenerateResponse(BSTR ServerName, BSTR Challenge,
									  BSTR *Response)
{
	std::wstring ServiceName(L"xmpp/");
	ServiceName += ServerName;

	/* Acquire a credentials handle if this is the initial response */
	if(m_fNewConversation == TRUE)
	{
		TimeStamp Expiry;
		SECURITY_STATUS	ss = ::AcquireCredentialsHandle(NULL, TEXT("Kerberos"),
						SECPKG_CRED_OUTBOUND, NULL, NULL, NULL, NULL,
						&m_hCred, &Expiry);
		if(ss != SEC_E_OK)
		{
			Error(L"GenerateResponse()", L"AcquireCredentialsHandle()", ss);
			return HRESULT_FROM_WIN32(ss);
		}
		m_fHaveCredHandle = TRUE;
	}

	/* Decode the Challenge */
	DWORD DecodedChallengeLength = 0;
	::CryptStringToBinary(Challenge, ::SysStringLen(Challenge),
		CRYPT_STRING_BASE64, NULL, &DecodedChallengeLength, NULL, NULL);
	std::vector<BYTE> DecodedChallenge(
		DecodedChallengeLength > 0 ? DecodedChallengeLength : 1);
	::CryptStringToBinary(Challenge, ::SysStringLen(Challenge),
		CRYPT_STRING_BASE64, &DecodedChallenge[0], &DecodedChallengeLength,
		NULL, NULL);

	/* Prepare input buffer */
	SecBuffer InSecBuff = {DecodedChallengeLength, SECBUFFER_TOKEN, &DecodedChallenge[0]};
	SecBufferDesc InBuffDesc = {SECBUFFER_VERSION, 1, &InSecBuff};

	/* Prepare output buffer */
	std::vector<BYTE> outputBuffer(m_dwMaxTokenSize);
	SecBuffer OutSecBuff = {m_dwMaxTokenSize, SECBUFFER_TOKEN, &outputBuffer[0]};
	SecBufferDesc OutBuffDesc = {SECBUFFER_VERSION, 1, &OutSecBuff};

	/* Process the input, generate new output */
	ULONG			ContextAttributes;
	TimeStamp		Expiry;

	SECURITY_STATUS	ss = ::InitializeSecurityContext(&m_hCred,
		m_fNewConversation ? NULL : &m_hCtxt, (SEC_WCHAR*) ServiceName.c_str(),
		ISC_REQ_MUTUAL_AUTH, 0, SECURITY_NATIVE_DREP, 
		m_fNewConversation ? NULL : &InBuffDesc,
		0, &m_hCtxt, &OutBuffDesc, &ContextAttributes, &Expiry);
	if(ss != SEC_E_OK && ss != SEC_I_CONTINUE_NEEDED &&
		ss != SEC_I_COMPLETE_NEEDED && ss != SEC_I_COMPLETE_AND_CONTINUE)
	{
		Error(L"GenerateResponse()", L"InitializeSecurityContext()", ss);
		return HRESULT_FROM_WIN32(ss);
	}
	m_fHaveCtxtHandle = TRUE;
	
	/* Complete the token when needed */
	if((SEC_I_COMPLETE_NEEDED == ss) || (SEC_I_COMPLETE_AND_CONTINUE == ss))  
	{
		ss = ::CompleteAuthToken(&m_hCtxt, &OutBuffDesc);
		if(ss != SEC_E_OK)
		{
			Error(L"GenerateResponse()", L"CompleteAuthToken()", ss);
			return HRESULT_FROM_WIN32(ss);
		}
	}
	m_fNewConversation = FALSE;

	/* Encode the response in Base64 */
	*Response = ::SysAllocString(Base64::Base64Encode(
		(BYTE*)OutBuffDesc.pBuffers->pvBuffer,
		OutBuffDesc.pBuffers->cbBuffer, false).c_str());

	return S_OK;
}
void GSSAPI::Error(LPWSTR Where, LPWSTR WhenCalling, DWORD ErrorCode)
{
	LPTSTR ErrorMessage;
	::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL, ErrorCode, 0, (LPTSTR)&ErrorMessage, 0, NULL);
	std::wostringstream dbgMsg;
	dbgMsg << L"GSSAPI error in " << Where <<
		L" when calling " << WhenCalling <<
		std::hex <<	std::setw(8) <<	std::setfill(L'0') << 
		L": (ERROR 0x" << ErrorCode << L") " <<
		ErrorMessage <<	std::endl;
	OutputDebugString(dbgMsg.str().c_str());
	::LocalFree(ErrorMessage);
}