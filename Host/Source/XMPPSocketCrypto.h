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
 * Description: This file declares a class that provides cryptography
 *              features to the XMPPSocket class. 
 */

#pragma once

class XMPPSocketCrypto
{
private:
	/*
	 * These objects hold the state of the TLS stream.
	 */
	CredHandle m_clientCreds;
	CtxtHandle m_context;
	DWORD      m_contextRequests;
	DWORD      m_contextAttributes;

	/*
	 * Data that has yet to be decrypted.
	 */
	std::vector<BYTE> m_PendingEncoded;

	/*
	 * The socket we're working on.
	 */
	SOCKET m_Socket;
public:

	XMPPSocketCrypto();
	~XMPPSocketCrypto();

	void OnConnected(SOCKET socket);
	void OnDisconnected();

	unsigned StartTLS(SOCKET socket);

	std::vector<BYTE> Decrypt(std::vector<BYTE>& data);
	std::vector<BYTE> Encrypt(std::vector<BYTE>& data);

	bool Select(bool write, bool read);

	/*
	 * Private helper methods
	 */
private:
	void SetBlocking();
	void SetNonBlocking();

	SECURITY_STATUS ClientHandshakeLoop(bool initialRead);
	std::vector<BYTE> TLSEncryptSingleMessage(std::vector<BYTE>& data);
};