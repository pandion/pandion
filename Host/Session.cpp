#include "stdafx.h"
#include "Session.h"
#include "Pile.h"

CSession::CSession( CPile* p, IStream *pERStream ) :
	m_pPile( p ), m_pER( NULL )
{
	m_SessionID = GetNewCookie();
	HRESULT hr = CoGetInterfaceAndReleaseStream( pERStream, IID_IEventRouter, (void **) &m_pER );
}
CSession::~CSession()
{
}

DWORD CSession::GetNewCookie()
{
	static int lastCookie = 0;
	return ++lastCookie;
}
DWORD CSession::GetSessionID()
{
	return m_SessionID;
}

void CSession::PreCommand()
{
	m_pPile->AddSession( this );
}
void CSession::Command()
{
}
void CSession::PostCommand()
{
	m_pER->Release();
	m_pPile->DelSession( GetSessionID() );
}
