#pragma once

#include "Transfer.h"

class CHTTPResponse;
struct CCSInfo;

class CClientGet : public CTransfer
{
private:
	CHTTPResponse* m_pResponse;
	CCSInfo* m_pCSInfo;

	DWORD m_nBytesToRecv;
	DWORD m_nTotalBytesToRecv;
public:
	CClientGet( IEventRouter*, CPile*, CCSInfo*, Socket*, DWORD );
	~CClientGet();

	DWORD SendRequest();
	DWORD RecvResponse();

	DWORD PreCommand();
	DWORD Command();
	DWORD PostCommand();

	DWORD GetProgress();
	DWORD GetLength();
};