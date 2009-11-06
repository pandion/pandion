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
 * Filename:    Socket.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This file declares a class that extends basic socket
 *              functionality. This class can be used for a whole range of
 *              applications.
 */
#pragma once

/*
 * This class provides thread-safe implementation of basic socket functionality
 * extended with compression en TLS encryption.
 */
class Socket
{
private:
	/*
	 * Critical Sections for providing exclusive access to the Socket.
	 */
	CRITICAL_SECTION m_csReading;
	CRITICAL_SECTION m_csWriting;

	/*
	 * The SOCKET handle for the Operating System.
	 */
	SOCKET  m_Socket;

	/*
	 * These objects hold the state of the (de)compression stream.
	 */
	z_stream    compressionStream;
	z_stream    decompressionStream;

	/*
	 * Data that has yet to be decompressed.
	 */
	BYTE*       pendingCompressed;
	bool		decHasMoreInput;

	/*
	 * These members describe the current state of the Socket.
	 */
	BOOL		m_bConnected;
	BOOL		m_bUsingSSL;
	BOOL		m_bUsingSC;

	/*
	 * These objects hold the state of the TLS stream.
	 */
	CredHandle  m_clientCreds;
	CtxtHandle  m_context;
	DWORD       m_contextRequests;
	DWORD       m_contextAttributes;

	/*
	 * Data that has yet to be decrypted.
	 */
	std::vector<char>  m_pendingEncoded;
	std::vector<char>  m_pendingDecoded;
public:
	Socket(SOCKET s = INVALID_SOCKET);
	~Socket();

	DWORD   Connect(_bstr_t ServerAddress, WORD wPort);
	DWORD   Disconnect();

	DWORD	StartTLS();
	DWORD   StartSC();
	
	int     Select(bool bRead, bool bWrite, timeval *tv = NULL);
	BOOL    SetTimeout(DWORD nSeconds);

	USHORT  Listen(WORD nPort);
	Socket* Accept();

	DWORD   Send(BYTE* buf, DWORD nBufLen);
	DWORD	Recv(BYTE* buf, DWORD nBufLen);

	DWORD	SendLine(LPWSTR str);
	DWORD	RecvLine(LPWSTR *str);

	_bstr_t GetRemoteAddress();
	WORD    GetRemotePort();
	_bstr_t GetLocalAddress();
	WORD	GetLocalPort();
	SOCKET	GetHandle();

	void	SetBlocking();
	void	SetNonBlocking();

	/*
	 * Private helper methods
	 */
private:
	SECURITY_STATUS ClientHandshakeLoop(bool initialRead);
	SECURITY_STATUS SecureRecv(PBYTE message,
		DWORD messageSize, PDWORD bytesReceived);
	SECURITY_STATUS SecureSend(PBYTE message, 
		DWORD messageSize, PDWORD bytesSent);

	DWORD   LookupAddress(_bstr_t ServerAddress);
};
