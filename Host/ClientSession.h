#pragma once
#include "Session.h"

class CProxyInfo;
class CPile;
class CFile;
class Socket;
class CHTTPResponse;
class CTransfer;

struct CCSInfo
{
	CProxyInfo*	pProxyInfo;
	WCHAR		address[128];
	USHORT		port;
	
	DWORD		method;
	WCHAR		URI[1024];

	IPdnFile*	pFile;
	DWORD		offset;
	DWORD		len;
};

class CClientSession :
	public CSession
{
private:
	Socket* m_pSocket;
	
	CCSInfo*       m_pCSInfo;
	CTransfer*     m_pTransfer;
public:
	CClientSession( CPile* pPile, IStream* pERStream, CCSInfo* pCSInfo );
	~CClientSession();

	DWORD ClientConnect();

	void PreCommand();
	void PostCommand();
	void Command();

	DWORD GetProgress();
	DWORD GetLength();
	void  Terminate();
};