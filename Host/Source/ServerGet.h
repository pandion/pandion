#pragma once
#include "Transfer.h"

class Socket;
class CHTTPRequest;
class CPile;
class CFile;

class CServerGet :
	public CTransfer
{
private:
	CHTTPRequest* m_pRequest;

	DWORD m_nBytesToSend;
	DWORD m_nTotalBytesToSend;
public:
	CServerGet( IEventRouter* pER, CPile* pPile, CHTTPRequest* pRequest, Socket* pSocket, DWORD sessionID );
	~CServerGet();

	DWORD PreCommand();
	DWORD Command();
	DWORD PostCommand();

	DWORD GetProgress();
	DWORD GetLength();
};
