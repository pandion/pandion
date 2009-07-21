#pragma once
#include "BasicThread.h"

class CPile;
class Socket;

class CListenThread :
	public CBasicThread
{
private:
	CComMultiThreadModel::AutoCriticalSection m_cs;

	std::vector<Socket*>	m_ListenSocks;

	IEventRouter*	m_pER;
	CPile*			m_pPile;

	HANDLE          m_hMutex;
public:
	CListenThread( CPile* p, IStream *pStream );
	~CListenThread();

	USHORT	AddPort( USHORT port );
	void	RemPort( USHORT port );

	void PreCommand();
	void PostCommand();
	void Command();
};