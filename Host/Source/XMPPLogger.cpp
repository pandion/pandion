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
 * Filename:    XMPPLogger.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "XMPPLogger.h"

/*
 * Only log in debug builds.
 */
#define CANLOG _DEBUG

/*
 * Constructor
 */
XMPPLogger::XMPPLogger()
{
}

/*
 * Destructor
 */
XMPPLogger::~XMPPLogger()
{
}

/*
 * Log received data
 */
void XMPPLogger::LogReceived(const std::wstring& dataReceived)
{
#ifdef CANLOG
	std::wostringstream dbgmsg;
	dbgmsg << L"RECV (" << dataReceived.size() << L" bytes): \t" <<
		dataReceived << std::endl;
	Log(dbgmsg.str());
#endif
}

/*
 * Log sent data
 */
void XMPPLogger::LogSent(const std::wstring& dataSent)
{
#ifdef CANLOG
	bool hasEndline = (*(dataSent.end() - 1) == L'\n');
	std::wostringstream dbgmsg;
	dbgmsg << L"SENT (" << dataSent.size() << L" bytes): \t" << dataSent;
	if(!hasEndline)
	{
		dbgmsg << std::endl;
	}
	Log(dbgmsg.str());
#endif
}

/*
 * Log socket read errors
 */
void XMPPLogger::LogReadError()
{
#ifdef CANLOG
	LPTSTR ErrorMessage;
	DWORD ErrorCode = ::WSAGetLastError();
	::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL, ErrorCode, 0, (LPTSTR)&ErrorMessage, 0, NULL);
	std::wostringstream dbgMsg;
	dbgMsg << L"Read error: (ERROR 0x" <<
		std::hex <<	std::setw(8) <<	std::setfill(L'0') << 
		ErrorCode << L") " << ErrorMessage << std::endl;
	Log(dbgMsg.str());
	::LocalFree(ErrorMessage);
#endif
}

/*
 * Log error executing loadXML
 */
void XMPPLogger::LogLoadXMLError(MSXML2::IXMLDOMDocument* xmlDoc,
									const std::wstring& parsedData)
{
#ifdef CANLOG
	MSXML2::IXMLDOMParseErrorPtr errorObj;
	long errorCode, lineNumber, linePosition;
	BSTR errorReason, sourceString;
	xmlDoc->get_parseError(&errorObj);
	errorObj->get_errorCode(&errorCode);
	errorObj->get_line(&lineNumber);
	errorObj->get_linepos(&linePosition);
	errorObj->get_reason(&errorReason);
	errorObj->get_srcText(&sourceString);

	std::wostringstream errorMessage;
	errorMessage << L"ERROR: MSXML2::IXMLDOMDocument::loadXML failed " <<
		L"while attempting to load XML received from the XMPP server" <<
		std::endl <<
		L"ERROR: Error code:    0x" <<
		std::hex << errorCode << std::endl <<
		L"ERROR: Line number:   " <<
		std::dec << lineNumber << std::endl <<
		L"ERROR: Line position: " <<
		linePosition <<	std::endl <<
		L"ERROR: Reason:        " <<
		errorReason <<
		L"ERROR: Line Source:   " <<
		sourceString << std::endl <<
		L"ERROR: XML Source:    " <<
		parsedData << std::endl;
	Log(errorMessage.str());

	::SysFreeString(errorReason);
	::SysFreeString(sourceString);
#endif
}

/*
 * Write to the log
 */
void XMPPLogger::Log(const std::wstring& logEntry)
{
#ifdef CANLOG
	::OutputDebugString(logEntry.c_str());
#endif
}

