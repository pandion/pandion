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
 * Filename:    Module.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#pragma once

const DWORD COPYDATA_CMDLINE = 0x0001;

class CMainWnd;

class CPandionModule : public CAtlExeModuleT< CPandionModule >
{
private:
	CMainWnd *m_pMainWnd;

	IHTTP *m_pHTTP;
	IXMPP *m_pXMPP;
	ISASL *m_pSASL;

	CComQIPtr< ScrRun::IDictionary > m_spGlobals;
	CComQIPtr< ScrRun::IDictionary > m_spWindows;
public :
	DECLARE_LIBID(LIBID_PandionLib)
	~CPandionModule();

	void GetMainWnd( CMainWnd **ppMainWnd );
	void GetHTTP( IHTTP **ppHTTP );
	void GetXMPP( IXMPP **ppXMPP );
	void GetSASL( ISASL **ppSASL );
	void GetGlobals( ScrRun::IDictionary **ppGlobals );
	void GetWindows( ScrRun::IDictionary **ppWindows );

	BOOL IsRunning();
	BOOL IsIEVersionOK();

	HRESULT PreMessageLoop( int nShowCmd );
	void    RunMessageLoop();
	HRESULT PostMessageLoop();
	BOOL    PreTranslateAccelerator( MSG* pMsg );
};