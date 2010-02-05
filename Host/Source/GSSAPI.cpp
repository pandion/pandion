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
#include "SRVLookup.h"

#include <dsgetdc.h>
#include <lm.h>

GSSAPI::GSSAPI() :
	m_fNewConversation(TRUE), m_fHaveCredHandle(FALSE), m_fHaveCtxtHandle(FALSE)
{
	m_CannotSelfDelete = true;
	m_fNewConversation = TRUE;
	m_fHaveCredHandle  = FALSE;
	m_fHaveCtxtHandle  = FALSE;
	m_fInitComplete    = FALSE;

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
		m_hCred.dwLower = m_hCred.dwUpper = 0;
		m_fHaveCredHandle  = FALSE;
	}
	if(m_fHaveCtxtHandle)
	{
		::DeleteSecurityContext(&m_hCtxt);
		m_hCtxt.dwLower = m_hCtxt.dwUpper = 0;
		m_fHaveCtxtHandle  = FALSE;
	}
	m_fNewConversation = TRUE;
	m_fInitComplete = FALSE;
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

	/* Decode the challenge from BASE64 */
	std::vector<BYTE> DecodedChallenge = Base64::Decode(CW2UTF8(Challenge));
	std::vector<BYTE> DecodedResponse;

	if(m_fInitComplete == FALSE)
	{
		DecodedResponse = Initialize(DecodedChallenge, ServerName);
	}
	else
	{
		DecodedResponse = PostInitialize(DecodedChallenge);
	}

	/* Encode the response in BASE64 */
	*Response = ::SysAllocString(Base64::Encode(
		DecodedResponse.size() ? &DecodedResponse[0] : 0,
		DecodedResponse.size(), false).c_str());

	return S_OK;
}
std::vector<BYTE> GSSAPI::Initialize(std::vector<BYTE> decodedChalenge, BSTR ServerName)
{
	/* Prepare input buffer */
	SecBuffer InSecBuff = {decodedChalenge.size(), SECBUFFER_TOKEN, 
		decodedChalenge.size() ? &decodedChalenge[0]: 0};
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
		ISC_REQ_STREAM | ISC_REQ_MUTUAL_AUTH, 0, SECURITY_NATIVE_DREP, 
		m_fNewConversation ? NULL : &InBuffDesc, 0, &m_hCtxt, &OutBuffDesc,
		&ContextAttributes, &Expiry);
	if(ss != SEC_E_OK && ss != SEC_I_CONTINUE_NEEDED &&
		ss != SEC_I_COMPLETE_NEEDED && ss != SEC_I_COMPLETE_AND_CONTINUE)
	{
		Error(L"GenerateResponse()", L"InitializeSecurityContext()", ss);
		return std::vector<BYTE>();
	}
	m_fHaveCtxtHandle = TRUE;

	/* Flag the security context as initialized when init completes */
	if(SEC_E_OK == ss)
	{
		m_fInitComplete = true;
	}

	/* Complete the token when needed */
	else if((SEC_I_COMPLETE_NEEDED == ss) || (SEC_I_COMPLETE_AND_CONTINUE == ss))  
	{
		ss = ::CompleteAuthToken(&m_hCtxt, &OutBuffDesc);
		if(ss != SEC_E_OK)
		{
			Error(L"GenerateResponse()", L"CompleteAuthToken()", ss);
			return std::vector<BYTE>();
		}
	}
	m_fNewConversation = FALSE;

	return std::vector<BYTE>((BYTE*) OutBuffDesc.pBuffers->pvBuffer,
		(BYTE*) OutBuffDesc.pBuffers->pvBuffer + OutBuffDesc.pBuffers->cbBuffer);
}
std::vector<BYTE> GSSAPI::PostInitialize(std::vector<BYTE> decodedChallenge)
{
	unsigned long qop = 0;
	unsigned char inDataBuffer[1024];
	SecBuffer inBuffers[2] = {{1024, SECBUFFER_DATA, inDataBuffer},
		{decodedChallenge.size(), SECBUFFER_STREAM, &decodedChallenge[0]}};
	SecBufferDesc inBuffersDesc = {SECBUFFER_VERSION, 2, inBuffers};

	SECURITY_STATUS ss = ::DecryptMessage(&m_hCtxt, &inBuffersDesc, 0, &qop);
	if(ss != SEC_E_OK)
	{
		Error(L"GenerateResponse()", L"DecryptMessage()", ss);
		return std::vector<BYTE>();
	}

	unsigned char LayerMask = inDataBuffer[0];
	unsigned int MaxMessageSize = 0;
	MaxMessageSize += inDataBuffer[1];
	MaxMessageSize <<= 8;
	MaxMessageSize += inDataBuffer[2];
	MaxMessageSize <<= 8;
	MaxMessageSize += inDataBuffer[3];

	SecPkgContext_Sizes sizes;
	::QueryContextAttributes(&m_hCtxt, SECPKG_ATTR_SIZES, &sizes);

	std::vector<BYTE> tokenBuffer(sizes.cbSecurityTrailer);
	std::vector<BYTE> paddingBuffer(sizes.cbBlockSize);
	unsigned char outDataBuffer[4];
	outDataBuffer[0] = 1;
	outDataBuffer[1] = 0;
	outDataBuffer[2] = 16;
	outDataBuffer[3] = 0;
	SecBuffer outBuffers[3] = {{tokenBuffer.size(), SECBUFFER_TOKEN, &tokenBuffer[0]},
		{4, SECBUFFER_DATA, outDataBuffer},
		{paddingBuffer.size(), SECBUFFER_PADDING, &paddingBuffer[0]}};
	SecBufferDesc outBuffersDesc = {SECBUFFER_VERSION, 3, outBuffers};

	ss = ::EncryptMessage(&m_hCtxt, SECQOP_WRAP_NO_ENCRYPT, &outBuffersDesc, 0);
	if(ss != SEC_E_OK)
	{
		Error(L"GenerateResponse()", L"EncryptMessage()", ss);
		return std::vector<BYTE>();
	}

	std::vector<BYTE> response;
	for(unsigned i = 0; i < outBuffersDesc.cBuffers; i++)
	{
		response.insert(response.end(), (char*) outBuffersDesc.pBuffers[i].pvBuffer,
			(char*) outBuffersDesc.pBuffers[i].pvBuffer +
			outBuffersDesc.pBuffers[i].cbBuffer);
	}

	return response;
}
std::wstring GSSAPI::GenerateServicePrincipalName(std::wstring ServerName)
{
	std::wstring Spn(L"xmpp/");

	PDOMAIN_CONTROLLER_INFO dci;
	DWORD error = ::DsGetDcName(NULL, NULL, NULL, NULL, DS_RETURN_DNS_NAME, &dci);
	if(ERROR_SUCCESS == error)
	{
		std::wstring DomainName(dci->DomainName);
		std::transform(DomainName.begin(), DomainName.end(),
			DomainName.begin(), ::toupper);
		::NetApiBufferFree(dci);

		Spn += GetServerFQDN(ServerName);
		Spn += L"@";
		Spn += DomainName;

		return Spn;
	}
	else
	{
		return std::wstring();
	}
}
std::wstring GSSAPI::GetServerFQDN(std::wstring ServerName)
{
	SRVLookup fqdnLookup = SRVLookup(L"xmpp-client", L"tcp", ServerName);
	if(SUCCEEDED(fqdnLookup.DoLookup()))
	{
		std::vector<SRVRecord> records = fqdnLookup.getRecords();
		return records.begin()->getTargetName();
	}
	else
	{
		return ServerName;
	}
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
	::MessageBox(NULL, dbgMsg.str().c_str(), L"GSSAPI error", MB_OK);
	::LocalFree(ErrorMessage);
}