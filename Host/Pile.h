#pragma once

class CSession;
class CServerSession;
class CClientSession;

class CSharedFile
{
private:
	DWORD GetNewCookie()
	{
		static int lastCookie = 0;
		return ++lastCookie;
	}
	DWORD m_Cookie;
public:
	CSharedFile( _bstr_t filename, _bstr_t url ) : m_FileName( filename ), m_URL( url )
	{
		m_Cookie = GetNewCookie();
	}
	DWORD GetCookie()
	{
		return m_Cookie;
	}

	_bstr_t m_URL;
	_bstr_t m_FileName;
};

class CPile
{
private:
	std::vector<CSharedFile*> m_SharedFiles;
	std::vector<CSession*> m_Sessions;
public:
	CPile();
	~CPile();

	_bstr_t GetFileNameByURL( _bstr_t URL );
	DWORD   GetCookieByURL( _bstr_t URL );

	void    AddSharedFile( CSharedFile* sf );
	HRESULT DelSharedFile( DWORD sfCookie  );
	
	void    AddSession( CSession* );
	HRESULT DelSession( DWORD );

	CSession* GetSession( DWORD sessionID );
}; 