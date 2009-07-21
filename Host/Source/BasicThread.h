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
 * Filename:    BasicThread.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
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