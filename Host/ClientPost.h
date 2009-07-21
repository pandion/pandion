#pragma once

#include "Transfer.h"

class CHTTPResponse;
struct CCSInfo;

class CClientPost : public CTransfer
{
private:
	CHTTPResponse* m_pResponse;
	CCSInfo* m_pCSInfo;

	DWORD m_nBytesToSend;
	DWORD m_nTotalBytesToSend;
public:
	CClientPost( IEventRouter*, CPile*, CCSInfo*, Socket*, DWORD );
	~CClientPost();

	DWORD SendRequest();
	DWORD RecvResponse();

	DWORD PreCommand();
	DWORD Command();
	DWORD PostCommand();

	DWORD GetProgress();
	DWORD GetLength();
};