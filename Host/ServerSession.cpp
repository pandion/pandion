#include "stdafx.h"
#include "ServerSession.h"
#include "Pile.h"
#include "Socket.h"
#include "HTTPRequest.h"
#include "ServerGet.h"
#include "ServerPost.h"
#include "ServerMPFDPost.h"

CServerSession::CServerSession( CPile *pPile, Socket *sock, IStream *pERStream ) : 
	CSession( pPile, pERStream ), m_pSocket( sock ), m_pTransfer( 0 )
{
	m_pRequest = new CHTTPRequest;
}
CServerSession::~CServerSession()
{
	delete m_pRequest;
	delete m_pSocket;
}
DWORD CServerSession::RecvRequest()
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

	if( i == 8191 )	return -2;

	m_pRequest->Parse( buf );
	return 0;
}
void CServerSession::PreCommand()
{
	CSession::PreCommand();

	BSTR remAddress = SysAllocString( m_pSocket->GetRemoteAddress() );
	m_pER->OnAccept( m_SessionID, remAddress, m_pSocket->GetRemotePort(), m_pSocket->GetLocalPort() );
	SysFreeString( remAddress );

	if( RecvRequest() )
	{
		m_pER->OnTransferAborted( m_SessionID );
		return TerminateThreadInternal();
	}

	switch( m_pRequest->m_method )
	{
	case HTTP_METHOD_GET:
		m_pTransfer = new CServerGet( m_pER, m_pPile, m_pRequest, m_pSocket, m_SessionID );
		break;
	case HTTP_METHOD_MPFDPOST:
		m_pTransfer = new CServerMPFDPost( m_pER, m_pPile, m_pRequest, m_pSocket, m_SessionID );
		break;
	case HTTP_METHOD_POST:
        m_pTransfer = new CServerPost( m_pER, m_pPile, m_pRequest, m_pSocket, m_SessionID );
		break;
	default:
		m_pER->OnHTTPErr( m_SessionID, 501 );
		m_pER->OnTransferAborted( m_SessionID );
		return TerminateThreadInternal();
	}

	if( m_pTransfer->PreCommand() )
		TerminateThreadInternal();
}
void CServerSession::Command()
{
	if( m_pTransfer->Command() )
		TerminateThreadInternal();
}
void CServerSession::PostCommand()
{
	if( m_pTransfer )
	{
		m_pTransfer->PostCommand();
		//delete m_pTransfer; //m_pTransfer deletes itself
		m_pTransfer = 0;
	}
	else
	{
		m_pER->OnTransferAborted( m_SessionID );
	}

	CSession::PostCommand();
}
DWORD CServerSession::GetProgress()
{
	if( m_pTransfer )
		return m_pTransfer->GetProgress();
	else
        return 0;
}
DWORD CServerSession::GetLength()
{
	if( m_pTransfer )
		return m_pTransfer->GetLength();
	else
		return 0;
}
void CServerSession::Terminate()
{
	TerminateThreadInternal();
}