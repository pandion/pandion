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
 * Filename:    External.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#include "stdafx.h"
#include "External.h"
#include "PopupMenu.h"
#include "NotifyIcon.h"
#include "MainWnd.h"
#include "File.h"
#include "Directory.h"
#include "XMPP.h"
#include "UTF8.h"
#include "shortcut.h"
#include "comctrlwrapper.h"
#include "module.h"
#include "Hash.h"
#include "Base64.h"

#include "pdnwnd.h"

External::External(CPdnWnd& Wnd) : 
	m_pModule(0), m_ComCtrl(Wnd), m_Wnd(Wnd)
{
	m_ComCtrl.DisableSelfDelete();
}
External::~External()
{
}

/* IExternal */
HRESULT External::Init(void* pModule)
{
	m_pModule = (PdnModule *) pModule;

	return S_OK;
}

STDMETHODIMP External::get_wnd(VARIANT* pDisp)
{
	pDisp->vt = VT_DISPATCH;
	return m_Wnd.QueryInterface(IID_IDispatch, (void**)&pDisp->pdispVal);
}
STDMETHODIMP External::get_mainWnd(VARIANT* pDisp)
{
	pDisp->vt = VT_DISPATCH;

	MainWnd *pMainWnd = m_pModule->GetMainWnd();
	pMainWnd->QueryInterface(IID_IDispatch, (void**)&pDisp->pdispVal);
	pMainWnd->Release();

	return S_OK;
}
STDMETHODIMP External::get_windows(VARIANT* pDisp)
{
	pDisp->vt = VT_DISPATCH;
	ScrRun::IDictionaryPtr pWindows = m_pModule->GetWindows();
	return pWindows->QueryInterface(IID_IDispatch, (void**)&pDisp->pdispVal);
}
STDMETHODIMP External::get_globals(VARIANT* pDisp)
{
	pDisp->vt = VT_DISPATCH;
	ScrRun::IDictionaryPtr pGlobals = m_pModule->GetGlobals();
	return pGlobals->QueryInterface(IID_IDispatch, (void**)&pDisp->pdispVal);
}
STDMETHODIMP External::get_ComCtrl(VARIANT* pDisp)
{
	pDisp->vt = VT_DISPATCH;
	return m_ComCtrl.QueryInterface(IID_IDispatch, (void**)&pDisp->pdispVal);
}
STDMETHODIMP External::get_XMPP(VARIANT *pDisp)
{
	pDisp->vt = VT_DISPATCH;
	pDisp->pdispVal = m_pModule->GetXMPP();
	return S_OK;
}
STDMETHODIMP External::get_SASL(VARIANT* pDisp)
{
	pDisp->vt = VT_DISPATCH;
	pDisp->pdispVal = m_pModule->GetSASL();
	return S_OK;
}
STDMETHODIMP External::get_HTTPEngine(VARIANT* pDisp)
{
	pDisp->vt = VT_DISPATCH;
	pDisp->pdispVal = m_pModule->GetHTTP();
	return S_OK;
}
STDMETHODIMP External::createWindow(BSTR name, BSTR file, VARIANT* params,
									BOOL bPopUnder, VARIANT* pDisp)
{
	RECT rc = { 300, 300, rc.left + 500, rc.top + 300 };
	CPdnWnd* newWindow = new CPdnWnd;
    newWindow->Create(rc, name, file, _variant_t(params), m_pModule);

	newWindow->QueryInterface(IID_IDispatch, (void**)&pDisp->pdispVal);
	pDisp->vt = VT_DISPATCH;
	return S_OK;
}
STDMETHODIMP External::shellExec(BSTR verb, BSTR file, BSTR params,
	BSTR dir, DWORD nShowCmd)
{
	::ShellExecute(::GetDesktopWindow(), verb, file, params, dir, nShowCmd);
	return S_OK;
}
STDMETHODIMP External::get_cursorX(VARIANT* retVal)
{
	POINT pt;
	::GetCursorPos(&pt);
	retVal->intVal = pt.x;
	retVal->vt = VT_I4;
	return S_OK;
}
STDMETHODIMP External::get_cursorY(VARIANT* retVal)
{
	POINT pt;
	::GetCursorPos(&pt);
	retVal->intVal = pt.y;
	retVal->vt = VT_I4;
	return S_OK;
}
STDMETHODIMP External::get_notifyIcon(VARIANT* pDisp)
{
	MainWnd *pMain = m_pModule->GetMainWnd();
	pMain->GetNotifyIcon(pDisp);
	pMain->Release();

	return S_OK;
}
STDMETHODIMP External::get_newPopupMenu(VARIANT* pDisp)
{
	(new CPopupMenu)->QueryInterface(IID_IDispatch, (void**)&(pDisp->pdispVal));
	pDisp->vt = VT_DISPATCH;
	return S_OK;
}
STDMETHODIMP External::get_IPs(VARIANT* pStr)
{
	pStr->vt = VT_BSTR;
	pStr->bstrVal = SysAllocString(L"error");

	char strHostName[1024];
	if(gethostname(strHostName, 1024) == SOCKET_ERROR) return S_FALSE;
	hostent* pHost = gethostbyname(strHostName);

	TCHAR strAddr[20];
	_bstr_t retStr = "";
	for(int i = 0; pHost->h_addr_list[i] != 0; i++)
	{
		_stprintf(strAddr, TEXT("%u.%u.%u.%u"),
			(UCHAR) pHost->h_addr_list[i][0],
			(UCHAR) pHost->h_addr_list[i][1],
			(UCHAR) pHost->h_addr_list[i][2],
			(UCHAR) pHost->h_addr_list[i][3]);
		retStr += strAddr;
		retStr += TEXT(";");
	}

	SysFreeString(pStr->bstrVal);
	pStr->bstrVal = retStr.Detach();

	return S_OK;
}
STDMETHODIMP External::get_CmdLine(VARIANT* pStr)
{
	pStr->vt = VT_BSTR;
	pStr->bstrVal = ::SysAllocString(GetCommandLineW());
	return S_OK;
}
STDMETHODIMP External::sleep(DWORD dwMilliseconds)
{
	::Sleep(dwMilliseconds);
	return S_OK;
}
STDMETHODIMP External::File(BSTR path, VARIANT *pDisp)
{
	pDisp->vt = VT_DISPATCH;
	(new CFile)->QueryInterface(IID_IDispatch, (LPVOID*) &pDisp->pdispVal);
    
	((IPdnFile*)pDisp->pdispVal)->Create(path, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_ALWAYS);
	
	return S_OK;
}
STDMETHODIMP External::FileExists(BSTR path, BOOL *bExists)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(path, &FindFileData);

	if(hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
		*bExists = TRUE;
	}
	else
		*bExists = FALSE;
    return S_OK;
}
STDMETHODIMP External::get_Directory(VARIANT *pDisp)
{
	pDisp->vt = VT_DISPATCH;
	return (new CDirectory)->QueryInterface(
		IID_IDispatch, (LPVOID*) &pDisp->pdispVal);
}
STDMETHODIMP External::StringToSHA1(BSTR str, BSTR *strSHA1)
{
	LPSTR szUTF8Buf = CW2UTF8(str);
	unsigned char digest[20];
	
	Hash::SHA1((unsigned char*)szUTF8Buf, strlen(szUTF8Buf), digest);

	wchar_t hexHash[20*2+1];
	hexHash[20*2] = 0;
	for(unsigned i = 0; i <20; i++)
	{
		hexHash[2*i] = (digest[i] >> 4) + 
			(((digest[i] >> 4) < 0xA) ? L'0' : (L'a' - 0xA));
		hexHash[2*i+1] = (digest[i] & 0x0F) + 
			(((digest[i] & 0x0F) < 0xA) ? L'0' : (L'a' - 0xA));
	}
	*strSHA1 = _bstr_t(hexHash).Detach();
	return S_OK;
}
STDMETHODIMP External::GetSpecialFolder(int nFolder, BSTR *Path)
{
	TCHAR szPath[MAX_PATH];

	if(SUCCEEDED(::SHGetFolderPath(NULL, nFolder, NULL,
		SHGFP_TYPE_CURRENT, szPath)))
	{
		*Path = ::SysAllocString(szPath);
	}
	else
	{
        *Path = ::SysAllocString(L"");
	}
	return S_OK;
}

STDMETHODIMP External::RegRead(BSTR hKey, BSTR key, BSTR value,
	VARIANT* vRetVal)
{
	HKEY RegKey, RootKey = StringToRegRootKey(hKey);
	ULONG uBytes = 0, dwType = 0;
	LSTATUS err = ::RegOpenKeyEx(RootKey, key, 0, KEY_READ, &RegKey);
	err = ::RegQueryValueEx(RegKey, value, NULL, &dwType, NULL, &uBytes);
	BYTE* pData = new BYTE[uBytes];
	err = ::RegQueryValueEx(RegKey, value, NULL, &dwType, pData, &uBytes);
	::RegCloseKey(RegKey);

	HRESULT hr = S_OK;
	if(err == ERROR_SUCCESS && dwType == REG_DWORD)
		*vRetVal = _variant_t(*(DWORD*)pData).Detach();
	else if(err == ERROR_SUCCESS && dwType == REG_SZ)
		*vRetVal = _variant_t((LPTSTR)pData).Detach();
	else
		hr = E_FAIL;

	delete[] pData;
	return hr;
}
STDMETHODIMP External::RegWriteString(BSTR hKey, BSTR key, BSTR value,
	BSTR data)
{
	HKEY RegKey, RootKey = StringToRegRootKey(hKey);
	LSTATUS err = ERROR_SUCCESS;
	err = ::RegOpenKeyEx(RootKey, key, 0, KEY_WRITE, &RegKey);
	err = ::RegSetValueEx(RegKey, value, NULL, REG_SZ, (const BYTE*) data,
		::SysStringByteLen(data) + 1);
	::RegCloseKey(RegKey);
	return err == ERROR_SUCCESS ? S_OK : E_FAIL;
}
STDMETHODIMP External::RegWriteDWORD(BSTR hKey, BSTR key, BSTR value,
	DWORD data)
{
	HKEY RegKey, RootKey = StringToRegRootKey(hKey);
	LSTATUS err = ERROR_SUCCESS;
	err = ::RegOpenKeyEx(RootKey, key, 0, KEY_WRITE, &RegKey);
	err = ::RegSetValueEx(RegKey, value, NULL, REG_DWORD, (const BYTE*) &data,
		sizeof(DWORD));
	::RegCloseKey(RegKey);
	return err == ERROR_SUCCESS ? S_OK : E_FAIL;
}
STDMETHODIMP External::get_Shortcut(VARIANT *pDisp)
{
	pDisp->vt = VT_DISPATCH;
	return (new Shortcut)->QueryInterface(IID_IDispatch,
		(LPVOID*) &pDisp->pdispVal);
}
STDMETHODIMP External::UnZip(BSTR path, BSTR targetDir, int *nSuccess)
{
	*nSuccess = 0;
	unzFile pZipFile = unzOpen(CW2UTF8(path));

	if(!pZipFile)
	{
        *nSuccess = -1;
		return S_OK;
	}

	if(unzGoToFirstFile(pZipFile) != UNZ_OK)
	{
		*nSuccess = -1;
		unzClose(pZipFile);
		return S_OK;
	}

	do
	{
		if(unzOpenCurrentFile(pZipFile) != UNZ_OK)
		{
			*nSuccess = -1;
			break;
		}
		unz_file_info_s file_info;
		char file_name[MAX_PATH] = "";
		char file_path[MAX_PATH] = "";

		unzGetCurrentFileInfo(pZipFile, &file_info,
			file_name, MAX_PATH, 0, 0, 0, 0);

		::StringCchCopyA(file_path, MAX_PATH, CW2UTF8(targetDir));
		::PathAppendA(file_path, file_name);

		IPdnFile *target_file;
		(new CFile)->QueryInterface(__uuidof(IPdnFile),
			(LPVOID*) &target_file);
		if(target_file->Create(_bstr_t(CUTF82W(file_path)),
			GENERIC_WRITE, FILE_SHARE_READ,	OPEN_ALWAYS) == S_OK)
		{
			int nRead = 0;
			target_file->Seek(file_info.uncompressed_size, 0, FILE_BEGIN);
			target_file->SetEOF();
			target_file->Seek(0, 0, FILE_BEGIN);
			do
			{	
				BYTE buf[4096];
				nRead =	unzReadCurrentFile(pZipFile, buf, sizeof(buf));
				target_file->Write(buf, nRead);
			}
			while(nRead > 0);
		}
		else
		{
			*nSuccess = -1;
		}
		target_file->Release();
		unzCloseCurrentFile(pZipFile);
	}
	while(unzGoToNextFile(pZipFile) == UNZ_OK);

	unzClose(pZipFile);
	return S_OK;
}
STDMETHODIMP External::Base64ToString(BSTR b64String, BSTR *UTF16String)
{
	DWORD decodeBufferSize;
	::CryptStringToBinary(b64String, ::SysStringLen(b64String),
		CRYPT_STRING_BASE64, NULL, &decodeBufferSize, NULL, NULL);
	std::vector<BYTE> decodeBuffer(decodeBufferSize);
	::CryptStringToBinary(b64String, ::SysStringLen(b64String),
		CRYPT_STRING_BASE64, &decodeBuffer[0], &decodeBufferSize, NULL, NULL);
	decodeBuffer.push_back('\0');
	*UTF16String = ::SysAllocString(CUTF82W((LPSTR) &decodeBuffer[0]));

	return S_OK;
}
STDMETHODIMP External::StringToBase64(BSTR UTF16String, BSTR *b64String)
{
	*b64String = ::SysAllocString(
		Base64::Base64Encode(UTF16String,
		::SysStringLen(UTF16String), false).c_str());
	return S_OK;
}
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	static int nFullScreen = 0;
	RECT WindowRect;
	GetWindowRect(hwnd, &WindowRect);

	if(WindowRect.left == 0 && WindowRect.top == 0 &&
		WindowRect.right == GetSystemMetrics(SM_CXSCREEN) &&
		WindowRect.bottom == GetSystemMetrics(SM_CYSCREEN))
	{
		nFullScreen++;
		if(nFullScreen> 2)
			*((bool *)lParam) = true;
	}
	return true;
}
STDMETHODIMP External::Fullscreen(BOOL *bFullscreen)
{
	*bFullscreen = false;
	EnumWindows(EnumWindowsProc, (LPARAM) bFullscreen);
	return S_OK;
}
STDMETHODIMP External::PlaySound(BSTR soundFile)
{
	DWORD flags = SND_FILENAME | SND_ASYNC;
	::PlaySound(soundFile, NULL, flags);
	return S_OK;
}
HKEY External::StringToRegRootKey(BSTR strHKey)
{
	HKEY RootKey;
	if(wcsstr(strHKey, L"HKEY_CLASSES_ROOT"))
		RootKey = HKEY_CLASSES_ROOT;
	else if(wcsstr(strHKey, L"HKEY_CURRENT_USER"))
		RootKey = HKEY_CURRENT_USER;
	else if(wcsstr(strHKey, L"HKEY_LOCAL_MACHINE"))
		RootKey = HKEY_LOCAL_MACHINE;
	else if(wcsstr(strHKey, L"HKEY_USERS"))
		RootKey = HKEY_USERS;
	else if(wcsstr(strHKey, L"HKEY_PERFORMANCE_DATA"))
		RootKey = HKEY_PERFORMANCE_DATA;
	else if(wcsstr(strHKey, L"HKEY_PERFORMANCE_TEXT"))
		RootKey = HKEY_PERFORMANCE_TEXT;
	else if(wcsstr(strHKey, L"HKEY_PERFORMANCE_NLSTEXT"))
		RootKey = HKEY_PERFORMANCE_NLSTEXT;
	else if(wcsstr(strHKey, L"HKEY_CURRENT_CONFIG"))
		RootKey = HKEY_CURRENT_CONFIG;
	else if(wcsstr(strHKey, L"HKEY_DYN_DATA"))
		RootKey = HKEY_DYN_DATA;
	else
		RootKey = NULL;
	return RootKey;
}