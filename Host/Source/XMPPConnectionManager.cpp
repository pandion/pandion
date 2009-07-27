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
#include "UTF8.h"

/*
 * Constructor
 */
XMPPConnectionManager::XMPPConnectionManager(XMPPHandlers& handlers, 
											 XMPPLogger& logger) :
	m_Handlers(handlers), m_Logger(logger), m_XMLParser(handlers, logger),
	m_Socket()
{
	m_ProxyMethod = ProxyMethodDontUse;

	m_DoStartTLS = false;
	m_DoStartSC = false;
	m_DoDisconnect = false;
}

/*
 * Destructor
 */
XMPPConnectionManager::~XMPPConnectionManager()
{
}

/*
 * Creates a new thread that attempts to connect to the XMPP server.
 */
void XMPPConnectionManager::Connect(const std::wstring& server,
			   unsigned short port, bool useSSL, ProxyMethod proxyMethod)
{
	m_Server = server;
	m_Port   = port;
	m_UseSSL = useSSL;

	m_ProxyMethod = proxyMethod;

	DWORD threadID;
	::CreateThread(NULL, 0, ConnectionMainProc, (LPVOID) this, 0, &threadID);
}

/*
 * Flags the connection thread to disconnect.
 */
void XMPPConnectionManager::Disconnect()
{
	m_DoDisconnect = true;
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
	std::string utf8Text = std::string(CW2UTF8(utf16Text.c_str()));
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
 *
 */
void XMPPConnectionManager::SetProxyPollURL(std::wstring &pollURL)
{
	m_ProxyMethod = ProxyMethodPoll;
	m_ProxyPollURL = pollURL;
}

/*
 *
 */
void XMPPConnectionManager::SetProxyServer(const std::wstring& server,
	unsigned short port, 
	const std::wstring& username, 
	const std::wstring& password)
{
	m_ProxyMethod = ProxyMethodConnect;
	m_ProxyServer = server;
	m_ProxyPort = port;
	m_ProxyUsername = username;
	m_ProxyPassword = password;
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
		else
		{
			canContinue = DoRecvData();
		}
	}
	m_SendQueue.SetDisconnected();
	m_XMLParser.SetDisconnected();
	m_Socket.Disconnect();
	m_Handlers.OnDisconnected();

	return 0;
}

bool XMPPConnectionManager::DoConnect()
{
	bool useSRV = false;
	SRVLookup theLookup = SRVLookup(L"xmpp-client", L"tcp", m_Server);

	if(SUCCEEDED(theLookup.DoLookup()))
	{
		return DoConnectWithSRV(theLookup);
	}
	else
	{
		return DoConnectWithoutSRV();
	}
}

bool XMPPConnectionManager::DoConnectWithSRV(SRVLookup& srvLookup)
{
	bool success = false;
	std::vector<SRVRecord> vec = srvLookup.getRecords();
	std::vector<SRVRecord>::const_iterator it = vec.begin();
	
	while(!success && it != vec.end())
	{
		m_Server = it->getTargetName();
		if(!m_UseSSL && it->getPort())
		{
			m_Port = it->getPort();
		}
		success = DoConnectWithoutSRV();
		it++;
	}

	return success;
}

bool XMPPConnectionManager::DoConnectWithoutSRV()
{
	bool success;
	if(m_ProxyMethod == ProxyMethodDontUse)
	{
		success = 
			m_Socket.Connect((BSTR) m_Server.c_str(), m_Port) == 0;
	}
	else if(m_ProxyMethod == ProxyMethodConnect)
	{
		success = 
			m_Socket.Connect((BSTR) m_ProxyServer.c_str(), m_ProxyPort) == 0;
		if(success)
		{
			success = ProxyConnect();
		}
	}
	else
	{
		success = false;
	}

	if(success)
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
 * TODO: clean up this mess
 */
bool XMPPConnectionManager::ProxyConnect()
{
	bool success = false;
	WCHAR SendBuffer[4096];
	StringCbPrintfW(SendBuffer, 4096, L"CONNECT %s:%d HTTP/1.1", m_Server, m_Port);
	m_Socket.SendLine(SendBuffer);
	if(m_ProxyUsername.length())
	{
		DWORD len = 1029;
		BYTE strdec[258], strenc[1029];
		StringCbPrintfA((LPSTR) strdec, 258, "%s:%s", CW2UTF8(m_ProxyUsername.c_str()), CW2UTF8(m_ProxyPassword.c_str()));
		/*Base64Encode(strdec, strlen((LPSTR) strdec), (LPSTR) strenc, &len, ATL_BASE64_FLAG_NOCRLF);*/
		::CryptStringToBinaryA((LPCSTR)strdec, strlen((LPSTR) strdec),
			CRYPT_STRING_BASE64, &strenc[0], &len, NULL, NULL);
		strenc[len] = 0;

		StringCbPrintfW(SendBuffer, 4096, L"Proxy-Authorization: Basic %s", strenc);
		m_Socket.SendLine(SendBuffer);
	}
	m_Socket.SendLine(L""); /* send a \r\n */


	unsigned char RecvBuffer[4096];
	int err = -1;
	for(int i = 0; i < 4096; i++)
	{
		if(m_Socket.Recv(RecvBuffer+i, 1) <= 0)
			break;
		if(i > 4 && RecvBuffer[i] == '\n' && RecvBuffer[i-1] == '\r' && RecvBuffer[i-2] == '\n' && RecvBuffer[i-3] == '\r')
		{
			RecvBuffer[strlen("HTTP/1.0 200 Connection established\r\n")] = 0;
			if(!strcmp((char *)RecvBuffer, "HTTP/1.0 200 Connection established\r\n"))
			{
				success = true;
			}
			break;
		}
	}
	return success;
}

bool XMPPConnectionManager::DoRecvData()
{
	static std::vector<BYTE> recvBuffer(0x10000);
	bool canContinue = false;
	timeval tv = { 0, 32 }; // 32ms
	int select = m_Socket.Select(true, false, &tv);
	if(select == 1)
	{
		int bytesRead = m_Socket.Recv(&recvBuffer[0], 
			recvBuffer.size() - 1);
		if(bytesRead > 0)
		{
			recvBuffer[bytesRead] = L'\0';
			std::wstring recvString(
				CUTF82W((char*)&recvBuffer[0]));

			m_Logger.LogReceived(recvString);
			canContinue = m_XMLParser.ParseChunk(recvString);
		}
		else if(bytesRead == 0) /* server disconnected */
		{
			canContinue = false;
		}
		else /* if(bytesRead < 0) *//* socket read error */
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
