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
 * Filename:    ClientGet.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "ClientGet.h"
#include "HTTPResponse.h"
#include "ClientSession.h"
#include "ProxyInfo.h"
#include "Socket.h"
#include "File.h"

CClientGet::CClientGet(IEventRouter* pER, CPile* pPile, CCSInfo* pCSInfo, Socket* pSocket, DWORD sessionID) :
	CTransfer(pER, pPile, pSocket, sessionID), m_pCSInfo(pCSInfo)
{
	m_nTotalBytesToRecv = 0;
	m_nBytesToRecv = 0;
	m_pResponse = new CHTTPResponse;
}
CClientGet::~CClientGet()
{
	delete m_pResponse;
}
DWORD CClientGet::SendRequest()
{
	WCHAR Buffer[8192];
	BSTR proxyAddress, proxyUsername, proxyPassword;
	m_pCSInfo->pProxyInfo->GetUsername(&proxyUsername);
	m_pCSInfo->pProxyInfo->GetPassword(&proxyPassword);
	m_pCSInfo->pProxyInfo->GetAddress(&proxyAddress);

	if(wcslen(proxyAddress))
	{
		StringCbPrintfW(Buffer, 8192, L"GET http://%s:%u%s HTTP/1.1", m_pCSInfo->address, m_pCSInfo->port, m_pCSInfo->URI);
		m_pSocket->SendLine(Buffer);
		StringCbPrintfW(Buffer, 8192, L"Host: %s:%u", m_pCSInfo->address, m_pCSInfo->port);
		m_pSocket->SendLine(Buffer);

		if(wcslen(proxyUsername))
		{
			int len = 1029;
			BYTE strdec[258], strenc[1029];
			StringCbPrintfA((LPSTR) strdec, 258, "%s:%s", CW2A(proxyUsername), CW2A(proxyPassword));
			Base64Encode(strdec, strlen((LPSTR) strdec), (LPSTR) strenc, &len, ATL_BASE64_FLAG_NOCRLF);
			strenc[len] = 0;

			StringCbPrintfW(Buffer, 8192, L"Proxy-Authorization: Basic %s", strenc);
			m_pSocket->SendLine(Buffer);
		}
	}
	else
	{
		StringCbPrintfW(Buffer, 8192, L"GET %s HTTP/1.1", m_pCSInfo->URI);
		m_pSocket->SendLine(Buffer);
	}

	StringCbPrintfW(Buffer, 8192, L"Host: %s:%u", m_pCSInfo->address, m_pCSInfo->port);
	m_pSocket->SendLine(Buffer);

	if(m_pCSInfo->offset)
	{
		StringCbPrintfW(Buffer, 8192, L"Content-Range: bytes %u-%u/*", m_pCSInfo->offset, unsigned(m_pCSInfo->offset + m_pCSInfo->len)); 
		m_pSocket->SendLine(Buffer);
	}

	m_pSocket->SendLine(L"Connection: close");
	m_pSocket->SendLine(L"");

	SysFreeString(proxyPassword);
	SysFreeString(proxyUsername);
	SysFreeString(proxyAddress);

	return 0;
}
DWORD CClientGet::RecvResponse()
{
	BYTE buf[8192], b;
	DWORD i;

	for(i = 0; i <8191; i++)
	{
		if(m_pSocket->Recv(&b, 1) == SOCKET_ERROR)
		{
			m_pER->OnSockErr(m_SessionID, WSAGetLastError());
			return -1;
		}
		buf[i] = b;
		buf[i+1] = 0;
		if(strstr((char*) buf, "\r\n\r\n"))	break;
	}
	if(i == 8191)	return -1;

	m_pResponse->Parse(buf);
	if(m_pResponse->m_dwCode != 200)
	{
		m_pER->OnHTTPErr(m_SessionID, m_pResponse->m_dwCode);
		return -1;
	}

	return 0;
}
DWORD CClientGet::PreCommand()
{
	if(SendRequest())
		return -1;
	if(RecvResponse())
		return -1;

	_variant_t Content_Length, Content_Range;
	m_pResponse->m_pHeaders->get_Item(&_variant_t(_bstr_t("Content-Length")), &Content_Length);
	if(Content_Length.vt == VT_BSTR)
		m_nBytesToRecv = _wtol(Content_Length.bstrVal);
	else
		m_nBytesToRecv = (DWORD)-1;
	m_pResponse->m_pHeaders->get_Item(&_variant_t(_bstr_t("Content-Range")), &Content_Range);
	if(Content_Range.vt == VT_BSTR)
	{
		swscanf(Content_Range.bstrVal, L"bytes %u-%u/*", &m_fp, &m_nBytesToRecv);
		m_nBytesToRecv -= m_fp;
	}
	m_nTotalBytesToRecv = m_nBytesToRecv;

	m_pCSInfo->pFile->Seek(m_nBytesToRecv, 0, FILE_BEGIN);
	m_pCSInfo->pFile->SetEOF();
	return 0;
}
DWORD CClientGet::Command()
{
	DWORD nBytesReceived = 0, nBytesWritten = 0, err = 0;

	nBytesReceived = m_pSocket->Recv(m_buf, m_nBytesToRecv>= BLOCK_SIZE ? BLOCK_SIZE : m_nBytesToRecv);

	if(nBytesReceived == SOCKET_ERROR)
	{
		if(m_nTotalBytesToRecv == -1)
		{
			m_bAborted = FALSE;
			return 1;
		}

		switch(err = WSAGetLastError())
		{
		case WSAEWOULDBLOCK:
			return 0;
		case WSAETIMEDOUT:
			m_pER->OnTimeout(m_SessionID);
		default:
			m_pER->OnSockErr(m_SessionID, err);
			return err;
		}
	}

	m_pCSInfo->pFile->Seek(m_fp, 0, FILE_BEGIN);
	if(FAILED(m_pCSInfo->pFile->Write(m_buf, nBytesReceived)))
	{
		m_pER->OnFileErr(m_SessionID, GetLastError());
		return 2;
	}

	m_nBytesToRecv -= nBytesReceived;
	m_fp += nBytesReceived;

	if(m_nBytesToRecv == 0)
	{
		m_bAborted = FALSE;
		return -1;
	}
	return 0;
}
DWORD CClientGet::PostCommand()
{
	m_pCSInfo->pFile->Seek(m_fp, 0, FILE_BEGIN);
	m_pCSInfo->pFile->SetEOF();
	m_pCSInfo->pFile->Close();

	if(m_bAborted)
		m_pER->OnTransferAborted(m_SessionID);
	else
		m_pER->OnTransferComplete(m_SessionID);

	delete this;
	return 0;
}
DWORD CClientGet::GetProgress()
{
	return m_fp;
}
DWORD CClientGet::GetLength()
{
	return m_nTotalBytesToRecv;
}
