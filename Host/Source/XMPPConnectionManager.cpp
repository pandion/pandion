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
 * Filename:    XMPPConnectionManager.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#include "stdafx.h"
#include "XMPPConnectionManager.h"
#include "UTF.h"

/*
 * Constructor
 */
XMPPConnectionManager::XMPPConnectionManager(XMPPHandlers& handlers, 
											 XMPPLogger& logger) :
	m_Socket(), 
	m_SendQueue(),
	m_XMLParser(handlers, logger),
	m_Handlers(handlers),
	m_Logger(logger)	
{
	m_DoStartTLS = false;
	m_DoStartSC = false;
	m_DoDisconnect = false;

	m_CanStartNewThread = ::CreateEvent(NULL, TRUE, TRUE, NULL);
}

/*
 * Destructor
 */
XMPPConnectionManager::~XMPPConnectionManager()
{
	::CloseHandle(m_CanStartNewThread);
}

void XMPPConnectionManager::SetProxySettings(
	ProxyMethod method,
	std::wstring server,
	unsigned short port,
	std::wstring username,
	std::wstring password,
	bool useSSL)
{
	m_ProxyMethod = method;
	m_ProxyServer = server;
	m_ProxyPort = port;
	m_ProxyUsername = username;
	m_ProxyPassword = password;
	m_ProxyUseSSL = useSSL;
}

/*
 * Creates a new thread that attempts to connect to the XMPP server.
 */
void XMPPConnectionManager::Connect(const std::wstring& server,
			   unsigned short port, bool useSSL)
{
	::WaitForSingleObject(m_CanStartNewThread, INFINITE);
	::ResetEvent(m_CanStartNewThread);

	m_ServerName = server;
	m_Port   = port;
	m_UseSSL = useSSL;

	DWORD threadID;
	::CreateThread(NULL, 0, ConnectionMainProc, (LPVOID) this, 0, &threadID);
}

/*
 * Flags the connection thread to disconnect.
 */
void XMPPConnectionManager::Disconnect()
{
	if(::WaitForSingleObject(m_CanStartNewThread, 0) != WAIT_OBJECT_0)
	{
		m_DoDisconnect = true;
		m_Handlers.OnDisconnected();
	}
}

/*
 * Flags the connection thread to start negotiating TLS.
 */
void XMPPConnectionManager::StartTLS()
{
	m_DoStartTLS = true;
}

/*
 * Flags the connection thread to initiate stream compression.
 */
void XMPPConnectionManager::StartSC()
{
	m_DoStartSC = true;
}

/*
 * Translates a string to UTF-8 and queues it up for transmission to the XMPP
 * server.
 */
void XMPPConnectionManager::SendText(const std::wstring& utf16Text)
{
	std::string utf8Text = UTF::utf16to8(utf16Text);
	AsyncSend((LPBYTE) utf8Text.c_str(), utf8Text.length());
	m_Logger.LogSent(utf16Text);
}

/*
 * Get the local IP used by the connection with the server.
 */
std::wstring XMPPConnectionManager::GetConnectionIP()
{
	return std::wstring(m_Socket.GetLocalAddress());
}

/*
 * Get the fully qualified domain name of the remote server.
 */
std::wstring XMPPConnectionManager::GetServerFQDN()
{
	return m_ServerName;
}

/*
 *
 */
DWORD __stdcall XMPPConnectionManager::ConnectionMainProc(void *pThis)
{
	::CoInitialize(NULL);
	return ((XMPPConnectionManager*) pThis)->ConnectionMain();
}

/*
 *
 */
DWORD XMPPConnectionManager::ConnectionMain()
{
	m_DoStartTLS = false;
	m_DoStartSC = false;
	m_DoDisconnect = false;

	bool canContinue = DoConnect();

	while(!m_DoDisconnect && canContinue)
	{
		if(m_DoStartTLS)
		{
			m_DoStartTLS = false;
			DoStartTLS();
		}
		else if(m_DoStartSC)
		{
			m_DoStartSC = false;
			DoStartSC();
		}
		else if(canContinue)
		{
			canContinue = DoRecvData();
			
			unsigned nextChar = PopNextCharFromBuffer();
			while(nextChar != -1)
			{
				canContinue = m_XMLParser.ParseChar(nextChar);
				nextChar = PopNextCharFromBuffer();
			}
		}
	}

	m_ServerName = L"";
	m_SendQueue.SetDisconnected();
	m_XMLParser.SetDisconnected();
	m_Socket.Disconnect();

	if(!m_DoDisconnect)
	{
		m_Handlers.OnDisconnected();
	}
	
	::SetEvent(m_CanStartNewThread);

	return 0;
}

bool XMPPConnectionManager::DoConnect()
{
	bool useSRV = false;
	SRVLookup theLookup = SRVLookup(L"xmpp-client", L"tcp", m_ServerName);

	if(SUCCEEDED(theLookup.DoLookup()))
	{
		return DoConnectWithSRV(theLookup);
	}
	else
	{
		return DoConnectWithoutSRV();
	}
}

/*
 *
 */
bool XMPPConnectionManager::DoConnectWithSRV(SRVLookup& srvLookup)
{
	bool success = false;
	std::vector<SRVRecord> vec = srvLookup.getRecords();
	std::vector<SRVRecord>::const_iterator it = vec.begin();
	
	while(!success && it != vec.end())
	{
		m_ServerName = it->getTargetName();
		if(!m_UseSSL && it->getPort())
		{
			m_Port = it->getPort();
		}
		success = DoConnectWithoutSRV();
		it++;
	}

	return success;
}

/*
 *
 */
bool XMPPConnectionManager::DoConnectWithoutSRV()
{
	bool success = m_Socket.Connect((BSTR) m_ServerName.c_str(), m_Port) == 0;

	if(success && !m_DoDisconnect)
	{
		if(m_UseSSL)
		{
			m_Socket.StartTLS();
		}
		m_Socket.SetNonBlocking();
		m_SendQueue.SetConnected(&m_Socket);
		m_XMLParser.SetConnected();
		m_Handlers.OnConnected();
	}

	return success;
}

/*
 *
 */
bool XMPPConnectionManager::DoRecvData()
{
	bool canContinue = false;
	int select = m_Socket.Select(true, false, 0, 10000);
	if(select == 1)
	{
		std::vector<unsigned char> recvBuffer(0x1000);
		int bytesReceived = m_Socket.Recv(recvBuffer);
		if(bytesReceived > 0 && recvBuffer.size() > 0)
		{
			m_DataBuffer.insert(m_DataBuffer.end(),
				recvBuffer.begin(), recvBuffer.end());
			m_Logger.LogReceived(UTF::utf8to16(std::string(
				(char*)&recvBuffer[0], recvBuffer.size())));
			canContinue = true;
		}
		else if(bytesReceived == 0) /* server disconnected */
		{
			canContinue = false;
		}
		else if(::WSAGetLastError() == WSAEWOULDBLOCK)
		{
			canContinue = true;
		}
		else /* if(bytesRead < 0) *//* XMPPSocket read error */
		{
			m_Logger.LogReadError();
		}
	}
	else if(select == 0)
	{
		/* select timed out */
		canContinue = true;
	}
	else /* if (select < 0) *//* disconnected somewhere else */
	{	
		canContinue = false;
	}
	return canContinue;
}

/*
 *
 */
void XMPPConnectionManager::DoStartTLS()
{
	if(m_Socket.StartTLS())
	{
		m_Handlers.OnStartTLSSucceeded();
	}
	else
	{
		m_Handlers.OnStartTLSFailed();
	}
}

/*
 *
 */
void XMPPConnectionManager::DoStartSC()
{
	if(m_Socket.StartSC())
	{
		m_Handlers.OnStartSCSucceeded();
	}
	else
	{
		m_Handlers.OnStartSCFailed();
	}
}

/*
 *
 */
void XMPPConnectionManager::AsyncSend(BYTE *data, DWORD len)
{
	m_SendQueue.AddData(data, len);
}

/*
 *
 */
unsigned XMPPConnectionManager::PopNextCharFromBuffer()
{
	std::vector<unsigned char>::iterator it = m_DataBuffer.begin();
	std::string utf8;
	unsigned codepoint, byteCounter = 0;
	while(it != m_DataBuffer.end())
	{
		byteCounter++;
		utf8.push_back((char)*it);
		codepoint = UTF::getNextUTF8Codepoint(utf8.begin(), utf8.end());
		if(codepoint != INVALID_CODEPOINT)
		{
			for(unsigned i = 0; i < byteCounter; i++)
			{
				m_DataBuffer.erase(m_DataBuffer.begin());
			}
			return codepoint;
		}
		if(byteCounter == 4)
		{
			m_DataBuffer.erase(m_DataBuffer.begin());
			return PopNextCharFromBuffer();
		}
		it++;
	}
	return -1;
}