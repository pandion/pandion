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
 * Filename:    Pile.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
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
	CSharedFile(_bstr_t filename, _bstr_t url) : m_FileName(filename), m_URL(url)
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

	_bstr_t GetFileNameByURL(_bstr_t URL);
	DWORD   GetCookieByURL(_bstr_t URL);

	void    AddSharedFile(CSharedFile* sf);
	HRESULT DelSharedFile(DWORD sfCookie );
	
	void    AddSession(CSession*);
	HRESULT DelSession(DWORD);

	CSession* GetSession(DWORD sessionID);
}; 