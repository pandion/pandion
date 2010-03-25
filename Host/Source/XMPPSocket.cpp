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
 * Filename:    XMPPSocket.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 - 2010 Dries Staelens
 * Description: This file declares a class that extends basic socket
 *              functionality. 
 */
#include "stdafx.h"
#include "XMPPSocket.h"

/*
 * Constructor for the XMPPSocket object
 */
XMPPSocket::XMPPSocket():
	m_UsingSSL(0), m_UsingSC(0), m_IsConnected(false)
{
	::InitializeCriticalSection(&m_ReadingSection);
	::InitializeCriticalSection(&m_WritingSection);

	m_Socket = ::socket(AF_INET, SOCK_STREAM, 0);
}

/*
 * Destructor for the XMPPSocket object
 */
XMPPSocket::~XMPPSocket()
{
	Disconnect();

	::DeleteCriticalSection(&m_ReadingSection);
	::DeleteCriticalSection(&m_WritingSection);
}

/*
 * This method sets up a TCP connection between the XMPPSocket and a remote host.
 */
DWORD XMPPSocket::Connect(_bstr_t ServerAddress, WORD wPort)
{
	EnterCriticalSection(&m_ReadingSection);
	EnterCriticalSection(&m_WritingSection);

	EnableKeepalive(30000, 1000);


	sockaddr_in sinRemote;
	sinRemote.sin_family = AF_INET;
	sinRemote.sin_addr.s_addr = LookupAddress(ServerAddress);
	sinRemote.sin_port = htons(wPort);

	int cerror = connect(m_Socket, (sockaddr*)&sinRemote, sizeof(sockaddr_in));

	if(cerror != SOCKET_ERROR)
	{
		m_IsConnected = true;
		m_SocketCompressor.OnConnected();
		m_SocketCrypto.OnConnected(m_Socket);
	}

	LeaveCriticalSection(&m_WritingSection);
	LeaveCriticalSection(&m_ReadingSection);
	return cerror == SOCKET_ERROR? ::WSAGetLastError() : 0;
}

/*
 * This method disconnects the XMPPSocket from the remote host.
 */
DWORD XMPPSocket::Disconnect()
{
	if(m_Socket == INVALID_SOCKET)
	{
		return 0;
	}

	EnterCriticalSection(&m_ReadingSection);
	EnterCriticalSection(&m_WritingSection);

	if(m_UsingSSL)
	{
		m_UsingSSL = false;
		m_SocketCrypto.OnDisconnected();
	}
	if(m_UsingSC)
	{
		m_UsingSC = false;
		m_SocketCompressor.OnDisconnected();
	}

	m_IsConnected = false;
	::shutdown(m_Socket, SD_BOTH);
	::closesocket(m_Socket);
	m_Socket = socket(AF_INET, SOCK_STREAM, 0);

	LeaveCriticalSection(&m_WritingSection);
	LeaveCriticalSection(&m_ReadingSection);

	return ::WSAGetLastError();
}

/*
 * This method initiates Transport Layer Security on the connection.
 * Returns 0 on failure.
 */
DWORD XMPPSocket::StartTLS()
{
	if(m_UsingSSL || !m_IsConnected)
	{
		return m_IsConnected;
	}

	EnterCriticalSection(&m_ReadingSection);
	EnterCriticalSection(&m_WritingSection);

	DWORD noError = m_SocketCrypto.StartTLS(m_Socket);
	if(noError)
	{
		m_UsingSSL = true;
	}

	LeaveCriticalSection(&m_WritingSection);
	LeaveCriticalSection(&m_ReadingSection);

	return noError;
}

/*
 * This method initiates compression of the data sent over the connection.
 */
DWORD XMPPSocket::StartSC()
{
	SetNonBlocking();
	m_UsingSC = true;
	return 1;
}

DWORD XMPPSocket::LookupAddress(_bstr_t ServerAddress)
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
int XMPPSocket::Send(std::vector<BYTE>& data)
{
	EnterCriticalSection(&m_WritingSection);
	int totalBytesSent = 0;

	if(m_UsingSC)
	{
		data = m_SocketCompressor.Compress(data);
	}
	if(m_UsingSSL)
	{
		data = m_SocketCrypto.Encrypt(data);
	}
	if(data.size())
	{
		while(totalBytesSent != data.size())
		{
			int bytesSent = send(m_Socket,
				(char*) &data[0] + totalBytesSent, data.size(), 0);
			if(bytesSent <= 0)
			{
				totalBytesSent = bytesSent;
				break;
			}
			else
			{
				totalBytesSent += bytesSent;
			}
		}
	}

    LeaveCriticalSection(&m_WritingSection);
	return totalBytesSent;
}


/*
 * This method receives raw data from the remote host.
 */
int XMPPSocket::Recv(std::vector<BYTE>& data)
{
	EnterCriticalSection(&m_ReadingSection);

	int bytesReceived = recv(m_Socket, (char*) &data[0], data.size(), 0);
	if(bytesReceived > 0)
	{
		data.resize(bytesReceived);
	}
	else if(bytesReceived < 0 && ::WSAGetLastError() == WSAEWOULDBLOCK)
	{
		::WSASetLastError(WSAEWOULDBLOCK);
		data.resize(0);
	}
	else
	{
		LeaveCriticalSection(&m_ReadingSection);
		return bytesReceived;
	}

	if(m_UsingSSL)
	{
		data = m_SocketCrypto.Decrypt(data);
	}
	if(m_UsingSC)
	{
		data = m_SocketCompressor.Decompress(data);
	}

	if(data.size())
	{
		bytesReceived = data.size();
	}
	else
	{
		::WSASetLastError(WSAEWOULDBLOCK);
		bytesReceived = -1;
	}

	LeaveCriticalSection(&m_ReadingSection);
	return bytesReceived;
}

/*
 * This method returns the IPv4 address of the remote host this XMPPSocket is
 * connected to.
 */
_bstr_t XMPPSocket::GetRemoteAddress()
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
 * This method returns the port of the remote host this XMPPSocket is connected to.
 */
WORD XMPPSocket::GetRemotePort()
{
	sockaddr_in sinRemote;
	int sinRemoteSize = sizeof(sockaddr_in);
	if(getpeername(m_Socket, (sockaddr*)&sinRemote, &sinRemoteSize)){
		return 0;
	}

	return ntohs(sinRemote.sin_port);
}

/*
 * This method returns the local IPv4 address from which the XMPPSocket is
 * connecting.
 */
_bstr_t XMPPSocket::GetLocalAddress()
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
 * This method returns the local port from which the XMPPSocket is connecting.
 */
WORD XMPPSocket::GetLocalPort()
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
void XMPPSocket::SetBlocking()
{
	DWORD arg = 0;
	ioctlsocket(m_Socket, FIONBIO, &arg);
}

/* 
 * This method makes the connect, send, receive and accept methods work
 * asynchroneous.
 */
void XMPPSocket::SetNonBlocking()
{
	DWORD arg = 1;
	ioctlsocket(m_Socket, FIONBIO, &arg);
}

/*
 * This method sets the send timeout value for the XMPPSocket.
 */
void XMPPSocket::SetSendTimeout(DWORD milliseconds)
{
	setsockopt(m_Socket, SOL_SOCKET, SO_SNDTIMEO,
		(char*) &milliseconds, sizeof(DWORD));
}

/*
 * This method sets the receive timeout value for the XMPPSocket.
 */
void XMPPSocket::SetRecvTimeout(DWORD milliseconds)
{
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO,
		(char*) &milliseconds, sizeof(DWORD));
}

/*
 * Enables TCP keepalive on this XMPPSocket. This will send a TCP keepalive packet
 * to the remote host every timeout seconds, with an OS determined number of 
 * retries every interval seconds.
 * http://msdn.microsoft.com/en-us/library/dd877220(VS.85).aspx
 */
void XMPPSocket::EnableKeepalive(DWORD timeout, DWORD interval)
{
	tcp_keepalive params = {1, timeout, interval};
	DWORD unused;
	::WSAIoctl(m_Socket, SIO_KEEPALIVE_VALS, &params, sizeof(tcp_keepalive),
		NULL, 0, &unused, NULL, NULL);
}

/*
 * Disables TCP keepalive on this XMPPSocket.
 */
void XMPPSocket::DisableKeepalive()
{
	tcp_keepalive params = {0, 0, 0};
	DWORD unused;
	::WSAIoctl(m_Socket, SIO_KEEPALIVE_VALS, &params, sizeof(tcp_keepalive),
		NULL, 0, &unused, NULL, NULL);
}

/*
 * The select function determines the status of the XMPPSocket, waiting if
 * necessary, to perform synchronous I/O.
 */
int XMPPSocket::Select(bool bRead, bool bWrite, long seconds, long useconds)
{
	 /* If we still have some compressed data to be read. */
	if(m_UsingSC && m_SocketCompressor.Select(bRead, bWrite))
	{
		return 1;
	}

	/*
	 * If we're reading from an SSL XMPPSocket, see if it's still got buffered
	 * data.
	 */
	if(m_UsingSSL && m_SocketCrypto.Select(bRead, bWrite))
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
