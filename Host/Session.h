#pragma once
#include "BasicThread.h"

class CPile;

class CSession : public CBasicThread
{
protected:
	DWORD         m_SessionID;
	IEventRouter *m_pER;
	CPile        *m_pPile;
public:
	CSession( CPile* p, IStream *pERStream );
	virtual ~CSession();

	DWORD GetSessionID();

	virtual void PreCommand();
	virtual void Command();
	virtual void PostCommand();

	virtual DWORD GetProgress() = 0;
	virtual DWORD GetLength() = 0;
	virtual void  Terminate() = 0;
private:
	static DWORD GetNewCookie();
};
