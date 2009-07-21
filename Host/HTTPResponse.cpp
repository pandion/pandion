#include "stdafx.h"
#include "HTTPResponse.h"

CHTTPResponse::CHTTPResponse()
{
	m_pHeaders.CoCreateInstance( OLESTR("Scripting.Dictionary") );
}
CHTTPResponse::~CHTTPResponse()
{
}
void CHTTPResponse::Parse( BYTE* buf )
{
	USES_CONVERSION;
	BYTE line[8192], line2[8192];

	VARIANT name, value;
	name.vt = value.vt = VT_BSTR;

	DWORD i;

	for( i = 1; buf[i-1] != '\r' && buf[i] != '\n'; line[i-2] = buf[++i - 2] );
	line[i-1] = 0;
	buf = buf + i + 1;

	sscanf( (char*)line, "%s %s", line2, line );
	m_dwCode = atol( (char*) line );

	while( buf[0] != '\r' && buf[1] != '\n' )
	{
        for( i = 1; buf[i-1] != '\r' && buf[i] != '\n'; line[i-2] = buf[++i - 2] );
		line[i-1] = 0;
		buf = buf + i + 1;
		if( strstr( (char*) line, ": " ) )
		{
			strstr( (char*) line, ": " )[0] = 0;
			name.bstrVal  = SysAllocString( A2OLE( (char*) line ) );
			value.bstrVal = SysAllocString( A2OLE( (char*) line + strlen( (char*) line ) + 2 ) );
			m_pHeaders->Add( &name, &value );
			SysFreeString( name.bstrVal  );
			SysFreeString( value.bstrVal );
		}
	}
}