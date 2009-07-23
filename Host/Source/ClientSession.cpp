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
 * Filename:    ClientSession.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "ClientSession.h"
#include "Pile.h"
#include "Socket.h"
#include "ProxyInfo.h"
#include "HTTPResponse.h"
#include "ClientGet.h"
#include "File.h"

CClientSession::CClientSession(CPile* pPile, IStream* pERStream, CCSInfo* pCSInfo) :
	CSession(pPile, pERStream), m_pCSInfo(pCSInfo), m_pTransfer(0)
{
	m_pSocket = new Socket();
}
CClientSession::~CClientSession()
{
	delete m_pCSInfo->pFile;
	delete m_pCSInfo;
	delete m_pSocket;
}
DWORD CClientSession::ClientConnect()
{
	USES_CONVERSION;

	DWORD err;
	BSTR proxyAddress;
	m_pCSInfo->pProxyInfo->GetAddress(&proxyAddress);

	if(wcslen(proxyAddress))
		err = m_pSocket->Connect(proxyAddress, m_pCSInfo->pProxyInfo->GetPort());
	else
		err = m_pSocket->Connect(m_pCSInfo->address, m_pCSInfo->port);
	
	switch(err)
	{
	case 0: break;
	case WSAETIMEDOUT:
		m_pER->OnTimeout(m_SessionID);
	default:
		m_pER->OnSockErr(m_SessionID, err);
		return -1;
	}

	_bstr_t remAddress = m_pSocket->GetRemoteAddress();
	m_pER->OnConnected(m_SessionID, remAddress, m_pSocket->GetRemotePort(), m_pSocket->GetLocalPort());

	return 0;
}
void CClientSession::PreCommand()
{
	CSession::PreCommand();
	if(ClientConnect())
	{
		return TerminateThreadInternal();
	}

	switch(m_pCSInfo->method)
	{
	case HTTP_METHOD_GET:
		m_pTransfer = new CClientGet(m_pER, m_pPile, m_pCSInfo, m_pSocket, m_SessionID);
		break;
	case HTTP_METHOD_POST:
	default:
		return TerminateThreadInternal();
	}

	if(m_pTransfer->PreCommand())
		TerminateThreadInternal();
}
void CClientSession::Command()
{
	if(m_pTransfer->Command())
		TerminateThreadInternal();
}
void CClientSession::PostCommand()
{
	if(m_pTransfer)
	{
		m_pTransfer->PostCommand();
		//delete m_pTransfer; // m_pTransfer should delete itself
		m_pTransfer = 0;
	}
	else
	{
		m_pCSInfo->pFile->Seek(0, 0, FILE_BEGIN);
		m_pCSInfo->pFile->SetEOF();
		m_pCSInfo->pFile->Close();
		m_pER->OnTransferAborted(m_SessionID);
	}
	CSession::PostCommand();
}
DWORD CClientSession::GetProgress()
{
	if(m_pTransfer)
		return m_pTransfer->GetProgress();
	return 0;
}
DWORD CClientSession::GetLength()
{
	if(m_pTransfer)
		return m_pTransfer->GetLength();
	return 0;
}
void CClientSession::Terminate()
{
	TerminateThreadInternal();
}
