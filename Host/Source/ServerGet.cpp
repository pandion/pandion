#include "stdafx.h"
#include "Transfer.h"
#include "ServerGet.h"
#include "HTTPRequest.h"
#include "Pile.h"
#include "Socket.h"
#include "File.h"

CServerGet::CServerGet( IEventRouter* pER, CPile* pPile, CHTTPRequest* pRequest, Socket* pSocket, DWORD sessionID ) :
	CTransfer( pER, pPile, pSocket, sessionID ), m_pRequest( pRequest ), m_nTotalBytesToSend( 0 )
{
}
CServerGet::~CServerGet()
{
}
DWORD CServerGet::PreCommand()
{
	unsigned uFileSize;

	DWORD err = m_pFile->Create( m_pPile->GetFileNameByURL( m_pRequest->m_URI ), GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE,	OPEN_EXISTING );

	if( err )
	{
		m_pER->OnHTTPErr( m_SessionID, 404 );
		m_pSocket->SendLine( L"HTTP/1.1 404 Not Found" );
		m_pSocket->SendLine( L"Connection: close" );
		m_pSocket->SendLine( L"" );
		return 404;
	}
	else
	{
		m_pFile->get_Size( &uFileSize );

		_variant_t Content_Range;
		m_pRequest->m_pHeaders->get_Item( &_variant_t( _bstr_t( "Content-Range" ) ), &Content_Range );
		if( Content_Range.vt == VT_BSTR )
		{
			swscanf( Content_Range.bstrVal, L"bytes %u-%u", &m_fp, &m_nBytesToSend );
			m_nBytesToSend -= m_fp;
			if( m_nBytesToSend > ( uFileSize - m_fp ) )
			{
				m_nBytesToSend = ( uFileSize - m_fp );
			}
			if( m_fp >= uFileSize )
			{
				m_pER->OnHTTPErr( m_SessionID, 404 );
				m_pSocket->SendLine( L"HTTP/1.1 404 Not Found" );
				m_pSocket->SendLine( L"Connection: close" );
				m_pSocket->SendLine( L"" );
				return 404;
			}
		}
		else
			m_nBytesToSend = uFileSize;
		m_nTotalBytesToSend = m_nBytesToSend;

		m_pFile->Seek( m_fp, 0, FILE_BEGIN );

		BSTR strFileName;
		m_pFile->get_FileName( &strFileName );
		m_pER->OnGetRequest( m_SessionID, m_pPile->GetCookieByURL( m_pRequest->m_URI ), _bstr_t( m_pRequest->m_URI ), strFileName );
		SysFreeString( strFileName );

		TCHAR Content_Type[128] = { 0 };
		DWORD nChars = 128;
		CRegKey Classes_Root( HKEY_CLASSES_ROOT );
		Classes_Root.Open( HKEY_CLASSES_ROOT, ::PathFindExtension( CW2T(m_pRequest->m_URI) ) );
		Classes_Root.QueryStringValue( TEXT("Content Type"), Content_Type, &nChars );
		if( !Content_Type[0] ) StringCchCopy( Content_Type, 128, TEXT("application/octet-stream") );

		WCHAR Buffer[8192];

		if( !m_fp )
		{
			m_pSocket->SendLine( L"HTTP/1.1 200 OK" );
		}
		else
		{
			m_pSocket->SendLine( L"HTTP/1.1 206 Partial Content" );
			StringCbPrintfW( Buffer, 8192, L"Content-Range: bytes %u-%u/%u", m_fp, m_fp + m_nBytesToSend, uFileSize );
			m_pSocket->SendLine( Buffer );
		}

		StringCbPrintfW( Buffer, 8192, L"Content-Length: %u", m_nBytesToSend );
		m_pSocket->SendLine( Buffer );
		StringCbPrintfW( Buffer, 8192, L"Content-Type: %s", CT2W(Content_Type) );
		m_pSocket->SendLine( Buffer );
		m_pSocket->SendLine( L"Connection: close" );
		m_pSocket->SendLine( L"" );

		return 0;
	}
}
DWORD CServerGet::Command()
{
	DWORD nBytesSent = 0, dwErr = 0;
	HRESULT hr;

	m_pFile->Seek( m_fp, 0, FILE_BEGIN );
	hr = m_pFile->Read( m_buf, m_nBytesToSend >= BLOCK_SIZE ? BLOCK_SIZE : m_nBytesToSend );

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
DWORD CServerGet::PostCommand()
{
	if( m_bAborted )
        m_pER->OnTransferAborted( m_SessionID );
	else
		m_pER->OnTransferComplete( m_SessionID );
	delete this;
	return 0;
}
DWORD CServerGet::GetProgress()
{
	return m_fp;
}
DWORD CServerGet::GetLength()
{
	return m_nTotalBytesToSend;
}