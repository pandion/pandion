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
 * Filename:    stdafx.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This header includes all headers which are oftenly used but
 *              seldomly changed. The purpose of this is to create a
 *              pre-compiled header file to speed up the recompilation process.
 */
#pragma once

/*
 * Target Windows 2000 or later:
 * http://msdn.microsoft.com/en-us/library/aa383745(VS.85).aspx
 */
#define WINVER 0x0500
#define _WIN32_WINNT 0x0500

/*
 * Target Microsoft Internet Explorer 7.0 or later:
 * http://msdn.microsoft.com/en-us/library/aa383745(VS.85).aspx
 */
#define _WIN32_IE 0x0700

/*
 * Demand explicit type casting:
 * http://msdn.microsoft.com/en-us/library/aa383681(VS.85).aspx
 */
#define STRICT

/*
 * Report memory leaks after running a debug build:
 * http://msdn.microsoft.com/en-us/library/10t349zs(VS.80).aspx
 */
#define _CRTDBG_MAP_ALLOC 

/*
 * Use Win32 security API.
 */
#define SECURITY_WIN32

/*
 * Request Common Controls v6 when available
 * http://msdn.microsoft.com/en-us/library/ms649781(VS.85).aspx
 */
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Wi"\
	"ndows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publi"\
	"cKeyToken='6595b64144ccf1df' language='*'\"")

/*
 * Microsoft Windows API headers
 */
#include <winsock2.h>
#include <windows.h>
#include <psapi.h>
#include <shellapi.h>
#include <commdlg.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <windns.h>
#include <Wincrypt.h>
#include <Schnlsp.h>
#include <Security.h>
#include <WinInet.h>
#include <Mstcpip.h>
#include <Ntdsapi.h>

/*
 * Compiler-specific headers
 */
#include <comdef.h>
#include <strsafe.h>
#include <crtdbg.h>

/*
 * Script Runtime headers
 */
#import <scrrun.dll> auto_rename \
	rename("FreeSpace", "FSpace") rename("Unknown", "Unk") \
	exclude("Folder") rename_namespace("ScrRun")

/*
 * Microsoft Internet Explorer-related headers and interfaces
 * NOTE: The most recent version of these headers are included in the 
 *       "Headers and Libraries for Windows Internet Explorer 8" SDK package
 *       available at:
 *       http://www.microsoft.com/downloads/details.aspx?FamilyID=bd98f760-1279-46f2-a456-de6df365584b
 */
#include <openservice.h>
#include <mshtml.h>
#include <mshtmhst.h>
#include <exdispid.h>
#pragma warning(disable : 4192)
#import <mshtml.tlb> \
	rename("TranslateAccelerator", "MSHTMLTranslateAccelerator")
#import <shdocvw.dll> exclude("tagREADYSTATE")

/*
 * ZLib headers
 */
#define ZLIB_WINAPI
#include "../zlib-1.2.3.win32/include/zlib.h"
#include "../zlib-1.2.3.win32/include/unzip.h"

/*
 * MSXML headers
 */
#import <msxml3.dll>

/*
 * Pandion COM interfaces
 */
#include "Pandion.h"

/*
 * Ignore deprecation warnings
 */
#pragma warning(disable : 4995)

/*
 * Ignore std::copy warnings
 */
#pragma warning(disable : 4996)

/*
 * ignore BOOL to bool performance warnings
 */
#pragma warning(disable : 4800)

/*
 * disable warning about using this in base member initializer list
 */
#pragma warning(disable : 4355)

/*
 * STL headers
 */
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include <memory>
#include <sstream>
#include <iomanip>

/*
 * Custom Window Messages
 */
const DWORD WM_JSINVOKE = WM_USER + 1001;
const DWORD WM_NOTIFYICON = WM_USER + 1002;
const DWORD WM_FORWARDMSG = WM_USER + 1003;

