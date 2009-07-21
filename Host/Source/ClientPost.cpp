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
 * Filename:    ClientPost.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "ClientPost.h"
#include "HTTPResponse.h"
#include "ClientSession.h"
#include "ProxyInfo.h"
#include "Socket.h"
#include "File.h"

CClientPost::CClientPost( IEventRouter* pER, CPile* pPile, CCSInfo* pCSInfo, Socket* pSocket, DWORD sessionID ) :
	CTransfer( pER, pPile, pSocket, sessionID ), m_pCSInfo( pCSInfo )
{
	m_nBytesToSend = 0;
	m_nTotalBytesToSend = 0;
	m_pResponse = new CHTTPResponse;
}
CClientPost::~CClientPost()
{
	delete m_pResponse;
}
DWORD CClientPost::SendRequest()
{
	unsigned nFileSize;
	m_pCSInfo->pFile->get_Size( &nFileSize );

	TCHAR Content_Type[128] = { 0 };
	DWORD nChars = 128;
	CRegKey Classes_Root( HKEY_CLASSES_ROOT );
	Classes_Root.Open( HKEY_CLASSES_ROOT, ::PathFindExtension( CW2T(m_pCSInfo->URI) ) );
	Classes_Root.QueryStringValue( TEXT("Content Type"), Content_Type, &nChars );
	if( !Content_Type[0] ) StringCchCopy( Content_Type, 128, TEXT("application/octet-stream") );

	WCHAR Buffer[8192];
	BSTR proxyAddress, proxyUsername, proxyPassword;
	m_pCSInfo->pProxyInfo->GetUsername( &proxyUsername );
	m_pCSInfo->pProxyInfo->GetPassword( &proxyPassword );
	m_pCSInfo->pProxyInfo->GetAddress( &proxyAddress );

	if( wcslen( proxyAddress ) )
	{
		StringCbPrintfW( Buffer, 8192, L"POST http://%s:%u%s HTTP/1.1", m_pCSInfo->address, m_pCSInfo->port, m_pCSInfo->URI );
		m_pSocket->SendLine( Buffer );
		StringCbPrintfW( Buffer, 8192, L"Host: %s:%u", m_pCSInfo->address, m_pCSInfo->port );
		m_pSocket->SendLine( Buffer );

		if( wcslen( proxyUsername ) )
		{
			CHAR aBuffer[8192];

			int len = 1029;
			BYTE strdec[258], strenc[1029];
			StringCbPrintfA( (LPSTR) strdec, 258, "%s:%s", CW2A(proxyUsername), CW2A(proxyPassword) );
			Base64Encode( strdec, strlen( (LPSTR) strdec ), (LPSTR) strenc, &len, ATL_BASE64_FLAG_NOCRLF );
			strenc[len] = 0;

			StringCbPrintfA( aBuffer, 1029, "Proxy-Authorization: Basic %s", strenc );
			m_pSocket->SendLine( CA2W(aBuffer) );
		}
	}
	else
	{
		StringCbPrintfW( Buffer, 8192, L"POST %s HTTP/1.1", m_pCSInfo->URI );
		m_pSocket->SendLine( Buffer );
	}

	StringCbPrintfW( Buffer, 8192, L"Host: %s:%u", m_pCSInfo->address, m_pCSInfo->port );
	m_pSocket->SendLine( Buffer );
	StringCbPrintfW( Buffer, 8192, L"Content-Type: %s", CT2W(Content_Type) );
	m_pSocket->SendLine( Buffer );

	if( m_pCSInfo->offset && m_pCSInfo->len > 0 && m_pCSInfo->offset + m_pCSInfo->len <= nFileSize )
	{
		StringCbPrintfW( Buffer, 8192, L"Content-Range: bytes %u-%u/*", m_pCSInfo->offset, unsigned( m_pCSInfo->offset + m_pCSInfo->len ) ); 
		m_pSocket->SendLine( Buffer );
	}
	else
	{
		StringCbPrintfW( Buffer, 8192, L"Content-Length: %u", nFileSize );
		m_pSocket->SendLine( Buffer );
	}

	m_pSocket->SendLine( L"Connection: close" );
	m_pSocket->SendLine( L"" );

	SysFreeString( proxyPassword );
	SysFreeString( proxyUsername );
	SysFreeString( proxyAddress );

	return 0;
}
DWORD CClientPost::RecvResponse()
{
	BYTE buf[8192], b;
	DWORD i;

	for( i = 0; i < 8191; i++ )
	{
		if( m_pSocket->Recv( &b, 1 ) == SOCKET_ERROR )
		{
			m_pER->OnSockErr( m_SessionID, WSAGetLastError() );
			return -1;
		}
		buf[i] = b;
		buf[i+1] = 0;
		if( strstr( (char*) buf, "\r\n\r\n" ) )	break;
	}
	if( i == 8191 )	return -1;

	m_pResponse->Parse( buf );
	if( m_pResponse->m_dwCode != 200 )
	{
		m_pER->OnHTTPErr( m_SessionID, m_pResponse->m_dwCode );
		return -1;
	}

	return 0;
}
DWORD CClientPost::PreCommand()
{
	unsigned nFileSize;
	m_pCSInfo->pFile->get_Size( &nFileSize );
	if( SendRequest() )
		return -1;
//	if( RecvResponse() ) /// 100 Continue ???
//		return -1;

	if( m_pCSInfo->offset && m_pCSInfo->len > 0 && m_pCSInfo->offset + m_pCSInfo->len <= nFileSize )
	{
		m_fp = m_pCSInfo->offset;
		m_nBytesToSend = m_pCSInfo->len;
	}
	else
	{
		m_fp = 0;
		m_nBytesToSend = nFileSize;
	}

	m_nTotalBytesToSend = m_nBytesToSend;

	m_fp = m_pCSInfo->pFile->Seek( m_fp, 0, FILE_BEGIN );

	return 0;
}
DWORD CClientPost::Command()
{
	DWORD nBytesSent = 0, dwErr = 0;
	HRESULT hr;

	m_pCSInfo->pFile->Seek( m_fp, 0, FILE_BEGIN );
	hr = m_pCSInfo->pFile->Read( m_buf, m_nBytesToSend >= BLOCK_SIZE ? BLOCK_SIZE : m_nBytesToSend );

	if( FAILED(hr) )
	{
		m_pER->OnFileErr( m_SessionID, GetLastError() );
		return -1;
	}

	nBytesSent = m_pSocket->Send( m_buf, m_nBytesToSend >= BLOCK_SIZE ? BLOCK_SIZE : m_nBytesToSend );

	if( nBytesSent == SOCKET_ERROR )
	{
		switch( dwErr = WSAGetLastError() )
		{
		case WSAEWOULDBLOCK:
			break;
		case WSAETIMEDOUT:
			m_pER->OnTimeout( m_SessionID );
		default:
			m_pER->OnSockErr( m_SessionID, dwErr );
			return dwErr;
		}
	}

	m_nBytesToSend -= nBytesSent;
	m_fp += nBytesSent;

	if( m_nBytesToSend == 0 )
	{
		m_bAborted = FALSE;
		return 1;
	}
	return 0;
}
DWORD CClientPost::PostCommand()
{
	if( m_bAborted )
        m_pER->OnTransferAborted( m_SessionID );
	else
	{
		RecvResponse();
		m_pER->OnTransferComplete( m_SessionID );
	}
	delete this;
	return 0;
}
DWORD CClientPost::GetProgress()
{
	return m_fp;
}
DWORD CClientPost::GetLength()
{
	return m_nTotalBytesToSend;
}
