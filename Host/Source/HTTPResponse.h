#pragma once

class CHTTPResponse
{
public:
	CHTTPResponse();
	~CHTTPResponse();

	CComQIPtr<ScrRun::IDictionary> m_pHeaders;
	DWORD m_dwCode;

	void Parse( BYTE* buf );
};
