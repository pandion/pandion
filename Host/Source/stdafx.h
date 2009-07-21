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

/* Target Windows 200 or later:
 * http://msdn.microsoft.com/en-us/library/aa383745(VS.85).aspx
 */
#define WINVER 0x0500
#define _WIN32_WINDOWS 0x0500
#define _WIN32_WINNT 0x0500

/*
 * Target Microsoft Internet Explorer 5.5 or later:
 * http://msdn.microsoft.com/en-us/library/aa383745(VS.85).aspx
 */
#define _WIN32_IE 0x0550

/*
 * Demand explicit type casting:
 * http://msdn.microsoft.com/en-us/library/aa383681(VS.85).aspx
 */
#define STRICT

/*
 * Configure the ATL library:
 * http://msdn.microsoft.com/en-us/library/kewhz9ce(VS.80).aspx
 */
#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_ALL_WARNINGS

/*
 * Report memory leaks after running a debug build:
 * http://msdn.microsoft.com/en-us/library/10t349zs(VS.80).aspx
 */
#define _CRTDBG_MAP_ALLOC 

/*
 * Disable warnings about unsafe string methods used by ATL:
 * http://msdn.microsoft.com/en-us/library/8ef0s5kh(VS.80).aspx
 */
#define _CRT_SECURE_NO_WARNINGS

/*
 * Use Win32 security API.
 */
#define SECURITY_WIN32

/*
 * Microsoft Windows API headers
 */
#include <windows.h>
#include <shellapi.h>
#include <commdlg.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <windns.h>
#include <Wincrypt.h>
#include <Schnlsp.h>
#include <Security.h>

/*
 * Microsoft Internet Explorer-related headers and interfaces
 */
#include <mshtml.h>
#include <urlmon.h>
#include <mshtmhst.h>
#include <exdispid.h>
#pragma warning(disable : 4192)
#import "mshtml.tlb" \
	rename("TranslateAccelerator", "MSHTMLTranslateAccelerator")
#import "shdocvw.dll" exclude("tagREADYSTATE")

/*
 * ScrRun has to be loaded in a separate namespace because of a naming conflict
 * with ShlObj.
 */
namespace ScrRun
{
	#include "scrrun.h"
}

/*
 * ATL headers
 */
#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atlenc.h>
#include <atlsafe.h>
#include <atlcoll.h>
using namespace ATL;

/*
 * ZLib headers
 */
#define ZLIB_WINAPI
#include "../zlib-1.2.3.win32/include/zlib.h"
#include "../zlib-1.2.3.win32/include/unzip.h"

/*
 * MSXML headers
 */
#import <msxml3.dll> raw_interfaces_only

/*
 * Compiler-specific headers
 */
#include <comdef.h>
#include <strsafe.h>

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

/*
 * HTTP Constants
 */
const DWORD BLOCK_SIZE = 0x2000;
enum HTTP_METHOD {
	HTTP_METHOD_UNKNOWN = -1, // Unknown request method
	HTTP_METHOD_GET,          // GET request
	HTTP_METHOD_PUT,          // PUT request
	HTTP_METHOD_POST,         // POST request
	HTTP_METHOD_MPFDPOST,     // multipart/form-data encoded POST request
	HTTP_METHOD_HEAD,         // HEAD request
	HTTP_METHOD_DELETE,       // DELETE request
	HTTP_METHOD_LINK,         // LINK request
	HTTP_METHOD_UNLINK,       // UNLINK request
	HTTP_METHOD_DEBUG         // Debugging support
};
