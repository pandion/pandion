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
 * Filename:    Module.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#include "stdafx.h"
#include "MainWnd.h"
#include "Module.h"

PdnModule::PdnModule()
{
	m_XMPP.SetMainWnd(&m_MainWnd);

	/* Disable Self Deletion for objects created on the stack */
	m_MainWnd.DisableSelfDelete();
	m_XMPP.DisableSelfDelete();
	m_SASL.DisableSelfDelete();
	m_HTTP.DisableSelfDelete();

	/* Create the COM objects */
	m_Globals.CreateInstance(OLESTR("Scripting.Dictionary"));
	m_Windows.CreateInstance(OLESTR("Scripting.Dictionary"));

	/* Add the path to this module to the globals dictionary */
	wchar_t cwd[MAX_PATH];
	::GetModuleFileName(NULL, cwd, MAX_PATH);
	::PathRemoveFileSpec(cwd);
	::PathAppend(cwd, L"src");
	::PathAddBackslash(cwd);
	m_Globals->Add(&_variant_t(L"cwd"), &_variant_t(cwd));

	/* Set the path to this module as the working dictionary */
	::SetCurrentDirectory(cwd);
}
PdnModule::~PdnModule()
{
}
MainWnd* PdnModule::GetMainWnd()
{
	m_MainWnd.AddRef();
	return &m_MainWnd;
}
IDispatch* PdnModule::GetHTTP()
{
	m_HTTP.AddRef();
	return dynamic_cast<IDispatch*>(&m_HTTP);
}
IDispatch* PdnModule::GetXMPP()
{
	m_XMPP.AddRef();
	return dynamic_cast<IDispatch*>(&m_XMPP);
}
IDispatch* PdnModule::GetSASL()
{
	m_SASL.AddRef();
	return dynamic_cast<IDispatch*>(&m_SASL);
}
ScrRun::IDictionary* PdnModule::GetGlobals()
{
	m_Globals.AddRef();
	return m_Globals.GetInterfacePtr();
}
ScrRun::IDictionary* PdnModule::GetWindows()
{
	m_Windows.AddRef();
	return m_Windows.GetInterfacePtr();
}
void PdnModule::PreMessageLoop(int nShowCmd)
{
	::SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc,
		NULL, ::GetCurrentThreadId());

	wchar_t path[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, &path[0]);
	::PathAddBackslash(path);
	::PathAppend(path, L"main.html");

	RECT rc = {100, 100, rc.left + 420, rc.top + 310};
	m_MainWnd.Create(rc, L"MainWindow", path, _variant_t(0), this);
}
void PdnModule::RunMessageLoop()
{
	MSG msg;
	while(::GetMessage(&msg, 0, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}
void PdnModule::PostMessageLoop()
{
}
LRESULT CALLBACK PdnModule::GetMsgProc(int code, WPARAM wParam, LPARAM lParam)
{
	LPMSG msg = (LPMSG) lParam;
	if((msg->message >= WM_KEYFIRST && msg->message <= WM_KEYLAST) ||
		(msg->message >= WM_MOUSEFIRST && msg->message <= WM_MOUSELAST))
	{
		HWND TargetWnd = GetAncestor(msg->hwnd, GA_ROOT);
		if(TargetWnd != msg->hwnd &&
			::SendMessage(TargetWnd, WM_FORWARDMSG, 0, (LPARAM) msg) == 1)
		{
			msg->message = WM_NULL;
			msg->hwnd = NULL;
			msg->lParam = 0;
			msg->wParam = 0;
		}
	}
	return ::CallNextHookEx(NULL, code, wParam, lParam);
}
static bool IsIEVersionOK()
{
	HINSTANCE hDllInst = ::LoadLibrary(L"shdocvw.dll");
	if(NULL == hDllInst)
	{
		return false;
	}
	FARPROC pDllGetVersion = ::GetProcAddress(hDllInst, "DllGetVersion");
	if(NULL == pDllGetVersion)
	{
		::FreeLibrary(hDllInst);
		return false;
	}
	DLLVERSIONINFO dvi = {sizeof(dvi), 0, 0, 0, 0};
	HRESULT hr = (*(DLLGETVERSIONPROC)pDllGetVersion)(&dvi);
	::FreeLibrary(hDllInst);

	return (dvi.dwMajorVersion == 5 && dvi.dwMinorVersion >= 50) ||
		dvi.dwMajorVersion >= 6;
}
static void HandleAlreadyRunning()
{
	HWND h = MainWnd::GetMainWindow();
	if(!h)
	{
		return;
	}

	/* Make the already running main window visible and put it on foreground */
	if(!(WS_VISIBLE & ::GetWindowLong(h, GWL_STYLE)))
	{
		::ShowWindow(h, SW_RESTORE);
	}
	::SetForegroundWindow(h);

	/* Send our command line to the already running window */
    COPYDATASTRUCT CDS;
	CDS.dwData = COPYDATA_CMDLINE;
	CDS.lpData = ::GetCommandLine();
	CDS.cbData = (::wcslen((const wchar_t *) CDS.lpData) + 1) * sizeof(wchar_t);
	::SendMessage(h, WM_COPYDATA, (WPARAM) 0, (LPARAM) &CDS);

	/* Kill this process */
	::ExitProcess(0);
}
int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPSTR /*lpCmdLine*/, int nShowCmd)
{
	/* Check for memory leaks in the debug builds */
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
//	_CrtSetBreakAlloc(3035);
#endif

	/* Check The Version of Internet Explorer */
	if(!IsIEVersionOK())
	{
		::MessageBox(NULL,
			L"This program requires Microsoft Internet Explorer "
			L"version 5.5 or higher.\nVisit this website to upgrade: "
			L"http://www.microsoft.com/ie",
			L"Failed to load program", MB_OK | MB_ICONERROR);
		return -1;
	}

	/* Initialize Winsock 2.0 */
	WSADATA wsad;
	if(::WSAStartup(MAKEWORD(2,0), &wsad))
	{
		::MessageBox(NULL,
			L"Failed to initialize Winsock 2.0", 
			L"Failed to load program", MB_OK | MB_ICONERROR);
		return -1;
	}

	/* Initialize OLE */
	::OleInitialize(NULL);

	/* Check if the software is already running and take action accordingly */
	HandleAlreadyRunning();

	/* Main Program */
	PdnModule theModule;
	theModule.PreMessageLoop(nShowCmd);
	theModule.RunMessageLoop();
	theModule.PostMessageLoop();

	/* Cleanup OLE */
	::OleUninitialize();

	/* Cleanup Winsock */
	::WSACleanup();

	return 0;
}
