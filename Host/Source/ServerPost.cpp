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
 * Filename:    ServerPost.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "Transfer.h"
#include "ServerPost.h"
#include "Socket.h"
#include "HTTPRequest.h"
#include "File.h"

CServerPost::CServerPost( IEventRouter* pER, CPile* pPile, CHTTPRequest* pRequest, Socket* pSocket, DWORD sessionID ) :
	CTransfer( pER, pPile, pSocket, sessionID ), m_pRequest( pRequest ), m_nTotalBytesToRecv( 0 )
{
}
CServerPost::~CServerPost()
{
}
DWORD CServerPost::PreCommand()
{
	USES_CONVERSION;

	BSTR strFilePath;
	m_pER->OnPostRequest( m_SessionID, _bstr_t( m_pRequest->m_URI ), &strFilePath );

	DWORD err = m_pFile->Create( strFilePath, GENERIC_WRITE, FILE_SHARE_READ, OPEN_ALWAYS );

	if( err )
	{
		BYTE* ErrorBuf = (BYTE *)"HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\n\r\n";
		m_pSocket->Send( ErrorBuf, strlen( (char*)ErrorBuf ) );
		m_pSocket->Disconnect();
		m_pER->OnFileErr( m_SessionID, err );
		return -1;
	}

	_variant_t Content_Length;
	m_pRequest->m_pHeaders->get_Item( &_variant_t( _bstr_t( "Content-Length" ) ), &Content_Length );
	m_nBytesToRecv = _wtol( Content_Length.bstrVal );
	m_fp = 0;

	m_pFile->Seek( m_nBytesToRecv, 0, FILE_BEGIN );
	m_pFile->SetEOF();

	return 0;
}
DWORD CServerPost::Command()
{
	DWORD nBytesReceived = 0, nBytesWritten = 0, err = 0;

	nBytesReceived = m_pSocket->Recv( m_buf, m_nBytesToRecv >= BLOCK_SIZE ? BLOCK_SIZE : m_nBytesToRecv );

	if( nBytesReceived == SOCKET_ERROR )
	{
		if( m_nTotalBytesToRecv == -1 )
		{
			m_bAborted = FALSE;
			return 1;
		}

		switch( err = WSAGetLastError() )
		{
		case WSAEWOULDBLOCK:
			return 0;
		case WSAETIMEDOUT:
			m_pER->OnTimeout( m_SessionID );
		default:
			m_pER->OnSockErr( m_SessionID, err );
			return err;
		}
	}

	m_pFile->Seek( m_fp, 0, FILE_BEGIN );
	if( FAILED(m_pFile->Write( m_buf, nBytesReceived )) )
	{
		m_pER->OnFileErr( m_SessionID, GetLastError() );
		return 2;
	}

	m_nBytesToRecv -= nBytesReceived;
	m_fp += nBytesReceived;

	if( m_nBytesToRecv == 0 )
	{
		m_bAborted = FALSE;
		return -1;
	}
	return 0;
}
DWORD CServerPost::PostCommand()
{
	if( m_bAborted )
        m_pER->OnTransferAborted( m_SessionID );
	else
	{
		BYTE* ErrorBuf = (BYTE *) "HTTP/1.1 204 No Content\r\nConnection: close\r\n\r\n";
		m_pSocket->Send( ErrorBuf, strlen( (char *) ErrorBuf ) );
		m_pER->OnTransferComplete( m_SessionID );
	}

	delete this;
	return 0;
}
DWORD CServerPost::GetProgress()
{
	return m_fp;
}
DWORD CServerPost::GetLength()
{
	return m_nTotalBytesToRecv;
}
