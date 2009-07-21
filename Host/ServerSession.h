#pragma once
#include "Session.h"

class Socket;
class CPile;
class CTransfer;
class CHTTPRequest;

class CServerSession :
	public CSession
{
private:
	Socket* m_pSocket;
	CHTTPRequest* m_pRequest;

	CTransfer* m_pTransfer;
public:
	CServerSession( CPile* pPile, Socket* sock, IStream *pERStream );
	~CServerSession();

	DWORD RecvRequest();

	void PreCommand();
	void PostCommand();
	void Command();

	DWORD GetProgress();
	DWORD GetLength();
	void  Terminate();
};