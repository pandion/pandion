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
 * Filename:    XMPPConnectionManager.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#pragma once
#include "Socket.h"
#include "XMPPSendQueue.h"
#include "XMPPXMLParser.h"
#include "SRVLookup.h"

class XMPPConnectionManager
{
private:
	Socket			m_Socket;
	XMPPSendQueue	m_SendQueue;
	XMPPXMLParser	m_XMLParser;
	XMPPHandlers&	m_Handlers;
	XMPPLogger&		m_Logger;

	std::wstring	m_Server;
	unsigned short	m_Port;
	bool			m_UseSSL;

	bool			m_DoStartTLS;
	bool			m_DoStartSC;
	bool			m_DoDisconnect;

	HANDLE          m_CanStartNewThread;

	std::vector<BYTE> m_RecvBuffer;
public:
	XMPPConnectionManager(XMPPHandlers& handlers, XMPPLogger& logger);
	~XMPPConnectionManager();

	void Connect(const std::wstring& server,
		unsigned short port, bool useSSL);
	void Disconnect();
	void StartTLS();
	void StartSC();
	void SendText(const std::wstring& utf16Text);

	std::wstring GetConnectionIP();
private:
	static DWORD __stdcall ConnectionMainProc(void *pThis);
	DWORD ConnectionMain();
	bool DoConnect();
	bool DoConnectWithSRV(SRVLookup& srvLookup);
	bool DoConnectWithoutSRV();
	bool DoRecvData();
	void DoStartTLS();
	void DoStartSC();

	void AsyncSend(BYTE *data, DWORD len);
};
