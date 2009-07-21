#pragma once

class CBasicThread
{
private:
	bool m_continue;
	HANDLE m_workerThread;
	CComMultiThreadModel::AutoCriticalSection m_CritSec;
public: 
	CBasicThread();
	virtual ~CBasicThread();

	HANDLE GetHandle();

	static DWORD __stdcall RunProc( void *pvoid );
	void StartThread( int nPriority = THREAD_PRIORITY_NORMAL );
	void TerminateThread();
	void TerminateThreadInternal();

	virtual void PreCommand() = 0;
	virtual void PostCommand() = 0;
	virtual void Command() = 0;
protected:
	int Run();
	virtual bool MayIContinue();
private:
	bool GetContinue();
	void SetContinue(bool val);
};