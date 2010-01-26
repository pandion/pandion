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
 * Filename:    SCRAM.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This file implements some helper methods for authenticating
 *              using the Salted Challenge Response (SCRAM) SASL mechanism.
 *              See http://tools.ietf.org/html/draft-ietf-sasl-scram-10
 */
#include "stdafx.h"
#include "SCRAM.h"
#include "Base64.h"
#include "UTF8.h"

SCRAM::SCRAM()
{
}
SCRAM::~SCRAM()
{
}
STDMETHODIMP SCRAM::GenerateResponse(BSTR Challenge, BSTR *Response)
{
	return E_NOTIMPL;
}
void SCRAM::Error(LPWSTR Where, LPWSTR WhenCalling, DWORD ErrorCode)
{
	LPTSTR ErrorMessage;
	::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL, ErrorCode, 0, (LPTSTR)&ErrorMessage, 0, NULL);
	std::wostringstream dbgMsg;
	dbgMsg << L"SCRAM error in " << Where <<
		L" when calling " << WhenCalling <<
		std::hex <<	std::setw(8) <<	std::setfill(L'0') << 
		L": (ERROR 0x" << ErrorCode << L") " <<
		ErrorMessage <<	std::endl;
	OutputDebugString(dbgMsg.str().c_str());
	::LocalFree(ErrorMessage);
}