#pragma once

#include "Transfer.h"

class CHTTPRequest;
class CFile;
struct CCSInfo;

class CServerPost : public CTransfer
{
private:
	CHTTPRequest* m_pRequest;

	DWORD m_nBytesToRecv;
	DWORD m_nTotalBytesToRecv;
public:
	CServerPost( IEventRouter*, CPile*, CHTTPRequest*, Socket*, DWORD );
	~CServerPost();

	DWORD SendRequest();
	DWORD RecvResponse();

	DWORD PreCommand();
	DWORD Command();
	DWORD PostCommand();

	DWORD GetProgress();
	DWORD GetLength();
};