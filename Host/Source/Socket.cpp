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
 * Filename:    Socket.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This file declares a class that extends basic socket
 *              functionality. This class can be used for a whole range of
 *              applications.
 */
#include "stdafx.h"
#include "Socket.h"

/*
 * Constructor for the Socket object
 */
Socket::Socket():
	m_bUsingSSL(0), m_bUsingSC(0), m_bConnected(false)
{
	::InitializeCriticalSection(&m_csReading);
	::InitializeCriticalSection(&m_csWriting);

	m_Socket = ::socket(AF_INET, SOCK_STREAM, 0);
}

/*
 * Destructor for the Socket object
 */
Socket::~Socket()
{
	Disconnect();

	::DeleteCriticalSection(&m_csReading);
	::DeleteCriticalSection(&m_csWriting);
}

/*
 * This method sets up a TCP connection between the socket and a remote host.
 */
DWORD Socket::Connect(_bstr_t ServerAddress, WORD wPort)
{
	EnterCriticalSection(&m_csReading);
	EnterCriticalSection(&m_csWriting);

	EnableKeepalive(30000, 1000);

	sockaddr_in sinRemote;
	sinRemote.sin_family = AF_INET;
	sinRemote.sin_addr.s_addr = LookupAddress(ServerAddress);
	sinRemote.sin_port = htons(wPort);

	int cerror = connect(m_Socket, (sockaddr*)&sinRemote, sizeof(sockaddr_in));

	if(cerror != SOCKET_ERROR)
	{
		m_bConnected = true;
		m_SocketCompressor.OnConnected();
	}

	LeaveCriticalSection(&m_csWriting);
	LeaveCriticalSection(&m_csReading);
	return cerror == SOCKET_ERROR? ::WSAGetLastError() : 0;
}

/*
 * This method disconnects the socket from the remote host.
 */
DWORD Socket::Disconnect()
{
	EnterCriticalSection(&m_csReading);
	EnterCriticalSection(&m_csWriting);

	if(m_Socket == INVALID_SOCKET)
	{
		return 0;
	}
	if(m_bUsingSSL)
	{
		m_bUsingSSL = false;
		FreeCredentialsHandle(&m_clientCreds);
		DeleteSecurityContext(&m_context);
	}
	m_SocketCompressor.OnDisconnected();
	m_bUsingSC = false;

	shutdown(m_Socket, SD_BOTH);
	closesocket(m_Socket);
	m_Socket = INVALID_SOCKET;
	m_bConnected = false;

	if(m_Socket == INVALID_SOCKET)
	{
        m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	}

	LeaveCriticalSection(&m_csWriting);
	LeaveCriticalSection(&m_csReading);

	return ::WSAGetLastError();
}

/*
 * This method initiates Transport Layer Security on the connection.
 */
DWORD Socket::StartTLS()
{
	if(m_bUsingSSL)
	{
		return 1;
	}

	EnterCriticalSection(&m_csReading);
	EnterCriticalSection(&m_csWriting);

	DWORD arg = 0;
	ioctlsocket(m_Socket, FIONBIO, &arg);

	if(m_bConnected)
	{
		HCERTSTORE certificateStore = CertOpenSystemStore(NULL, TEXT("MY"));

		if(certificateStore != NULL)
		{
			TimeStamp expiry;
			SCHANNEL_CRED schannelCred;
			ZeroMemory(&schannelCred, sizeof(schannelCred));
			schannelCred.dwVersion = SCHANNEL_CRED_VERSION;
			schannelCred.grbitEnabledProtocols = SP_PROT_SSL3_CLIENT;
			schannelCred.dwFlags = SCH_CRED_AUTO_CRED_VALIDATION | SCH_CRED_NO_SERVERNAME_CHECK;

			SECURITY_STATUS status = AcquireCredentialsHandle(NULL, 
				UNISP_NAME, SECPKG_CRED_OUTBOUND, NULL, 
				&schannelCred, NULL, NULL, &m_clientCreds, &expiry);

			if(status == SEC_E_OK)
			{
				SecBufferDesc outBuffer;
				SecBuffer outBuffers[1];

				outBuffers[0].pvBuffer = NULL;
				outBuffers[0].cbBuffer = 0;
				outBuffers[0].BufferType = SECBUFFER_TOKEN;

				outBuffer.pBuffers = outBuffers;
				outBuffer.cBuffers = 1;
				outBuffer.ulVersion = SECBUFFER_VERSION;

				m_contextRequests = ISC_REQ_SEQUENCE_DETECT |
					ISC_REQ_REPLAY_DETECT | ISC_REQ_CONFIDENTIALITY |
					ISC_RET_EXTENDED_ERROR | ISC_REQ_ALLOCATE_MEMORY |
					ISC_REQ_STREAM | ISC_REQ_MANUAL_CRED_VALIDATION;

				TimeStamp expiry;
				status = InitializeSecurityContext(&m_clientCreds,
					NULL, NULL, m_contextRequests, 0, SECURITY_NETWORK_DREP,
					NULL, 0, &m_context, &outBuffer, &m_contextAttributes, 
					&expiry);

				if(status == SEC_I_CONTINUE_NEEDED)
				{
					send(m_Socket, (const char*)outBuffers[0].pvBuffer, 
						outBuffers[0].cbBuffer, 0);

					FreeContextBuffer(outBuffers[0].pvBuffer);
					outBuffers[0].pvBuffer = NULL;

					status = ClientHandshakeLoop(true);

					if(status == SEC_E_OK) {
						m_bUsingSSL = true;
						LeaveCriticalSection(&m_csWriting);
						LeaveCriticalSection(&m_csReading);
						return 1;
					}
				}
			}
		}
	}

	arg = 1;
	ioctlsocket(m_Socket, FIONBIO, &arg);

	LeaveCriticalSection(&m_csWriting);
	LeaveCriticalSection(&m_csReading);
	return 0;
}

/*
 * This method performs the secure handshake loop for the TLS.
 */
SECURITY_STATUS Socket::ClientHandshakeLoop(bool initialRead)
{
    SecBufferDesc   inBuffer;
    SecBuffer       inBuffers[2];
    SecBufferDesc   outBuffer;
    SecBuffer       outBuffers[1];

	DWORD           dataSize;
    SECURITY_STATUS status = SEC_I_CONTINUE_NEEDED;
    bool            doRead = initialRead;
	
	const DWORD		ioBufferSize = 0x10000;
    DWORD           ioBufferUsed = 0;
	std::vector<char> ioBuffer(ioBufferSize);

	while(status == SEC_I_CONTINUE_NEEDED        ||
          status == SEC_E_INCOMPLETE_MESSAGE     ||
          status == SEC_I_INCOMPLETE_CREDENTIALS) 
   {
        if(0 == ioBufferUsed || status == SEC_E_INCOMPLETE_MESSAGE)
        {
            if(doRead)
            {
                dataSize = recv(m_Socket, &ioBuffer.begin()[0] + ioBufferUsed, 
                              ioBufferSize - ioBufferUsed, 0);
                if(dataSize == SOCKET_ERROR || dataSize == 0)
                {
                    status = SEC_E_INTERNAL_ERROR;
                    break;
                }
                ioBufferUsed += dataSize;
            }
            else
            {
                doRead = true;
            }
        }
        inBuffers[0].pvBuffer   = &ioBuffer.begin()[0];
        inBuffers[0].cbBuffer   = ioBufferUsed;
        inBuffers[0].BufferType = SECBUFFER_TOKEN;
        inBuffers[1].pvBuffer   = NULL;
        inBuffers[1].cbBuffer   = 0;
        inBuffers[1].BufferType = SECBUFFER_EMPTY;
        inBuffer.cBuffers       = 2;
        inBuffer.pBuffers       = inBuffers;
        inBuffer.ulVersion      = SECBUFFER_VERSION;

        outBuffers[0].pvBuffer  = NULL;
        outBuffers[0].BufferType= SECBUFFER_TOKEN;
        outBuffers[0].cbBuffer  = 0;
        outBuffer.cBuffers      = 1;
        outBuffer.pBuffers      = outBuffers;
        outBuffer.ulVersion     = SECBUFFER_VERSION;

		TimeStamp expiry;
        status = InitializeSecurityContext(&m_clientCreds,
			&m_context, NULL, m_contextRequests, 0, SECURITY_NATIVE_DREP,
			&inBuffer, 0, NULL, &outBuffer, &m_contextAttributes, &expiry);

        if(status == SEC_E_OK                ||
           status == SEC_I_CONTINUE_NEEDED   ||
           FAILED(status) && (m_contextAttributes & ISC_RET_EXTENDED_ERROR))
        {
            if(outBuffers[0].cbBuffer != 0 && outBuffers[0].pvBuffer != NULL)
            {
                dataSize = send(m_Socket, (const char*) outBuffers[0].pvBuffer,
					outBuffers[0].cbBuffer, 0);
				FreeContextBuffer(outBuffers[0].pvBuffer);
                outBuffers[0].pvBuffer = NULL;

                if(dataSize == SOCKET_ERROR || dataSize == 0)
                {
                    DeleteSecurityContext(&m_context);
                    return SEC_E_INTERNAL_ERROR;
                }
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
			m_pendingEncoded.clear();
			m_pendingDecoded.clear();
			for(unsigned long i = 0; i <outBuffer.cBuffers; i++)
			{
				if(outBuffers[i].BufferType == SECBUFFER_EXTRA)
				{
					m_pendingEncoded.resize(outBuffers[i].cbBuffer);
					std::copy((char*) outBuffers[i].pvBuffer, 
						(char*) outBuffers[i].pvBuffer + outBuffers[i].cbBuffer, 
						&m_pendingEncoded.begin()[0]);
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
            MoveMemory(&ioBuffer.begin()[0],
                       &ioBuffer.begin()[0] + (ioBufferUsed - inBuffers[1].cbBuffer),
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

    return status;
}

/*
 * This method initiates compression of the data sent over the connection.
 */
DWORD Socket::StartSC()
{
	m_bUsingSC = true;
	return 1;
}

DWORD Socket::LookupAddress(_bstr_t ServerAddress)
{
	u_long nRemoteAddress = inet_addr(ServerAddress);
	if(nRemoteAddress == INADDR_NONE)
	{
		hostent* pHE = gethostbyname(ServerAddress);
        if(pHE == 0)
		{
           return INADDR_NONE;
		}
		nRemoteAddress = *((u_long*)pHE->h_addr_list[0]);
	}
	return nRemoteAddress;
}

/*
 * This method sends raw data to the remote host.
 */
DWORD Socket::Send(std::vector<BYTE>& data)
{
	EnterCriticalSection(&m_csWriting);
	int iResult;

	if(m_bUsingSC)
	{
		data = m_SocketCompressor.Compress(data);
	}
	if(m_bUsingSSL)
	{
		SECURITY_STATUS status = SecureSend(&data[0],
			data.size(), (DWORD *)&iResult);
	}
	else
	{
		iResult = send(m_Socket, (char*) &data[0], data.size(), 0);
	}

    LeaveCriticalSection(&m_csWriting);
	return iResult;
}


/*
 * This method receives raw data from the remote host.
 */
DWORD Socket::Recv(std::vector<BYTE>& data)
{
	EnterCriticalSection(&m_csReading);
	int iResult;

	if(m_bUsingSSL)
	{
		SecureRecv(&data[0], data.size(), (PDWORD) &iResult);
	}
	else
	{
		iResult = recv(m_Socket, (char*) &data[0], data.size(), 0);
	}
	if(iResult >= 0)
	{
		data.resize(iResult);
		if(m_bUsingSC)
		{
			data = m_SocketCompressor.Decompress(data);
		}
	}
	else
	{
	}

	LeaveCriticalSection(&m_csReading);
	return iResult;
}

/*
 * This method returns the IPv4 address of the remote host this socket is
 * connected to.
 */
_bstr_t Socket::GetRemoteAddress()
{
	sockaddr_in sinRemote;
	int sinRemoteSize = sizeof(sockaddr_in);
	if(getpeername(m_Socket, (sockaddr*)&sinRemote, &sinRemoteSize))
		return "";
	TCHAR tmp[17];
	StringCbPrintf(tmp, 17, TEXT("%d.%d.%d.%d"),
		sinRemote.sin_addr.S_un.S_un_b.s_b1,
		sinRemote.sin_addr.S_un.S_un_b.s_b2,
		sinRemote.sin_addr.S_un.S_un_b.s_b3,
		sinRemote.sin_addr.S_un.S_un_b.s_b4);
	_bstr_t foo = tmp;
	return foo;
}

/*
 * This method returns the port of the remote host this socket is connected to.
 */
WORD Socket::GetRemotePort()
{
	sockaddr_in sinRemote;
	int sinRemoteSize = sizeof(sockaddr_in);
	if(getpeername(m_Socket, (sockaddr*)&sinRemote, &sinRemoteSize)){
		return 0;
	}

	return ntohs(sinRemote.sin_port);
}

/*
 * This method returns the local IPv4 address from which the socket is
 * connecting.
 */
_bstr_t Socket::GetLocalAddress()
{
	sockaddr_in sinLocal;
	int sinLocalSize = sizeof(sockaddr_in);
	if(getsockname(m_Socket, (sockaddr*)&sinLocal, &sinLocalSize))
		return "";
	TCHAR tmp[17];
	StringCbPrintf(tmp, 17, TEXT("%d.%d.%d.%d"),
		sinLocal.sin_addr.S_un.S_un_b.s_b1,
		sinLocal.sin_addr.S_un.S_un_b.s_b2,
		sinLocal.sin_addr.S_un.S_un_b.s_b3,
		sinLocal.sin_addr.S_un.S_un_b.s_b4);
	_bstr_t foo = tmp;
	return foo;
}

/*
 * This method returns the local port from which the socket is connecting.
 */
WORD Socket::GetLocalPort()
{
	sockaddr_in sinLocal;
	int sinLocalSize = sizeof(sockaddr_in);
	if(getsockname(m_Socket, (sockaddr*)&sinLocal, &sinLocalSize))
		return 0;

	return ntohs(sinLocal.sin_port);
}

/*
 * This method makes the connect, send, receive and accept methods block.
 */
void Socket::SetBlocking()
{
	DWORD arg = 0;
	ioctlsocket(m_Socket, FIONBIO, &arg);
}

/* 
 * This method makes the connect, send, receive and accept methods work
 * asynchroneous.
 */
void Socket::SetNonBlocking()
{
	DWORD arg = 1;
	ioctlsocket(m_Socket, FIONBIO, &arg);
}

/*
 * This method sets the send timeout value for the socket.
 */
void Socket::SetSendTimeout(DWORD milliseconds)
{
	setsockopt(m_Socket, SOL_SOCKET, SO_SNDTIMEO,
		(char*) &milliseconds, sizeof(DWORD));
}

/*
 * This method sets the receive timeout value for the socket.
 */
void Socket::SetRecvTimeout(DWORD milliseconds)
{
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO,
		(char*) &milliseconds, sizeof(DWORD));
}

/*
 * Enables TCP keepalive on this socket. This will send a TCP keepalive packet
 * to the remote host every timeout seconds, with an OS determined number of 
 * retries every interval seconds.
 * http://msdn.microsoft.com/en-us/library/dd877220(VS.85).aspx
 */
void Socket::EnableKeepalive(DWORD timeout, DWORD interval)
{
	tcp_keepalive params = {1, timeout, interval};
	DWORD unused;
	::WSAIoctl(m_Socket, SIO_KEEPALIVE_VALS, &params, sizeof(tcp_keepalive),
		NULL, 0, &unused, NULL, NULL);
}

/*
 * Disables TCP keepalive on this socket.
 */
void Socket::DisableKeepalive()
{
	tcp_keepalive params = {0, 0, 0};
	DWORD unused;
	::WSAIoctl(m_Socket, SIO_KEEPALIVE_VALS, &params, sizeof(tcp_keepalive),
		NULL, 0, &unused, NULL, NULL);
}

/*
 * The select function determines the status of the socket, waiting if
 * necessary, to perform synchronous I/O.
 */
int Socket::Select(bool bRead, bool bWrite, long seconds, long useconds)
{
	 /* If we still have some compressed data to be read. */
	if(m_SocketCompressor.Select(bRead, bWrite))
	{
		return 1;
	}

	/*
	 * If we're reading from an SSL socket, see if it's still got buffered
	 * data.
	 */
	if(bRead && m_bUsingSSL &&
		!m_pendingEncoded.empty() || !m_pendingDecoded.empty())
	{
		return 1;
	}

	timeval tv = {seconds, useconds};
	fd_set fdSet;
	FD_ZERO(&fdSet);
	FD_SET(m_Socket, &fdSet);

	return select(FD_SETSIZE,
		bRead ? &fdSet : NULL,
		bWrite ? &fdSet : NULL,
		NULL, &tv);
}

SECURITY_STATUS Socket::SecureRecv(PBYTE message, DWORD messageSize,
								   PDWORD bytesReceived)
{
	if(!m_pendingDecoded.empty())
	{
		if(messageSize < m_pendingDecoded.size())
		{
			std::copy(m_pendingDecoded.begin(),
				m_pendingDecoded.begin() + messageSize,	message);
			*bytesReceived = messageSize;
			std::copy(m_pendingDecoded.begin() + messageSize,
				m_pendingDecoded.end(), m_pendingDecoded.begin());
			m_pendingDecoded.resize(m_pendingDecoded.size() - messageSize);
			return 0;
		}
		else
		{
			std::copy(m_pendingDecoded.begin(),
				m_pendingDecoded.end(), message);
			*bytesReceived = m_pendingDecoded.size();
			m_pendingDecoded.clear();
			return 0;
		}
	}

	*bytesReceived = 0;

	SecPkgContext_StreamSizes sizes;
	SECURITY_STATUS status = QueryContextAttributes(&m_context,
		SECPKG_ATTR_STREAM_SIZES, &sizes);

	DWORD ioBufferSize = sizes.cbHeader + 
		sizes.cbMaximumMessage + sizes.cbTrailer;
	DWORD ioBufferUsed = 0;
	std::vector<char> ioBuffer(ioBufferSize);

    while(TRUE)
    {
		if(!m_pendingEncoded.empty())
		{
			DWORD ioBufferFree = ioBuffer.size() - ioBufferUsed;
			if(m_pendingEncoded.size() > ioBufferFree)
			{
				std::copy(m_pendingEncoded.begin(),
					m_pendingEncoded.begin() + ioBufferFree,
					ioBuffer.begin() + ioBufferUsed);
				ioBufferUsed += ioBufferFree;
				m_pendingEncoded.erase(m_pendingEncoded.begin(), 
					m_pendingEncoded.begin() + ioBufferFree);
			}
			else
			{
				std::copy(m_pendingEncoded.begin(),
					m_pendingEncoded.end(),
					ioBuffer.begin() + ioBufferUsed);
				ioBufferUsed += m_pendingEncoded.size();
				m_pendingEncoded.clear();
			}
		}
        if(0 == ioBufferUsed || status == SEC_E_INCOMPLETE_MESSAGE)
        {
            *bytesReceived = recv(m_Socket,
				&ioBuffer.begin()[0] + ioBufferUsed, 
				ioBufferSize - ioBufferUsed, 0);
			if(*bytesReceived == SOCKET_ERROR)
			{
				if(::WSAGetLastError() != WSAEWOULDBLOCK)
				{
					status = SEC_E_INTERNAL_ERROR;
					break;
				}
				else
				{
					Sleep(1);
				}
            }
            else if(*bytesReceived == 0 && ioBufferUsed)
            {
				return SEC_E_INTERNAL_ERROR;
			}
			else if(*bytesReceived == 0)
			{
				break;
			}
            else
            {
                ioBufferUsed += *bytesReceived;
            }
        }

		SecBuffer               buffers[4];
		buffers[0].pvBuffer     = &ioBuffer.begin()[0];
		buffers[0].cbBuffer     = ioBufferUsed;
		buffers[0].BufferType   = SECBUFFER_DATA;
		buffers[1].pvBuffer     = NULL;
		buffers[1].cbBuffer     = 0;
		buffers[1].BufferType   = SECBUFFER_EMPTY;
		buffers[2].pvBuffer     = NULL;
		buffers[2].cbBuffer     = 0;
		buffers[2].BufferType   = SECBUFFER_EMPTY;
		buffers[3].pvBuffer     = NULL;
		buffers[3].cbBuffer     = 0;
		buffers[3].BufferType   = SECBUFFER_EMPTY;

		SecBufferDesc           cipherText;
		cipherText.ulVersion    = SECBUFFER_VERSION;
		cipherText.cBuffers     = 4;
		cipherText.pBuffers     = buffers;

		status = DecryptMessage(&m_context, &cipherText, 0, NULL);

        if(status == SEC_E_INCOMPLETE_MESSAGE)
        {
            continue;
        }
		else if(status == SEC_I_CONTEXT_EXPIRED)
		{
			break;
		}
		else if(status != SEC_E_OK && status != SEC_I_RENEGOTIATE)
        {
            return status;
        }
		else
		{
			for(int i = 1; i < 4; i++)
			{
				if(buffers[i].BufferType == SECBUFFER_DATA)
				{
					if(messageSize > buffers[i].cbBuffer)
					{
						std::copy((char*) buffers[i].pvBuffer, 
							(char*) buffers[i].pvBuffer + buffers[i].cbBuffer, 
							message);
						*bytesReceived = buffers[i].cbBuffer;
					}
					else
					{
						std::copy((char*) buffers[i].pvBuffer, 
							(char*) buffers[i].pvBuffer + messageSize, 
							message);
						*bytesReceived = messageSize;

						m_pendingDecoded.resize(buffers[i].cbBuffer - messageSize, 0);
						std::copy((char*) buffers[i].pvBuffer + messageSize,
							(char*) buffers[i].pvBuffer + buffers[i].cbBuffer,
							&m_pendingDecoded.begin()[0]);
					}
				}
				else if(buffers[i].BufferType == SECBUFFER_EXTRA)
				{
					m_pendingEncoded.resize(buffers[i].cbBuffer, 0);
					std::copy((char*) buffers[i].pvBuffer, 
						(char*) buffers[i].pvBuffer + buffers[i].cbBuffer,
						&m_pendingEncoded.begin()[0]);
				}
			}
			ioBufferUsed = 0;

			if(status == SEC_I_RENEGOTIATE)
			{
				status = ClientHandshakeLoop(false);
				if(status != SEC_E_OK)
				{
					return status;
				}
				continue;
			}
			break;
		}
    }
	return status;
}

SECURITY_STATUS Socket::SecureSend(PBYTE message, DWORD messageSize,
								   PDWORD bytesSent)
{
	SecPkgContext_StreamSizes sizes;
	SECURITY_STATUS status = QueryContextAttributes(&m_context,
		SECPKG_ATTR_STREAM_SIZES, &sizes);

	if(messageSize > sizes.cbMaximumMessage)
	{
		DWORD bytesSent = 0, d = 0;
		while(bytesSent != messageSize && SUCCEEDED(status))
		{
			if(messageSize - bytesSent > sizes.cbMaximumMessage)
			{
				status = SecureSend(message + bytesSent,
					sizes.cbMaximumMessage, &d);
				bytesSent += sizes.cbMaximumMessage;
			}
			else
			{
				status = SecureSend(message + bytesSent,
					messageSize - bytesSent, &d);
				bytesSent += messageSize - bytesSent;
			}
		}
	}
	else
	{
		DWORD ioBufferSize = sizes.cbHeader + 
			sizes.cbMaximumMessage + sizes.cbTrailer;
		std::vector<char> ioBuffer(ioBufferSize);
		std::copy(message, message + messageSize, &ioBuffer[sizes.cbHeader]);

		SecBuffer buffers[4] = {
			{sizes.cbHeader, SECBUFFER_STREAM_HEADER, &ioBuffer[0]},
			{messageSize, SECBUFFER_DATA, &ioBuffer[sizes.cbHeader]},
			{sizes.cbTrailer, SECBUFFER_STREAM_TRAILER,
				&ioBuffer[sizes.cbHeader + messageSize]},
			{0, NULL, SECBUFFER_EMPTY }
		};
		SecBufferDesc cipherText = {SECBUFFER_VERSION, 4, buffers};

		status = EncryptMessage(&m_context, 0, &cipherText, 0);
		if(FAILED(status))
		{
			return status;
		}

		*bytesSent = send(m_Socket, &ioBuffer[0],
			buffers[0].cbBuffer + 
			buffers[1].cbBuffer + 
			buffers[2].cbBuffer, 0);
		if(*bytesSent == SOCKET_ERROR || *bytesSent == 0)
		{
			return SEC_E_INTERNAL_ERROR;
		}
	}

	return status;
}