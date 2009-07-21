#include "stdafx.h"
#include "Socket.h"
#include "utf.h"
#include "file.h"
#include "HTTP.h"

CHTTP::CHTTP()
{
	(new CComObject<CHTTPServer>)->QueryInterface( IID_IHTTPServer, (void **) &m_pServer );
	(new CComObject<CHTTPClient>)->QueryInterface( IID_IHTTPClient, (void **) &m_pClient );
}
CHTTP::~CHTTP()
{
	m_pServer->Release();
	m_pClient->Release();
}
STDMETHODIMP CHTTP::get_Server( IDispatch **ppServer )
{
	m_pServer->QueryInterface( IID_IDispatch, (void **) ppServer );
	return S_OK;
}
STDMETHODIMP CHTTP::get_Client( IDispatch **ppClient )
{
	m_pClient->QueryInterface( IID_IDispatch, (void **) ppClient );
	return S_OK;
}
CEventEntry::CEventEntry( BSTR event, BSTR handler, IDispatch *pWnd )
{
	m_Event = SysAllocString( event );
	m_Handler = SysAllocString( handler );
	pWnd->QueryInterface( IID_IPdnWnd, (void **) &m_pWnd );
}
CEventEntry::~CEventEntry()
{
	SysFreeString( m_Event );
	SysFreeString( m_Handler );
	m_pWnd->Release();
}
CHTTPServer::CHTTPServer()
{
	m_bContinueListening = true;

	DWORD ThreadID;
	::CreateThread( NULL, 0, AsyncListenProc, (void *)this, 0, &ThreadID );
}
CHTTPServer::~CHTTPServer()
{
	m_cs.Lock();
	m_bContinueListening = false;
	m_cs.Unlock();
}
DWORD __stdcall CHTTPServer::AsyncListenProc( void *pThis )
{
	return ((CHTTPServer *) pThis)->AsyncListen();
}
DWORD CHTTPServer::AsyncListen()
{
	while( m_bContinueListening )
	{
		m_cs.Lock();
		if( !m_ListenSockets.GetCount() )
		{
			m_cs.Unlock();
			Sleep( 50 );
			continue;
		}

		fd_set set;
		FD_ZERO( &set );

		for( DWORD i = 0; i < m_ListenSockets.GetCount(); i++ )
			FD_SET( m_ListenSockets[i]->GetHandle(), &set );

		timeval tv = { 0, 500 };
		select( 0, &set, 0, 0, &tv );

		for( DWORD i = 0; i < m_ListenSockets.GetCount(); i++ )
			if( FD_ISSET( m_ListenSockets[i]->GetHandle(), &set ) )
			{
				ULONG u = 0;
				CSocket *pSockRemote = new CSocket( accept( m_ListenSockets[i]->GetHandle(), 0, 0 ) );
				ioctlsocket( pSockRemote->GetHandle(), FIONBIO, &u );

				CServerSession *pNewSession = new CComObject<CServerSession>;
				pNewSession->SetSocket( pSockRemote );
				pNewSession->SetSharedFiles( &m_SharedFiles );

				VARIANT v;
				VariantInit( &v );
				v.vt = VT_DISPATCH;
                pNewSession->QueryInterface( IID_IDispatch, (void **) &v.pdispVal );
				DispatchEvent( L"HTTP.Server.Accept", &v, 1 );
				VariantClear( &v );
			}
		m_cs.Unlock();
	}
	StopListen( 0 );
	return 0;
}
VOID CHTTPServer::DispatchEvent( BSTR event, VARIANT *parameters, DWORD nParameters )
{
	for( DWORD i = 0; i < m_Events.GetCount(); i++ )
        if( !wcscmp( event, m_Events[i]->m_Event ) )
			m_Events[i]->m_pWnd->Do( m_Events[i]->m_Handler, parameters, nParameters );
}
STDMETHODIMP CHTTPServer::RegisterEvent( BSTR event, BSTR handler, IDispatch *pWnd )
{
	m_Events.Add( new CEventEntry( event, handler, pWnd ) );
	return S_OK;
}
STDMETHODIMP CHTTPServer::Listen( USHORT port, BOOL useTLS, USHORT *retPort )
{
	CSocket* ls = new CSocket;

	if( !( *retPort = ls->Listen( port ) ) )
		if( !( *retPort = ls->Listen( 0 ) ) )
			return S_FALSE;

	ULONG u = 1;
	ioctlsocket( ls->GetHandle(), FIONBIO, &u );

	m_cs.Lock();
	m_ListenSockets.Add( ls );
	m_cs.Unlock();

	return S_OK;
}
STDMETHODIMP CHTTPServer::StopListen( USHORT port )
{
	for( DWORD i = 0; i < m_ListenSockets.GetCount(); i++ )
	{
		if( m_ListenSockets[i]->GetLocalPort() == port || !port )
		{
			m_cs.Lock();
			delete m_ListenSockets[i];
			m_ListenSockets.RemoveAt( i );
			m_cs.Unlock();
			break;
		}
	}
	return S_OK;
}
STDMETHODIMP CHTTPServer::ShareFile( BSTR path, BSTR URL )
{
	m_SharedFiles[CComBSTR(URL)] = CComBSTR(path);
	return S_OK;
}
STDMETHODIMP CHTTPServer::UnShareFile( BSTR URL )
{
	m_SharedFiles.RemoveKey( URL );
	return S_OK;
}
CServerSession::CServerSession()
{
	m_HTTPHeaders.InitHashTable( 256 );
	m_pFile = new CComObject<CFile>;
	m_bContinueTransfer = true;

	DWORD ThreadID;
	m_hThread = ::CreateThread( NULL, 0, AsyncTransferProc, (void *)this, CREATE_SUSPENDED, &ThreadID );
}
CServerSession::~CServerSession()
{
	m_cs.Lock();
	m_bContinueTransfer = false;
	m_cs.Unlock();
	if( m_pFile )
	{
        delete m_pFile;
	}
	if( m_pSocket )
	{
		delete m_pSocket;
	}
}
VOID CServerSession::SetSocket( CSocket *pSocket )
{
	m_pSocket = pSocket;
}
VOID CServerSession::SetSharedFiles( CAtlMap<CComBSTR,CComBSTR> *pSharedFiles )
{
	m_pSharedFiles = pSharedFiles;
}
DWORD CServerSession::AsyncTransferProc( void *pThis )
{
	return ((CServerSession *)pThis)->AsyncTransfer();
}
DWORD CServerSession::AsyncTransfer()
{
	DWORD err = RecvRequest();
	if( !err )
	{
		err = SendResponse();
		if( !err )
		{
			if( m_Method == HTTP_METHOD_GET )
			{
				UploadFile();
			}
			else if( m_Method == HTTP_METHOD_POST )
			{
				DownloadFile();
			}
		}
	}
	else if( err = -1 )
	{
		/* Socket error */
	}
	else if( err = -2 )
	{
		/* Buffer too small */
	}
	return 0;
}
DWORD CServerSession::RecvRequest()
{
	/* Receive the request from the client */
	BYTE buf[8192], b;
	DWORD i;

	for( i = 0; i < 8191; i++ )
	{
		if( m_pSocket->Recv( &b, 1 ) == SOCKET_ERROR ) return -1;
		buf[i] = b;
		buf[i+1] = 0;
		if( strstr( (char*) buf, "\r\n\r\n" ) )	break;
	}

	if( i == 8191 )	return -2;

	/* Parse the request */
	BYTE line[8192];
	char method[8192];
	char URI[8192];

	/* Parse the request line */
	for( i = 1; buf[i-1] != '\r' && buf[i] != '\n'; line[i-2] = buf[++i - 2] );
	line[i-1] = 0;
	BYTE *p = buf + i + 1;
	sscanf( (char*)line, "%s%s", method, URI );

	m_RequestURI = CUTF82W( URI );

	/* Parse the HTTP headers */
	while( p[0] != '\r' && p[1] != '\n' )
	{
        for( i = 1; p[i-1] != '\r' && p[i] != '\n'; line[i-2] = p[++i - 2] );
		line[i-1] = 0;
		p = p + i + 1;
		if( strstr( (char*) line, ": " ) )
		{
			strstr( (char*) line, ": " )[0] = 0;
			m_HTTPHeaders[CComBSTR( CUTF82W( (char*) line ) )] = 
				CComBSTR( CUTF82W( (char*) line + strlen( (char*) line ) + 2 ) );
		}
	}

	if( strstr( method, "GET" ) )
		m_Method = HTTP_METHOD_GET;
	else if( strstr(  method, "PUT" ) )
		m_Method = HTTP_METHOD_PUT;
	else if( strstr( method, "POST" ) && !wcscmp( m_HTTPHeaders[CComBSTR(L"Content-Type")],  L"multipart/form-data" ) )
		m_Method = HTTP_METHOD_MPFDPOST;
	else if( strstr( method, "POST" ) )
		m_Method = HTTP_METHOD_POST;
	else
		m_Method = HTTP_METHOD_UNKNOWN;

	return 0;
}
DWORD CServerSession::SendResponse()
{
	if( m_Method = HTTP_METHOD_GET )
	{
		unsigned uFileSize, err;

		if( (*m_pSharedFiles)[m_RequestURI] )
		{
			err = m_pFile->Create( (*m_pSharedFiles)[m_RequestURI], GENERIC_READ, 
				FILE_SHARE_READ | FILE_SHARE_WRITE,	OPEN_EXISTING );
		}
		else
		{
			err = -1;
		}

		if( err )
		{
			/* HTTP ERROR 404 */
			m_pSocket->SendLine( L"HTTP/1.1 404 Not Found" );
			m_pSocket->SendLine( L"Connection: close" );
			m_pSocket->SendLine( L"" );
			m_pSocket->Disconnect();
			return 404;
		}
		else
		{
			m_pFile->get_Size( &uFileSize );

			if( m_HTTPHeaders[CComBSTR(L"Content-Range")].Length() > 0 )
			{
				swscanf( m_HTTPHeaders[CComBSTR(L"Content-Range")], L"bytes %u-%u", &m_fp, &m_BytesToSend );
				m_BytesToSend -= m_fp;
				if( m_BytesToSend > ( uFileSize - m_fp ) )
				{
					m_BytesToSend = ( uFileSize - m_fp );
				}
				if( m_fp >= uFileSize )
				{
					/* HTTP ERROR 404 */
					m_pSocket->SendLine( L"HTTP/1.1 404 Not Found" );
					m_pSocket->SendLine( L"Connection: close" );
					m_pSocket->SendLine( L"" );
					m_pSocket->Disconnect();
					m_pFile->Close();
					return 404;
				}
			}
			else
			{
				m_BytesToSend = uFileSize;
			}

			m_TotalBytesToSend = m_BytesToSend;

			m_pFile->Seek( m_fp, 0, FILE_BEGIN );

			BSTR strFileName;
			m_pFile->get_FileName( &strFileName );
			/* m_pER->OnGetRequest( m_SessionID, m_pPile->GetCookieByURL( m_pRequest->m_URI ), _bstr_t( m_pRequest->m_URI ), strFileName ); */
			SysFreeString( strFileName );

			TCHAR Content_Type[128] = { 0 };
			DWORD nChars = 128;
			CRegKey Classes_Root( HKEY_CLASSES_ROOT );
			Classes_Root.Open( HKEY_CLASSES_ROOT, ::PathFindExtension( CW2T(m_RequestURI) ) );
			Classes_Root.QueryStringValue( TEXT("Content Type"), Content_Type, &nChars );
			if( !Content_Type[0] ) _tcscpy( Content_Type, TEXT("application/octet-stream") );

			WCHAR Buffer[8192];

			if( !m_fp )
			{
				m_pSocket->SendLine( L"HTTP/1.1 200 OK" );
			}
			else
			{
				m_pSocket->SendLine( L"HTTP/1.1 206 Partial Content" );
				wsprintfW( Buffer, L"Content-Range: bytes %u-%u/%u", m_fp, m_fp + m_BytesToSend, uFileSize );
				m_pSocket->SendLine( Buffer );
			}

			wsprintfW( Buffer, L"Content-Length: %u", m_BytesToSend );
			m_pSocket->SendLine( Buffer );
			wsprintfW( Buffer, L"Content-Type: %s", CT2W(Content_Type) );
			m_pSocket->SendLine( Buffer );
			m_pSocket->SendLine( L"Connection: close" );
			m_pSocket->SendLine( L"" );

			return 0;
		}
	}
	else if( m_Method = HTTP_METHOD_POST )
	{
		return -1;
	}
	return 0;
}
DWORD CServerSession::UploadFile()
{
	return 0;
}
DWORD CServerSession::DownloadFile()
{
	return 0;
}
VOID CServerSession::DispatchEvent( BSTR event, VARIANT *parameters, DWORD nParameters )
{
	for( DWORD i = 0; i < m_Events.GetCount(); i++ )
        if( !wcscmp( event, m_Events[i]->m_Event ) )
			m_Events[i]->m_pWnd->Do( m_Events[i]->m_Handler, parameters, nParameters );
}
STDMETHODIMP CServerSession::RegisterEvent( BSTR event, BSTR handler, IDispatch *pWnd )
{
	m_Events.Add( new CEventEntry( event, handler, pWnd ) );
	return S_OK;
}
STDMETHODIMP CServerSession::get_Progress( DWORD *Progress )
{
	return S_OK;
}
STDMETHODIMP CServerSession::get_Length( DWORD *Length )
{
	return S_OK;
}
STDMETHODIMP CServerSession::get_Status( BSTR *Status )
{
	return S_OK;
}
STDMETHODIMP CServerSession::Pause()
{
	return S_OK;
}
STDMETHODIMP CServerSession::Resume()
{
	::ResumeThread( m_hThread );
	return S_OK;
}
STDMETHODIMP CServerSession::Abort()
{
	return S_OK;
}
STDMETHODIMP CServerSession::get_Method( BSTR *Request )
{
	return S_OK;
}
STDMETHODIMP CServerSession::get_RequestURI( BSTR *Request )
{
	return S_OK;
}
STDMETHODIMP CServerSession::get_HTTPHeaders( BSTR *Headers )
{
	return S_OK;
}
CHTTPClient::CHTTPClient()
{
}
CHTTPClient::~CHTTPClient()
{
}
STDMETHODIMP CHTTPClient::SetProxySettings( BSTR Server, USHORT Port, BSTR Username, BSTR Password )
{
	return S_OK;
}
STDMETHODIMP CHTTPClient::Get( BSTR URL, BSTR File, DWORD Offset, DWORD Length, IDispatch **ppSession )
{
	return S_OK;
}
STDMETHODIMP CHTTPClient::Post( BSTR URL, BSTR File, DWORD Offset, DWORD Length, IDispatch **ppSession )
{
	return S_OK;
}
