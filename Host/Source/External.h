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
 * Filename:    External.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#pragma once
#include "ComCtrlWrapper.h"

struct CTypeInfo
{
	LPOLESTR name;
	DWORD dispID;
};

class CPdnWnd;
class PdnModule;

class External :
	public DispInterfaceImpl<IExternal>
{
private:
	CComCtrl		m_ComCtrl;
	CPdnWnd&		m_Wnd;
	PdnModule*	m_pModule;
public:
	External(CPdnWnd& Wnd);
	~External();

	STDMETHOD(Init)(void *pModule);

	/* IExternal implementation */
	STDMETHOD(get_wnd)(VARIANT *pDisp);
	STDMETHOD(get_mainWnd)(VARIANT *pDisp);
	STDMETHOD(get_windows)(VARIANT *pDisp);
	STDMETHOD(get_globals)(VARIANT *pDisp);
	STDMETHOD(get_ComCtrl)(VARIANT *pDisp);
	STDMETHOD(get_HTTPEngine)(VARIANT *pDisp);
	STDMETHOD(get_SASL)(VARIANT *pDisp);
	STDMETHOD(createWindow)(BSTR name, BSTR file, VARIANT *params,
		BOOL bPopUnder = TRUE, VARIANT *pDisp = NULL);
	STDMETHOD(shellExec)(BSTR verb, BSTR file, BSTR params, BSTR dir,
		DWORD nShowCmd);

	STDMETHOD(get_cursorX)(VARIANT *retVal);
	STDMETHOD(get_cursorY)(VARIANT *retVal);

	STDMETHOD(get_notifyIcon)(VARIANT *pDisp);
	STDMETHOD(get_newPopupMenu)(VARIANT *pDisp);

	STDMETHOD(get_IPs)(VARIANT *pStr);
	STDMETHOD(get_CmdLine)(VARIANT *pStr);

	STDMETHOD(sleep)(DWORD dwMilliseconds);

	STDMETHOD(File)(BSTR path, VARIANT *pDisp);
	STDMETHOD(FileExists)(BSTR path, BOOL *bExists);
	STDMETHOD(get_Directory)(VARIANT *pDisp);
	STDMETHOD(get_XMPP)(VARIANT *pDisp);
	STDMETHOD(StringToSHA1)(BSTR str, BSTR *strSHA1);
	STDMETHOD(GetSpecialFolder)(int nFolder, BSTR* Path);
	
	STDMETHOD(RegRead)(BSTR hKey, BSTR key, BSTR value, VARIANT* vRetVal);
	STDMETHOD(RegWriteString)(BSTR hKey, BSTR key, BSTR value, BSTR data);
	STDMETHOD(RegWriteDWORD)(BSTR hKey, BSTR key, BSTR value, DWORD data);

	STDMETHOD(get_Shortcut)(VARIANT *pDisp);
	STDMETHOD(UnZip)(BSTR path, BSTR targetDir, int *nSuccess);
	STDMETHOD(Base64ToString)(BSTR b64String, BSTR *UTF16String);
	STDMETHOD(StringToBase64)(BSTR UTF16String, BSTR *b64String);
	STDMETHOD(Fullscreen)(BOOL *bFullscreen);
	STDMETHOD(PlaySound)(BSTR soundFile);
	STDMETHOD(SetDefaultMSIESearchProvider)(BSTR url);

private:
	HKEY StringToRegRootKey(BSTR str);
};
