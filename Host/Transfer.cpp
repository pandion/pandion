#include "stdafx.h"
#include "Transfer.h"
#include "File.h"

CTransfer::CTransfer( IEventRouter* pER, CPile* pPile, Socket* pSocket, DWORD sessionID ) :
	m_pER( pER ), m_pPile( pPile ), m_pSocket( pSocket ), m_SessionID( sessionID ), m_bAborted( TRUE ), m_fp( 0 )
{
	(new CComObject<CFile>)->QueryInterface( &m_pFile );
	m_buf = new BYTE[BLOCK_SIZE];
}
CTransfer::~CTransfer()
{
	delete m_buf;
}
