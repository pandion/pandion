#include "stdafx.h"

#include "BasicThread.h"

CBasicThread::CBasicThread()
{
	m_continue = true;
}
CBasicThread::~CBasicThread()
{
}
HANDLE CBasicThread::GetHandle()
{
	return m_workerThread;
}
DWORD __stdcall CBasicThread::RunProc( void *pvoid )
{
	CBasicThread* pBT = (CBasicThread*) pvoid;
	return pBT->Run();
}
void CBasicThread::StartThread( int nPriority )
{
	DWORD ThreadID;
	m_continue = true;
	m_workerThread = CreateThread( NULL, 0, RunProc, (LPVOID) this, 0, &ThreadID );
	SetThreadPriority( m_workerThread, nPriority );
}
void CBasicThread::TerminateThread()
{
	SetContinue(false);
	::WaitForSingleObject( m_workerThread, INFINITE );
}
void CBasicThread::TerminateThreadInternal()
{
	SetContinue(false);
}
int CBasicThread::Run()
{
	CoInitializeEx( NULL, COINIT_APARTMENTTHREADED  );
	PreCommand();
	while( MayIContinue() )
		Command();
	PostCommand();
	return 0;
}
bool CBasicThread::MayIContinue()
{
	return GetContinue();
}
bool CBasicThread::GetContinue()
{
	m_CritSec.Lock();
	bool b = m_continue;
	m_CritSec.Unlock();
	return b;
}
void CBasicThread::SetContinue(bool val)
{
	m_CritSec.Lock();
	m_continue = val;
	m_CritSec.Unlock();
}