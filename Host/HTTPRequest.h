#pragma once

class CHTTPRequest
{
public:
	CComQIPtr<ScrRun::IDictionary> m_pHeaders;
	HTTP_METHOD m_method;
	WCHAR m_URI[8192];

	CHTTPRequest();
	~CHTTPRequest();

	BOOL IsMPFD();
	void Parse( BYTE* buf );
};
