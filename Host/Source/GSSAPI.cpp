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
#include "UTF.h"
#include "SRVLookup.h"

#include <dsgetdc.h>
#include <lm.h>

GSSAPIException::GSSAPIException(
	std::wstring caller,
	std::wstring callee,
	std::wstring debuginfo,
	unsigned errorCode)
{
	m_Caller = caller;
	m_Callee = callee;
	m_DebugInfo = debuginfo;
	m_ErrorCode = errorCode;
}

std::wstring GSSAPIException::ToString()
{
	LPWSTR msg;
	::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL, m_ErrorCode, 0, (LPTSTR)&msg, 0, NULL);

	std::wostringstream dbgMsg;
	dbgMsg << L"GSSAPI error in " << m_Caller <<
		L" when calling " << m_Callee <<
		std::hex <<	std::setw(8) <<	std::setfill(L'0') << 
		L": (ERROR 0x" << m_ErrorCode << L") " <<
		msg << std::endl;
	::LocalFree(msg);
	return dbgMsg.str() + m_DebugInfo;
}
unsigned GSSAPIException::GetErrorCode()
{
	return m_ErrorCode;
}

GSSAPI::GSSAPI()
{
	m_CannotSelfDelete = true;
	m_NewConversation  = true;
	m_CredHandleValid  = false;
	m_CtxtHandleValid  = false;
	m_InitComplete     = false;

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
		GSSAPIException e = GSSAPIException(L"Reset()",
			L"QuerySecurityPackageInfo()", GenerateDebugInfo(), ss);
		SetLastErrorMessage(e.ToString());
		return HRESULT_FROM_WIN32(e.GetErrorCode());
	}
	m_MaxTokenSize = SecurityPackage->cbMaxToken;
	::FreeContextBuffer(SecurityPackage);

	/* Clean up existing handles */
	if(m_CredHandleValid)
	{
		::FreeCredentialsHandle(&m_CredHandle);
		m_CredHandle.dwLower = m_CredHandle.dwUpper = 0;
		m_CredHandleValid = false;
	}
	if(m_CtxtHandleValid)
	{
		::DeleteSecurityContext(&m_CtxtHandle);
		m_CtxtHandle.dwLower = m_CtxtHandle.dwUpper = 0;
		m_CtxtHandleValid = false;;
	}
	m_NewConversation = true;
	m_InitComplete = false;
	m_ServicePrincipalName.clear();
	m_ServerName.clear();
	m_LastErrorMessage.clear();
	return S_OK;
}
STDMETHODIMP GSSAPI::GenerateResponse(
	BSTR ServerName,
	BSTR Challenge,
	BSTR *Response)
{
	try
	{
		m_ServerName = ServerName;

		/* Acquire a credentials handle if this is the initial response */
		if(m_NewConversation == true)
		{
			AcquireCredentials();
		}

		/* Decode the challenge from BASE64 */
		std::vector<BYTE> DecodedChallenge = 
			Base64::Decode(UTF::utf16to8(Challenge));

		/* Generate the appropriate response */
		std::vector<BYTE> DecodedResponse;
		if(m_InitComplete == false)
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
	catch(GSSAPIException e)
	{
		SetLastErrorMessage(e.ToString());
		return HRESULT_FROM_WIN32(e.GetErrorCode());
	}
}

STDMETHODIMP GSSAPI::GetLastErrorMessage(BSTR* ErrorMessage)
{
	*ErrorMessage = ::SysAllocString(m_LastErrorMessage.c_str());
	return S_OK;
}
void GSSAPI::SetLastErrorMessage(std::wstring ErrorMessage)
{
	m_LastErrorMessage = ErrorMessage;
}
std::wstring GSSAPI::GenerateDebugInfo()
{
	std::wostringstream debugInfo;
	debugInfo << std::endl << 
		std::hex << std::setw(8) << std::setfill(L'0') << 
		L"GSSAPI DEBUG INFORMATION" << std::endl <<
		L"========================" << std::endl << std::endl <<
		L"m_MaxTokenSize: " << m_MaxTokenSize << std::endl <<
		L"m_CredHandleValid: " << m_CredHandleValid << std::endl <<
		L"m_CredHandle.dwLower: " << m_CredHandle.dwLower << std::endl <<
		L"m_CredHandle.dwUpper: " << m_CredHandle.dwUpper << std::endl <<
		L"m_CtxtHandleValid: " << m_CtxtHandleValid << std::endl <<
		L"m_CtxtHandle.dwLower: " << m_CtxtHandle.dwLower << std::endl <<
		L"m_CtxtHandle.dwUpper: " << m_CtxtHandle.dwUpper << std::endl <<
		L"m_NewConversation: " << m_NewConversation << std::endl <<
		L"m_InitComplete: " << m_InitComplete << std::endl <<
		L"m_ServerName: " << m_ServerName << std::endl <<
		L"m_ServicePrincipalName: " << m_ServicePrincipalName << std::endl <<
		std::endl;
	return debugInfo.str();
}
void GSSAPI::AcquireCredentials()
{
	TimeStamp Expiry;
	SECURITY_STATUS	ss = ::AcquireCredentialsHandle(NULL, TEXT("Kerberos"),
		SECPKG_CRED_OUTBOUND, NULL, NULL, NULL, NULL, &m_CredHandle, &Expiry);
	if(ss != SEC_E_OK)
	{
		throw GSSAPIException(L"GenerateResponse()",
			L"AcquireCredentialsHandle()", GenerateDebugInfo(), ss);
	}
	else
	{
		m_CredHandleValid = true;
	}
}
std::vector<BYTE> GSSAPI::Initialize(
	std::vector<BYTE> decodedChalenge,
	BSTR ServerName)
{
	/* Prepare input buffer */
	SecBuffer InSecBuff = {decodedChalenge.size(), SECBUFFER_TOKEN, 
		decodedChalenge.size() ? &decodedChalenge[0]: 0};
	SecBufferDesc InBuffDesc = {SECBUFFER_VERSION, 1, &InSecBuff};

	/* Prepare output buffer */
	std::vector<BYTE> OutputBuffer(m_MaxTokenSize);
	SecBuffer OutSecBuff = {m_MaxTokenSize, SECBUFFER_TOKEN, &OutputBuffer[0]};
	SecBufferDesc OutBuffDesc = {SECBUFFER_VERSION, 1, &OutSecBuff};

	/* Process the input, generate new output */
	ULONG			ContextAttributes;
	TimeStamp		Expiry;
	std::wstring    Spn = GenerateServicePrincipalName(ServerName);

	SECURITY_STATUS	ss = ::InitializeSecurityContext(&m_CredHandle,
		m_NewConversation ? NULL : &m_CtxtHandle, (SEC_WCHAR*) Spn.c_str(),
		ISC_REQ_STREAM | ISC_REQ_MUTUAL_AUTH, 0, SECURITY_NATIVE_DREP, 
		m_NewConversation ? NULL : &InBuffDesc, 0, &m_CtxtHandle,
		&OutBuffDesc, &ContextAttributes, &Expiry);

	m_NewConversation = false;

	if( ss != SEC_E_OK &&
		ss != SEC_I_CONTINUE_NEEDED &&
		ss != SEC_I_COMPLETE_NEEDED &&
		ss != SEC_I_COMPLETE_AND_CONTINUE)
	{
		throw GSSAPIException(L"GenerateResponse()",
			L"InitializeSecurityContext()", GenerateDebugInfo(), ss);
	}
	else if(SEC_E_OK == ss)
	{
		m_CtxtHandleValid = true;
		m_InitComplete = true;
	}
	else if((SEC_I_COMPLETE_NEEDED == ss) ||
		(SEC_I_COMPLETE_AND_CONTINUE == ss))  
	{
		/* Complete the token when needed */
		m_CtxtHandleValid = true;
		ss = ::CompleteAuthToken(&m_CtxtHandle, &OutBuffDesc);
		if(ss != SEC_E_OK)
		{
			throw GSSAPIException(L"GenerateResponse()",
				L"CompleteAuthToken()", GenerateDebugInfo(), ss);
		}
	}
	else
	{
		m_CtxtHandleValid = true;
	}

	return std::vector<BYTE>((BYTE*) OutBuffDesc.pBuffers->pvBuffer,
		(BYTE*) OutBuffDesc.pBuffers->pvBuffer +
		OutBuffDesc.pBuffers->cbBuffer);
}
std::vector<BYTE> GSSAPI::PostInitialize(std::vector<BYTE> decodedChallenge)
{
	unsigned long qop = 0;
	unsigned char inDataBuffer[1024];
	SecBuffer inBuffers[2] = {{1024, SECBUFFER_DATA, inDataBuffer},
		{decodedChallenge.size(), SECBUFFER_STREAM, &decodedChallenge[0]}};
	SecBufferDesc inBuffersDesc = {SECBUFFER_VERSION, 2, inBuffers};

	SECURITY_STATUS ss = 
		::DecryptMessage(&m_CtxtHandle, &inBuffersDesc, 0, &qop);
	if(ss != SEC_E_OK)
	{
		throw GSSAPIException(L"GenerateResponse()",
			L"DecryptMessage()", GenerateDebugInfo(), ss);
	}

	unsigned char LayerMask = inDataBuffer[0];
	unsigned int MaxMessageSize = 0;
	MaxMessageSize += inDataBuffer[1];
	MaxMessageSize <<= 8;
	MaxMessageSize += inDataBuffer[2];
	MaxMessageSize <<= 8;
	MaxMessageSize += inDataBuffer[3];

	SecPkgContext_Sizes sizes;
	::QueryContextAttributes(&m_CtxtHandle, SECPKG_ATTR_SIZES, &sizes);

	std::vector<BYTE> tokenBuffer(sizes.cbSecurityTrailer);
	std::vector<BYTE> paddingBuffer(sizes.cbBlockSize);
	unsigned char outDataBuffer[4];
	outDataBuffer[0] = 1;
	outDataBuffer[1] = 0;
	outDataBuffer[2] = 16;
	outDataBuffer[3] = 0;
	SecBuffer outBuffers[3] = {
		{tokenBuffer.size(), SECBUFFER_TOKEN, &tokenBuffer[0]},
		{4, SECBUFFER_DATA, outDataBuffer},
		{paddingBuffer.size(), SECBUFFER_PADDING, &paddingBuffer[0]}};
	SecBufferDesc outBuffersDesc = {SECBUFFER_VERSION, 3, outBuffers};

	ss = ::EncryptMessage(
		&m_CtxtHandle, SECQOP_WRAP_NO_ENCRYPT, &outBuffersDesc, 0);
	if(ss != SEC_E_OK)
	{
		throw GSSAPIException(L"GenerateResponse()",
			L"EncryptMessage()", GenerateDebugInfo(), ss);
	}

	std::vector<BYTE> response;
	for(unsigned i = 0; i < outBuffersDesc.cBuffers; i++)
	{
		response.insert(response.end(),
			(char*) outBuffersDesc.pBuffers[i].pvBuffer,
			(char*) outBuffersDesc.pBuffers[i].pvBuffer +
			outBuffersDesc.pBuffers[i].cbBuffer);
	}

	return response;
}
std::wstring GSSAPI::GenerateServicePrincipalName(std::wstring ServerName)
{
	std::wstring Spn(L"xmpp/");

	PDOMAIN_CONTROLLER_INFO dci;
	DWORD error = ::DsGetDcName(
		NULL, NULL, NULL, NULL, DS_RETURN_DNS_NAME, &dci);
	if(ERROR_SUCCESS == error)
	{
		std::wstring DomainName(dci->DomainName);
		std::transform(DomainName.begin(), DomainName.end(),
			DomainName.begin(), ::toupper);
		::NetApiBufferFree(dci);

		Spn += ServerName;
		Spn += L"@";
		Spn += DomainName;

		m_ServicePrincipalName = Spn;

		return Spn;
	}
	else
	{
		throw GSSAPIException(L"GenerateServicePrincipalName()",
			L"DsGetDcName()", GenerateDebugInfo(), error);
	}
}