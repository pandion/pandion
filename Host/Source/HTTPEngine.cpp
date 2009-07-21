#include "stdafx.h"
#include "HTTPEngine.h"
#include "EventRouter.h"
#include "Pile.h"
#include "ProxyInfo.h"
#include "ListenThread.h"
#include "BasicThread.h"
#include "ClientSession.h"
#include "File.h"

CHTTP::CHTTP()
{
	m_pPile      = new CPile;
	m_pProxyInfo = new CProxyInfo;

	m_pER        = new CComObject< CEventRouter >;
	m_pER->AddRef();

	IStream *pERStream = NULL;
	CoMarshalInterThreadInterfaceInStream( IID_IEventRouter, m_pER, &pERStream );

	m_pLT        = new CListenThread( m_pPile, pERStream );
	m_pLT->StartThread();
}
CHTTP::~CHTTP()
{
	delete m_pPile;
	delete m_pProxyInfo;

	m_pER->Release();
	m_pLT->TerminateThreadInternal();
}
STDMETHODIMP CHTTP::ShareFile( BSTR filename, BSTR URL, DWORD* retCookie )
{
	CSharedFile* pSF = new CSharedFile( filename, URL );

	m_pPile->AddSharedFile( pSF );
	*retCookie = pSF->GetCookie();

	return S_OK;
}
STDMETHODIMP CHTTP::UnShareFile( DWORD sfCookie )
{
	return m_pPile->DelSharedFile( sfCookie );
}

STDMETHODIMP CHTTP::Listen( unsigned short port )
{
	m_pER->OnListening( m_pLT->AddPort( port ) );
	return S_OK;
}
STDMETHODIMP CHTTP::StopListening( unsigned short port )
{
	m_pLT->RemPort( port );
	return S_OK;
}

STDMETHODIMP CHTTP::Get( BSTR filename, BSTR URI, DWORD offset, DWORD len, BSTR address, USHORT port, DWORD* sessionID )
{
	USES_CONVERSION;
	*sessionID = 0;
	if( wcslen( URI ) > 1024 || wcslen( address ) > 128 )
		return S_FALSE;


	IPdnFile* pFile = new CComObject<CFile>;
	pFile->AddRef();
	DWORD err = pFile->Create( filename, GENERIC_WRITE, FILE_SHARE_READ, OPEN_ALWAYS );
	if( err == (DWORD)INVALID_HANDLE_VALUE )
	{
		pFile->Release();
		return S_FALSE;
	}


	CCSInfo* pCSInfo = new CCSInfo();
	StringCchCopyW( pCSInfo->address, 128, address );
	pCSInfo->len = len;
	pCSInfo->method = HTTP_METHOD_GET;
	pCSInfo->offset = offset;
	pCSInfo->pFile = pFile;
	pCSInfo->port = port;
	pCSInfo->pProxyInfo = m_pProxyInfo;
	StringCchCopyW( pCSInfo->URI, 1024, URI );

	IStream *pERStream = NULL;
	CoMarshalInterThreadInterfaceInStream( IID_IEventRouter, m_pER, &pERStream );
	CClientSession* cs = new CClientSession( m_pPile, pERStream, pCSInfo );
	cs->StartThread();

	*sessionID = cs->GetSessionID();
	return S_OK;
}
STDMETHODIMP CHTTP::Post( BSTR filename, BSTR URI, DWORD offset, DWORD len, BSTR address, USHORT port, DWORD* sessionID )
{
	*sessionID = 0;
	if( wcslen( URI ) > 1024 || wcslen( address ) > 128 )
		return S_FALSE;


	IPdnFile* pFile = new CComObject<CFile>;
	pFile->AddRef();
	DWORD err = pFile->Create( filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_ALWAYS );
	if( err == (DWORD)INVALID_HANDLE_VALUE )
	{
		pFile->Release();
		return S_FALSE;
	}


	CCSInfo* pCSInfo = new CCSInfo();
	StringCchCopyW( pCSInfo->address, 128, address );
	pCSInfo->len = len;
	pCSInfo->method = HTTP_METHOD_POST;
	pCSInfo->offset = offset;
	pCSInfo->pFile = pFile;
	pCSInfo->port = port;
	pCSInfo->pProxyInfo = m_pProxyInfo;
	StringCchCopyW( pCSInfo->URI, 1024, URI );

	IStream *pERStream = NULL;
	CoMarshalInterThreadInterfaceInStream( IID_IEventRouter, m_pER, &pERStream );
	CClientSession* cs = new CClientSession( m_pPile, pERStream, pCSInfo );
	cs->StartThread();

	*sessionID = cs->GetSessionID();
	return S_OK;
}
STDMETHODIMP CHTTP::SetProxyInfo( BSTR proxyAddress, USHORT proxyPort, BSTR proxyUName, BSTR proxyPass )
{
	USES_CONVERSION;
	m_pProxyInfo->SetAddress ( proxyAddress );
	m_pProxyInfo->SetPort    ( proxyPort );
	m_pProxyInfo->SetUsername( proxyUName );
	m_pProxyInfo->SetPassword( proxyPass );
	return S_OK;
}
STDMETHODIMP CHTTP::Subscribe( IDispatch* wnd )
{
	m_pER->Subscribe( wnd );
	return S_OK;
}
STDMETHODIMP CHTTP::Unsubscribe( IDispatch* wnd )
{
	m_pER->Unsubscribe( wnd );
	return S_OK;
}

STDMETHODIMP CHTTP::GetProgress( DWORD sessionID, DWORD* retval )
{
	*retval = 0;
	CSession* pSession = m_pPile->GetSession( sessionID );
	if( pSession )
		*retval = pSession->GetProgress();
	return S_OK;
}
STDMETHODIMP CHTTP::GetLength( DWORD sessionID, DWORD* retval )
{
	*retval = 0;
	CSession* pSession = m_pPile->GetSession( sessionID );
	if( pSession )
		*retval = pSession->GetLength();
	return S_OK;
}
STDMETHODIMP CHTTP::Abort( DWORD sessionID )
{
	CSession* pSession = m_pPile->GetSession( sessionID );
	if( pSession )
		pSession->Terminate();
	return S_OK;
}