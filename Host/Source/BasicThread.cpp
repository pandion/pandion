/**
 * This file is part of Pandion.
 *
 * Pandion is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pandion is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Pandion.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Filename:    BasicThread.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
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