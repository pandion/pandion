#pragma once
#include "Transfer.h"

class CFile;
class CPile;
class CHTTPRequest;
class CSocket;

class CServerMPFDPost :
	public CTransfer
{
private:
	BYTE*  m_buf;
	TCHAR   m_boundary[128];

	DWORD  m_fp;
	HANDLE m_hFile;
	CFile* m_pFile;
	CHTTPRequest* m_pRequest;

	DWORD  m_nBytesToRecv;
public:
	CServerMPFDPost( IEventRouter* pER, CPile* pPile, CHTTPRequest* pRequest, Socket* pSocket, DWORD sessionID );
	~CServerMPFDPost();

	bool EatBoundary();

	DWORD PreCommand();
	DWORD Command();
	DWORD PostCommand();

	DWORD GetProgress();
	DWORD GetLength();
};