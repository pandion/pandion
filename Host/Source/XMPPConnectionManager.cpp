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
#include "SRVLookup.h"

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
 *
 */
void XMPPConnectionManager::Connect(const std::wstring& server,
			   unsigned short port, bool useSSL, ProxyMethod proxyMethod)
{
	m_Server = server;
	m_Port   = port;
	m_UseSSL = useSSL;

	m_ProxyMethod = proxyMethod;

	DWORD threadID;
	::CreateThread(NULL, 0, AsyncConnectProc, (LPVOID) this, 0, &threadID);
}

/*
 *
 */
void XMPPConnectionManager::Disconnect()
{
	m_DoDisconnect = true;
}

/*
 *
 */
void XMPPConnectionManager::StartTLS()
{
	m_DoStartTLS = true;
}

/*
 *
 */
void XMPPConnectionManager::StartSC()
{
	m_DoStartSC = true;
}

/*
 *
 */
void XMPPConnectionManager::SendText(const std::wstring& utf16Text)
{
	std::string utf8Text = std::string(CW2UTF8(utf16Text.c_str()));
	AsyncSend((LPBYTE) utf8Text.c_str(), utf8Text.length());
	m_Logger.LogSent(utf16Text);
}

/*
 *
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
DWORD __stdcall XMPPConnectionManager::AsyncConnectProc(void *pThis)
{
	::CoInitialize(NULL);
	return ((XMPPConnectionManager*) pThis)->AsyncConnect();
}

/*
 *
 */
DWORD XMPPConnectionManager::AsyncConnect()
{
	SRVLookup theLookup = SRVLookup(_bstr_t(TEXT("xmpp-client")),
		_bstr_t(TEXT("tcp")), _bstr_t(m_Server.c_str()));
	if(SUCCEEDED(theLookup.DoLookup()))
	{
		m_Server = 
			(wchar_t*)(*theLookup.getRecordsIterator()).getTargetName();
		WORD srvport = 
			(*theLookup.getRecordsIterator()).getPort();
		
		if(m_ProxyMethod == ProxyMethodDontUse)
		{
			if(srvport != 0 && !m_UseSSL)
			{
				m_Port = srvport;
			}
		}
	}

	DWORD err = -1;
	if(m_ProxyMethod == ProxyMethodDontUse)
	{
		err = m_Socket.Connect((BSTR) m_Server.c_str(), m_Port);
	}
	else if(m_ProxyMethod == ProxyMethodConnect)
	{
		err = m_Socket.Connect((BSTR) m_ProxyServer.c_str(), m_ProxyPort);
		if(!err)
		{
			ProxyConnect();
		}
	}
	else if(m_ProxyMethod == ProxyMethodPoll)
	{
		err = -1;
	}

	if(!err && m_UseSSL)
	{
		m_Socket.StartTLS();
	}

	if(!err)
	{
		m_Socket.SetNonBlocking();
		m_SendQueue.SetConnected(&m_Socket);
		m_XMLParser.SetConnected();
		m_Handlers.OnConnected();

		std::vector<BYTE> recvBuffer(0x10000);
		m_DoStartTLS = false;
		m_DoStartSC = false;
		m_DoDisconnect = false;

		while(!m_DoDisconnect)
		{
			if(m_DoStartTLS)
			{
				m_DoStartTLS = false;
				if(m_Socket.StartTLS())
				{
					m_Handlers.OnStartTLSSucceeded();
					continue;
				}
				else
				{
					m_Handlers.OnStartTLSFailed();
					continue;
				}
			}
			else if(m_DoStartSC)
			{
				m_DoStartSC = false;
				if(m_Socket.StartSC())
				{
					m_Handlers.OnStartSCSucceeded();
					continue;
				}
				else
				{
					m_Handlers.OnStartSCFailed();
				}
			}
			else
			{
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
						if(m_XMLParser.ParseChunk(recvString) == false)
						{
							break;
						}
					}
					else if(bytesRead == 0)
					{
						/* server disconnected */
						break;
					}
					else /* if(bytesRead < 0) */
					{
						/* socket read error */
						m_Logger.LogReadError();
						break;
					}
				}
				else if(select == 0)
				{
					/* select timed out */
					continue;
				}
				else /* if (select < 0) */
				{
					/* disconnected somewhere else */
					break;
				}
			}
		}
		m_SendQueue.SetDisconnected();
		m_XMLParser.SetDisconnected();
		m_Socket.Disconnect();
	}
	else
	{
//		OutputDebugString(TEXT("XMPP::AsyncConnect(): Could not connect to XMPP server!\n"));
	}
	m_Handlers.OnDisconnected();
	return 0;
}

/* 
 *
 */
void XMPPConnectionManager::ProxyConnect()
{
	WCHAR SendBuffer[4096];
	StringCbPrintfW(SendBuffer, 4096, L"CONNECT %s:%d HTTP/1.1", m_Server, m_Port);
	m_Socket.SendLine(SendBuffer);
	if(m_ProxyUsername.length())
	{
		int len = 1029;
		BYTE strdec[258], strenc[1029];
		StringCbPrintfA((LPSTR) strdec, 258, "%s:%s", CW2A(m_ProxyUsername.c_str()), CW2A(m_ProxyPassword.c_str()));
		Base64Encode(strdec, strlen((LPSTR) strdec), (LPSTR) strenc, &len, ATL_BASE64_FLAG_NOCRLF);
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
			err = 0;
			break;
		}
	}
	if(!err)
	{
		err = -1;
		RecvBuffer[strlen("HTTP/1.0 200 Connection established\r\n")] = 0;
		if(!strcmp((char *)RecvBuffer, "HTTP/1.0 200 Connection established\r\n"))
		{
			err = 0;
		}
	}
}

/*
 *
 */
void XMPPConnectionManager::AsyncSend(BYTE *data, DWORD len)
{
	m_SendQueue.AddData(data, len);
}
