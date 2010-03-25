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
 * Filename:    XMPPSocket.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 - 2010 Dries Staelens
 * Description: This file declares a class that extends basic socket
 *              functionality.
 */
#pragma once
#include "XMPPSocketCompressor.h"
#include "XMPPSocketCrypto.h"

/*
 * This class provides thread-safe implementation of basic XMPPSocket functionality
 * extended with compression en TLS encryption.
 */
class XMPPSocket
{
private:
	/*
	 * Critical Sections for providing exclusive access to the XMPPSocket.
	 */
	CRITICAL_SECTION m_ReadingSection;
	CRITICAL_SECTION m_WritingSection;

	/*
	 * The socket handle for the Operating System.
	 */
	SOCKET m_Socket;

	/*
	 * These members describe the current state of the Socket.
	 */
	bool m_IsConnected;
	bool m_UsingSSL;
	bool m_UsingSC;

	/*
	 * The object that handles stream (de)compression.
	 */
	XMPPSocketCompressor m_SocketCompressor;

	/*
	 * The object that handles stream (de)cryption.
	 */
	XMPPSocketCrypto m_SocketCrypto;

public:
	XMPPSocket();
	~XMPPSocket();

	DWORD   Connect(_bstr_t ServerAddress, WORD wPort);
	DWORD   Disconnect();

	DWORD	StartTLS();
	DWORD   StartSC();
	
	int     Select(bool bRead, bool bWrite, 
				long seconds = 0, long useconds = 0);

	int		Send(std::vector<BYTE>& data);
	int		Recv(std::vector<BYTE>& data);

	void	SetBlocking();
	void	SetNonBlocking();
	void    SetSendTimeout(DWORD milliseconds);
	void    SetRecvTimeout(DWORD milliseconds);
	void	EnableKeepalive(DWORD timeout, DWORD interval);
	void	DisableKeepalive();

	_bstr_t GetRemoteAddress();
	WORD    GetRemotePort();
	_bstr_t GetLocalAddress();
	WORD	GetLocalPort();

	DWORD   LookupAddress(_bstr_t ServerAddress);
};
