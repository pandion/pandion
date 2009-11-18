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

#include "XMPP.h"
#include "SASL.h"
#include "HTTP.h"
#include "MainWnd.h"

const DWORD COPYDATA_CMDLINE = 0x0001;

class PdnModule
{
private:
	MainWnd                m_MainWnd;
	XMPP                   m_XMPP;
	SASL                   m_SASL;
	HTTP                   m_HTTP;

	ScrRun::IDictionaryPtr m_Globals;
	ScrRun::IDictionaryPtr m_Windows;
public :
	PdnModule();
	~PdnModule();

	MainWnd*             GetMainWnd();
	IDispatch*           GetHTTP();
	IDispatch*           GetXMPP();
	IDispatch*           GetSASL();
	ScrRun::IDictionary* GetGlobals();
	ScrRun::IDictionary* GetWindows();

	void PreMessageLoop(int nShowCmd);
	void RunMessageLoop();
	void PostMessageLoop();

private:
	static LRESULT CALLBACK GetMsgProc(int code, WPARAM wParam, LPARAM lParam);
};
