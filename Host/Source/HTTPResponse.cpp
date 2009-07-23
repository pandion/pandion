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
 * Filename:    HTTPResponse.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "HTTPResponse.h"

CHTTPResponse::CHTTPResponse()
{
	m_pHeaders.CoCreateInstance(OLESTR("Scripting.Dictionary"));
}
CHTTPResponse::~CHTTPResponse()
{
}
void CHTTPResponse::Parse(BYTE* buf)
{
	USES_CONVERSION;
	BYTE line[8192], line2[8192];

	VARIANT name, value;
	name.vt = value.vt = VT_BSTR;

	DWORD i;

	for(i = 1; buf[i-1] != '\r' && buf[i] != '\n'; line[i-2] = buf[++i - 2]);
	line[i-1] = 0;
	buf = buf + i + 1;

	sscanf((char*)line, "%s %s", line2, line);
	m_dwCode = atol((char*) line);

	while(buf[0] != '\r' && buf[1] != '\n')
	{
        for(i = 1; buf[i-1] != '\r' && buf[i] != '\n'; line[i-2] = buf[++i - 2]);
		line[i-1] = 0;
		buf = buf + i + 1;
		if(strstr((char*) line, ": "))
		{
			strstr((char*) line, ": ")[0] = 0;
			name.bstrVal  = SysAllocString(A2OLE((char*) line));
			value.bstrVal = SysAllocString(A2OLE((char*) line + strlen((char*) line) + 2));
			m_pHeaders->Add(&name, &value);
			SysFreeString(name.bstrVal );
			SysFreeString(value.bstrVal);
		}
	}
}