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

#include <dsgetdc.h>
#include <lm.h>

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
	/* Acquire a credentials handle if this is the initial response */
	if(m_fNewConversation == TRUE)
	{
		HRESULT hr = AcquireCredentials();
		if(FAILED(hr))
		{
			return hr;
		}
	}

	/* Decode the Challenge */
	std::vector<BYTE> DecodedChallenge =
		Base64::Base64Decode(std::wstring(Challenge));

	/* Prepare input buffer */
	SecBuffer InSecBuff = {DecodedChallenge.size(), SECBUFFER_TOKEN, &DecodedChallenge[0]};
	SecBufferDesc InBuffDesc = {SECBUFFER_VERSION, 1, &InSecBuff};

	/* Prepare output buffer */
	std::vector<BYTE> OutputBuffer(m_dwMaxTokenSize);
	SecBuffer OutSecBuff = {m_dwMaxTokenSize, SECBUFFER_TOKEN, &OutputBuffer[0]};
	SecBufferDesc OutBuffDesc = {SECBUFFER_VERSION, 1, &OutSecBuff};

	/* Process the input, generate new output */
	ULONG			ContextAttributes;
	TimeStamp		Expiry;
	std::wstring    Spn = GenerateServicePrincipalName(ServerName);

	SECURITY_STATUS	ss = ::InitializeSecurityContext(&m_hCred,
		m_fNewConversation ? NULL : &m_hCtxt, (SEC_WCHAR*) Spn.c_str(),
		ISC_REQ_MUTUAL_AUTH |  ISC_REQ_STREAM, 0, SECURITY_NATIVE_DREP, 
		m_fNewConversation ? NULL : &InBuffDesc, 0, &m_hCtxt, &OutBuffDesc,
		&ContextAttributes, &Expiry);
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
HRESULT GSSAPI::AcquireCredentials()
{
	TimeStamp Expiry;
	SECURITY_STATUS	ss = ::AcquireCredentialsHandle(NULL, TEXT("Kerberos"),
		SECPKG_CRED_BOTH, NULL, NULL, NULL, NULL, &m_hCred, &Expiry);
	if(ss != SEC_E_OK)
	{
		Error(L"GenerateResponse()", L"AcquireCredentialsHandle()", ss);
	}
	else
	{
		m_fHaveCredHandle = TRUE;
	}
	return HRESULT_FROM_WIN32(ss);
}
std::wstring GSSAPI::GenerateServicePrincipalName(std::wstring ServerName)
{
	std::wstring Spn(L"xmpp/");

	PDOMAIN_CONTROLLER_INFO dci;
	::DsGetDcName(NULL, NULL, NULL, NULL, DS_RETURN_DNS_NAME, &dci);
	std::wstring DomainName(dci->DomainName);
	std::transform(DomainName.begin(), DomainName.end(),
		DomainName.begin(), ::toupper);
	::NetApiBufferFree(dci);

	Spn += ServerName;
	Spn += L"@";
	Spn += DomainName;

	return Spn;
}