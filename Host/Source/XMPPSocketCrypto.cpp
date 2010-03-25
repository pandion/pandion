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
 * Filename:    XMPPSocketCrypto.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 - 2010 Dries Staelens
 * Description: This file defines a class that provides cryptography
 *              features to the XMPPSocket class. 
 */

#include "stdafx.h"
#include "XMPPSocketCrypto.h"

XMPPSocketCrypto::XMPPSocketCrypto()
{
}

XMPPSocketCrypto::~XMPPSocketCrypto()
{
}

void XMPPSocketCrypto::OnConnected(SOCKET socket)
{
	m_Socket = socket;
}

void XMPPSocketCrypto::OnDisconnected()
{
	m_PendingEncoded.clear();
	m_Socket = INVALID_SOCKET;
	FreeCredentialsHandle(&m_clientCreds);
	DeleteSecurityContext(&m_context);
}

/*
 * This method attemts to negotiate TLS with the remote server.
 * Returns 0 on failure.
 */
unsigned XMPPSocketCrypto::StartTLS(SOCKET socket)
{
	HCERTSTORE certificateStore = CertOpenSystemStore(NULL, TEXT("MY"));
	if(certificateStore != NULL)
	{
		TimeStamp expiry;
		SCHANNEL_CRED schannelCred = {SCHANNEL_CRED_VERSION, 0, 0, 0, 0,
			0, 0, 0, SP_PROT_SSL3_CLIENT | SP_PROT_TLS1_CLIENT, 0, 0, 0,
			SCH_CRED_AUTO_CRED_VALIDATION | SCH_CRED_NO_SERVERNAME_CHECK, 0};

		SECURITY_STATUS status = AcquireCredentialsHandle(NULL, UNISP_NAME,
			SECPKG_CRED_OUTBOUND, NULL, &schannelCred, NULL, NULL,
			&m_clientCreds, &expiry);
		if(status == SEC_E_OK)
		{
			SecBuffer outBuffers[1] = {0, SECBUFFER_TOKEN, NULL};
			SecBufferDesc outBuffer = {SECBUFFER_VERSION, 1, outBuffers};
			m_contextRequests = ISC_REQ_SEQUENCE_DETECT |
				ISC_REQ_REPLAY_DETECT | ISC_REQ_CONFIDENTIALITY |
				ISC_RET_EXTENDED_ERROR | ISC_REQ_ALLOCATE_MEMORY |
				ISC_REQ_STREAM | ISC_REQ_MANUAL_CRED_VALIDATION;

			status = ::InitializeSecurityContext(&m_clientCreds,
				NULL, NULL, m_contextRequests, 0, SECURITY_NETWORK_DREP,
				NULL, 0, &m_context, &outBuffer, &m_contextAttributes, 
				&expiry);
			if(status == SEC_I_CONTINUE_NEEDED)
			{
				send(socket, (const char*)outBuffers[0].pvBuffer, 
					outBuffers[0].cbBuffer, 0);

				FreeContextBuffer(outBuffers[0].pvBuffer);
				outBuffers[0].pvBuffer = NULL;

				status = ClientHandshakeLoop(true);
				if(status == SEC_E_OK)
				{
					return 1;
				}
			}
		}
	}
	return 0;
}

std::vector<BYTE> XMPPSocketCrypto::Decrypt(std::vector<BYTE>& data)
{
	std::vector<BYTE> decodedData;
	m_PendingEncoded.insert(m_PendingEncoded.end(), data.begin(), data.end());
	if(m_PendingEncoded.size() == 0)
	{
		return data;
	}

	SecPkgContext_StreamSizes sizes;
	SECURITY_STATUS status = ::QueryContextAttributes(&m_context,
		SECPKG_ATTR_STREAM_SIZES, &sizes);

	std::vector<BYTE> ioBuffer;
	DWORD ioBufferSize = sizes.cbHeader + 
		sizes.cbMaximumMessage + sizes.cbTrailer;
	DWORD ioBufferUsed = m_PendingEncoded.size() < ioBufferSize ? 
		m_PendingEncoded.size() : ioBufferSize;
	ioBuffer.insert(ioBuffer.begin(), m_PendingEncoded.begin(),
		m_PendingEncoded.begin() + ioBufferUsed);
	m_PendingEncoded.erase(m_PendingEncoded.begin(),
		m_PendingEncoded.begin() + ioBufferUsed);

	SecBuffer buffers[4] = {{ioBuffer.size(), SECBUFFER_DATA, &ioBuffer[0]},
		{0, SECBUFFER_EMPTY, NULL},
		{0, SECBUFFER_EMPTY ,NULL},
		{0, SECBUFFER_EMPTY, NULL}};
	SecBufferDesc cipherText = {SECBUFFER_VERSION, 4, buffers};

	status = ::DecryptMessage(&m_context, &cipherText, 0, NULL);
	if(status == SEC_E_OK || status == SEC_I_RENEGOTIATE)
	{
		for(int i = 1; i < 4; i++)
		{
			if(buffers[i].BufferType == SECBUFFER_DATA)
			{
				decodedData.insert(decodedData.begin(), 
					(BYTE*) buffers[i].pvBuffer,
					(BYTE*) buffers[i].pvBuffer + buffers[i].cbBuffer);
			}
			else if(buffers[i].BufferType == SECBUFFER_EXTRA)
			{
				m_PendingEncoded.insert(m_PendingEncoded.begin(), 
					(BYTE*) buffers[i].pvBuffer,
					(BYTE*) buffers[i].pvBuffer + buffers[i].cbBuffer);
			}
		}
		if(status == SEC_I_RENEGOTIATE)
		{
			status = ClientHandshakeLoop(false);
		}
	}
	else if(status == SEC_E_INCOMPLETE_MESSAGE)
	{
		m_PendingEncoded.insert(m_PendingEncoded.begin(),
			ioBuffer.begin(), ioBuffer.end());
	}
	else
	{
		OutputDebugString(L"Decryption error\n");
	}
	return decodedData;
}

std::vector<BYTE> XMPPSocketCrypto::Encrypt(std::vector<BYTE>& data)
{
	std::vector<BYTE> encryptedData(0);
	SecPkgContext_StreamSizes streamSizes;
	SECURITY_STATUS status = ::QueryContextAttributes(&m_context,
		SECPKG_ATTR_STREAM_SIZES, &streamSizes);

	DWORD bytesEncrypted = 0;
	while(bytesEncrypted != data.size() && SUCCEEDED(status))
	{
		DWORD messageSize = 
			min(streamSizes.cbMaximumMessage, data.size() - bytesEncrypted);
		std::vector<BYTE>::const_iterator it =
			data.begin() + bytesEncrypted;
		std::vector<BYTE> encryptedMessage =
			TLSEncryptSingleMessage(std::vector<BYTE>(it, it + messageSize));
		encryptedData.insert(encryptedData.end(),
			encryptedMessage.begin(), encryptedMessage.end());
		bytesEncrypted += messageSize;
	}

	return encryptedData;
}

bool XMPPSocketCrypto::Select(bool read, bool write)
{
	return read && !m_PendingEncoded.empty();
}

/*
 * This method makes the connect, send, receive and accept methods block.
 */
void XMPPSocketCrypto::SetBlocking()
{
	DWORD arg = 0;
	ioctlsocket(m_Socket, FIONBIO, &arg);
}

/* 
 * This method makes the connect, send, receive and accept methods work
 * asynchroneous.
 */
void XMPPSocketCrypto::SetNonBlocking()
{
	DWORD arg = 1;
	ioctlsocket(m_Socket, FIONBIO, &arg);
}

/*
 * This method performs the secure handshake loop for the TLS.
 */
SECURITY_STATUS XMPPSocketCrypto::ClientHandshakeLoop(bool initialRead)
{
	SetBlocking();

	BOOL  doRead = initialRead;
	CHAR  ioBuffer[0x10000];
    DWORD ioBufferUsed = 0, ioBufferSize = sizeof(ioBuffer);
    SECURITY_STATUS status = SEC_I_CONTINUE_NEEDED;
	
	while(status == SEC_I_CONTINUE_NEEDED ||
		status == SEC_E_INCOMPLETE_MESSAGE ||
		status == SEC_I_INCOMPLETE_CREDENTIALS)
   {
        if(0 == ioBufferUsed || status == SEC_E_INCOMPLETE_MESSAGE && doRead)
        {
            DWORD dataSize = recv(m_Socket, ioBuffer + ioBufferUsed, 
                          ioBufferSize - ioBufferUsed, 0);
            if(dataSize == SOCKET_ERROR || dataSize == 0)
            {
                status = SEC_E_INTERNAL_ERROR;
                break;
            }
            ioBufferUsed += dataSize;
		}
		else if(0 == ioBufferUsed || status == SEC_E_INCOMPLETE_MESSAGE && !doRead)
		{
			doRead = true;
        }

		TimeStamp expiry;
		SecBuffer inBuffers[2] = 
			{{ioBufferUsed, SECBUFFER_TOKEN, ioBuffer}, 
			{0, SECBUFFER_EMPTY, NULL}};
		SecBufferDesc inBuffer[1] = {SECBUFFER_VERSION, 2, inBuffers};
		SecBuffer outBuffers[1] = {0, SECBUFFER_TOKEN, NULL};
		SecBufferDesc outBuffer[1] = {SECBUFFER_VERSION, 1, outBuffers};
		status = ::InitializeSecurityContext(&m_clientCreds,
			&m_context, NULL, m_contextRequests, 0, SECURITY_NETWORK_DREP,
			inBuffer, 0, NULL, outBuffer, &m_contextAttributes, &expiry);

        if((status == SEC_E_OK || status == SEC_I_CONTINUE_NEEDED ||
            FAILED(status) && (m_contextAttributes & ISC_RET_EXTENDED_ERROR)) &&
			(outBuffers[0].cbBuffer != 0 && outBuffers[0].pvBuffer != NULL))
        {
            DWORD dataSize = send(m_Socket,
				(const char*) outBuffers[0].pvBuffer, outBuffers[0].cbBuffer, 0);
			FreeContextBuffer(outBuffers[0].pvBuffer);
            outBuffers[0].pvBuffer = NULL;

            if(dataSize == SOCKET_ERROR || dataSize == 0)
            {
                DeleteSecurityContext(&m_context);
				SetNonBlocking();
                return SEC_E_INTERNAL_ERROR;
            }
        }
		else if(status == SEC_E_INCOMPLETE_MESSAGE)
        {
            continue;
        }
		else if(FAILED(status))
        {
            break;
        }

        if(status == SEC_E_OK)
        {
			m_PendingEncoded.clear();
			for(unsigned long i = 0; i < outBuffer->cBuffers; i++)
			{
				if(outBuffers[i].BufferType == SECBUFFER_EXTRA)
				{
					m_PendingEncoded.resize(outBuffers[i].cbBuffer);
					std::copy((char*) outBuffers[i].pvBuffer, 
						(char*) outBuffers[i].pvBuffer + outBuffers[i].cbBuffer,
						&m_PendingEncoded.begin()[0]);
				}
			}
            break;
        }

        if(status == SEC_I_INCOMPLETE_CREDENTIALS)
        {
			doRead = false;
			status = SEC_I_CONTINUE_NEEDED;
			continue;
        }

        if (inBuffers[1].BufferType == SECBUFFER_EXTRA)
        {
            MoveMemory(ioBuffer,
				ioBuffer + (ioBufferUsed - inBuffers[1].cbBuffer),
				inBuffers[1].cbBuffer);

            ioBufferUsed = inBuffers[1].cbBuffer;
        }
        else
        {
            ioBufferUsed = 0;
        }
    }

    // Delete the security context in the case of a fatal error.
    if(FAILED(status))
    {
        DeleteSecurityContext(&m_context);
    }

	SetNonBlocking();
    return status;
}

std::vector<BYTE> XMPPSocketCrypto::TLSEncryptSingleMessage(
	std::vector<BYTE>& data)
{
	SecPkgContext_StreamSizes streamSizes;
	SECURITY_STATUS status = ::QueryContextAttributes(&m_context,
		SECPKG_ATTR_STREAM_SIZES, &streamSizes);

	if(FAILED(status))
	{
		return std::vector<BYTE>(0);
	}

	DWORD ioBufferSize = streamSizes.cbHeader + 
		streamSizes.cbMaximumMessage + streamSizes.cbTrailer;
	std::vector<BYTE> ioBuffer(ioBufferSize);
	std::copy(data.begin(), data.end(), &ioBuffer[streamSizes.cbHeader]);

	SecBuffer buffers[4] = {
		{streamSizes.cbHeader, SECBUFFER_STREAM_HEADER, &ioBuffer[0]},
		{data.size(), SECBUFFER_DATA, &ioBuffer[streamSizes.cbHeader]},
		{streamSizes.cbTrailer, SECBUFFER_STREAM_TRAILER,
			&ioBuffer[streamSizes.cbHeader + data.size()]},
		{0, NULL, SECBUFFER_EMPTY }
	};
	SecBufferDesc cipherText = {SECBUFFER_VERSION, 4, buffers};
	status = ::EncryptMessage(&m_context, 0, &cipherText, 0);

	ioBuffer.resize(buffers[0].cbBuffer +
		buffers[1].cbBuffer +
		buffers[2].cbBuffer);
	return ioBuffer;
}