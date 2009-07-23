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
Socket::Socket(SOCKET s):
	m_Socket(s), m_bUsingSSL(0), m_bUsingSC(0), m_bConnected(false),
	pendingCompressed(NULL), decHasMoreInput(false)
{
	InitializeCriticalSection(&m_csReading);
	InitializeCriticalSection(&m_csWriting);

	if(m_Socket == INVALID_SOCKET)
	{
        m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	}
}

/*
 * Destructor for the Socket object
 */
Socket::~Socket()
{
	Disconnect();

	if(m_bUsingSC)
	{
		deflateEnd(&compressionStream);
		inflateEnd(&decompressionStream);
		if(pendingCompressed)
			delete pendingCompressed;
	}
	DeleteCriticalSection(&m_csReading);
	DeleteCriticalSection(&m_csWriting);
}

/*
 * This method sets the network timeout value for the socket in seconds.
 */
BOOL Socket::SetTimeout(DWORD nSeconds)
{
	DWORD dwMSecs = nSeconds * 1000;

	setsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO,
		(char*) &dwMSecs, sizeof(DWORD));
	setsockopt(m_Socket, SOL_SOCKET, SO_SNDTIMEO,
		(char*) &dwMSecs, sizeof(DWORD));

	return true;
}

/*
 * This method sets up a TCP connection between the socket and a remote host.
 */
DWORD Socket::Connect(_bstr_t ServerAddress, WORD wPort)
{
	EnterCriticalSection(&m_csReading);
	EnterCriticalSection(&m_csWriting);
	sockaddr_in sinRemote;
	sinRemote.sin_family = AF_INET;
	sinRemote.sin_addr.s_addr = LookupAddress(ServerAddress);
	sinRemote.sin_port = htons(wPort);

	int cerror = connect(m_Socket, (sockaddr*)&sinRemote, sizeof(sockaddr_in));
	m_bConnected = (cerror != SOCKET_ERROR);

	LeaveCriticalSection(&m_csWriting);
	LeaveCriticalSection(&m_csReading);
	return ::WSAGetLastError();
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

	//FIXME: Second parameter should be SD_BOTH instead of 0x02.
	shutdown(m_Socket, 0x02);
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
					ISC_REQ_STREAM;

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
//TODO: Get new credentials...
//            doRead = false;
//            status = SEC_I_CONTINUE_NEEDED;
//            continue;
			return SEC_E_INTERNAL_ERROR;
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
	EnterCriticalSection(&m_csReading);
	EnterCriticalSection(&m_csWriting);

	compressionStream.zalloc = Z_NULL;
	compressionStream.zfree = Z_NULL;
	compressionStream.opaque = Z_NULL;

	decompressionStream.next_in = Z_NULL;
	decompressionStream.avail_in = 0;
	decompressionStream.zalloc = Z_NULL;
	decompressionStream.zfree = Z_NULL;
	decompressionStream.opaque = Z_NULL;

	if(deflateInit(&compressionStream, 9) == Z_OK &&
		inflateInit(&decompressionStream) == Z_OK)
	{
		m_bUsingSC = true;
		LeaveCriticalSection(&m_csWriting);
		LeaveCriticalSection(&m_csReading);
		return 1;
	}

	LeaveCriticalSection(&m_csWriting);
	LeaveCriticalSection(&m_csReading);
	return 0;
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
 * This method sets the Socket to listen on the specified port.
 */
USHORT Socket::Listen(WORD nPort)
{
	sockaddr_in sinLocal;
	sinLocal.sin_family = AF_INET;
	sinLocal.sin_addr.s_addr = LookupAddress("0.0.0.0");
	sinLocal.sin_port = htons(nPort);

	if(bind(m_Socket, (sockaddr*) &sinLocal, sizeof(sockaddr_in)))
	{
		return 0;
	}
	if(listen(m_Socket, SOMAXCONN))
	{
		return 0;
	}

	int namelen = sizeof(sockaddr);
	getsockname(m_Socket, (sockaddr*) &sinLocal, &namelen);

	return ntohs(sinLocal.sin_port);
}

/*
 * This method accepts an incoming connection if there is one.
 */
Socket* Socket::Accept()
{
	SOCKET s = accept(m_Socket, 0, 0);

	if(s == INVALID_SOCKET)
	{
		return NULL;
	}

	return new Socket(s);
}

/*
 * This method sends raw data to the remote host.
 */
DWORD Socket::Send(BYTE *buf, DWORD nBufLen)
{
	EnterCriticalSection(&m_csWriting);
	int iResult;

	BYTE *outputBuffer = buf;
	DWORD outputSize = nBufLen;
	if(m_bUsingSC)
	{
		compressionStream.next_in = buf;
		compressionStream.avail_in = nBufLen;

		outputSize = nBufLen + nBufLen / 1000 + 12;
		outputBuffer = new BYTE[outputSize];
		compressionStream.next_out = outputBuffer;
		compressionStream.avail_out = outputSize;

		deflate(&compressionStream, Z_SYNC_FLUSH);

		outputSize -= compressionStream.avail_out;
	}
	if(m_bUsingSSL)
	{
		SECURITY_STATUS status = SecureSend(buf, nBufLen, (DWORD *)&iResult);
/*		while(true)
		{
            iResult = SSL_write(m_ssl, outputBuffer, outputSize);
			if(SSL_get_error(m_ssl, iResult) == SSL_ERROR_WANT_WRITE || 
				SSL_get_error(m_ssl, iResult) == SSL_ERROR_WANT_READ)
			{
				Sleep(1);
				continue;
			}
			else
			{
				break;
			}
		}*/
	}
	else
	{
		iResult = send(m_Socket, (char*) outputBuffer, outputSize, 0);
	}
	if(m_bUsingSC)
	{
		delete outputBuffer;
	}

    LeaveCriticalSection(&m_csWriting);
	return iResult;
}


/*
 * This method receives raw data from the remote host.
 */
//TODO: Clean this up a bit
DWORD Socket::Recv(BYTE *buf, DWORD nBufLen)
{
	EnterCriticalSection(&m_csReading);
	int iResult;

	if(pendingCompressed)
	{
		int oldavail_in = decompressionStream.avail_in;
		decompressionStream.avail_out = nBufLen;
		decompressionStream.next_out = buf;

		inflate(&decompressionStream, Z_SYNC_FLUSH);

		if(decompressionStream.avail_in == 0)
		{
			decHasMoreInput = false;
			delete pendingCompressed;
			pendingCompressed = NULL;
		}

		if(oldavail_in != decompressionStream.avail_in)
		{
            return nBufLen - decompressionStream.avail_out;
		}
		else
		{
			decHasMoreInput = false;
			/* nothing was decompressed, queue up some more bytes */
		}
	}
	if(m_bUsingSSL)
	{
		SecureRecv(buf, nBufLen, (PDWORD) &iResult);
	}
	else
	{
		iResult = recv(m_Socket, (char*) buf, nBufLen, 0);
	}
	if(m_bUsingSC && iResult > 0)
	{
		if(pendingCompressed)
		{
			BYTE *oldPendingCompressed = pendingCompressed;
			pendingCompressed = new BYTE[decompressionStream.avail_in+iResult];
			memcpy(pendingCompressed,
				oldPendingCompressed, decompressionStream.avail_in);
			memcpy(pendingCompressed + decompressionStream.avail_in,
				buf, iResult);
			decompressionStream.avail_in =
				decompressionStream.avail_in + iResult;
			delete oldPendingCompressed;
		}
		else
		{
			pendingCompressed = new BYTE[iResult];
			memcpy(pendingCompressed, buf, iResult);
			decompressionStream.avail_in = iResult;
		}

		int oldavail_in = decompressionStream.avail_in;
		decompressionStream.next_in = pendingCompressed;

		decompressionStream.avail_out = nBufLen;
		decompressionStream.next_out = buf;

		inflate(&decompressionStream, Z_SYNC_FLUSH);
		iResult = nBufLen - decompressionStream.avail_out;

		if(decompressionStream.avail_in == 0)
		{
			decHasMoreInput = false;
			delete pendingCompressed;
			pendingCompressed = NULL;
		}
		else
		{
			decHasMoreInput = true;
		}
	}

	LeaveCriticalSection(&m_csReading);
	return iResult;
}

/*
 * This method converts the string in the parameter to UTF-8 encoding, adds a
 * \r\n line delimiter and sens it to the remote host.
 */
DWORD Socket::SendLine(LPWSTR strLine)
{
	DWORD reqBufSize = ::WideCharToMultiByte(CP_UTF8, 0, 
		strLine, wcslen(strLine), NULL, 0, NULL, NULL);
	char *strUTF8 = new char[reqBufSize+2];
	::WideCharToMultiByte(CP_UTF8, 0, 
		strLine, wcslen(strLine), strUTF8, reqBufSize, NULL, NULL);
	strUTF8[reqBufSize] = '\r';
	strUTF8[reqBufSize+1] = '\n';

	DWORD retVal = Send((BYTE *)strUTF8, reqBufSize+2);

	delete strUTF8;
	return retVal;
}
/*
 * This method receives a \r\n delimited line from the remote host. The line is
 * assumed to be UTF-8 encoded and is subsequently transcoded into UTF-16.
 */
//TODO: implement me.
DWORD Socket::RecvLine(LPWSTR *str)
{
	return 0;
}

/*
 * This method returns the IPv4 address of the remote host this socket is
 * conntected to.
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
 * This method returns the OS handle to the socket.
 */
//TODO: outphase this method
SOCKET Socket::GetHandle()
{
	return m_Socket;
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
 * The select function determines the status of the socket, waiting if
 * necessary, to perform synchronous I/O.
 */
int Socket::Select(bool bRead, bool bWrite, timeval *tv)
{
	 // If we still have some compressed data to be read.
	if(bRead && decHasMoreInput)
	{
		return 1;
	}
	// If we're reading from an SSL socket, see if it's still got buffered
	// data.
	if(bRead && m_bUsingSSL && !m_pendingEncoded.empty())
	{
		return 1;
	}

	fd_set fdSet;
	FD_ZERO(&fdSet);
	FD_SET(m_Socket, &fdSet);

	return select(FD_SETSIZE,
		bRead ? &fdSet : NULL,
		bWrite ? &fdSet : NULL,
		NULL, tv);
}

SECURITY_STATUS Socket::SecureRecv(PBYTE message, DWORD messageSize,
								   PDWORD bytesReceived)
{
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
                status = SEC_E_INTERNAL_ERROR;
                break;
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
			for(int i = 1; i <4; i++)
			{
				if(buffers[i].BufferType == SECBUFFER_DATA)
				{
					/* FIXME: account for message size */
					std::copy((char*) buffers[i].pvBuffer, 
						(char*) buffers[i].pvBuffer + buffers[i].cbBuffer, 
						message);
					*bytesReceived = buffers[i].cbBuffer;
				}
				else if(buffers[i].BufferType == SECBUFFER_EXTRA)
				{
					m_pendingEncoded.resize(
						m_pendingEncoded.size() + buffers[i].cbBuffer, 0);
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

	// FIXME: allow for larger messages to be sent in multiple rounds
	if(sizes.cbMaximumMessage <messageSize)
	{
		return SEC_E_INTERNAL_ERROR;
	}

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
		buffers[0].cbBuffer + buffers[1].cbBuffer + buffers[2].cbBuffer, 0);
	if(*bytesSent == SOCKET_ERROR || *bytesSent == 0)
    {
        return SEC_E_INTERNAL_ERROR;
    }

	return status;
}