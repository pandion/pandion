#pragma once

class CPile;
class CHTTPRequest;
class Socket;

class CTransfer
{
protected:
	CComPtr<IEventRouter>	m_pER;
	CPile*					m_pPile;

	Socket*					m_pSocket;

	DWORD					m_SessionID;
	BOOL					m_bAborted;
protected:
	CComPtr<IPdnFile>		m_pFile;
	BYTE*					m_buf;
	DWORD					m_fp;
public:
	CTransfer( IEventRouter* pER, CPile* pPile, Socket* pSocket, DWORD sessionID );
	~CTransfer();

	virtual DWORD PreCommand() = 0;
	virtual DWORD Command() = 0;
	virtual DWORD PostCommand() = 0;

	virtual DWORD GetProgress() = 0;
	virtual DWORD GetLength() = 0;
};