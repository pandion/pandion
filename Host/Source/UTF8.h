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
 * Filename:    UTF8.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This file defines two classes for ATL-style immediate
 *              conversion of UTF-8 strings to and from UTF-16 character
 *              strings. The classes are implemented in the header for
 *              performance reasons. Conversion is implemented through the
 *              WideCharToMultiByte and MultiByteToWideChar Windows API
 *              functions.
 */
#pragma once

/*
 * CW2UTF8EX<> Behaves similar to the ATL character conversion classes.
 */
template<int t_nBufferLength = 128>
class CW2UTF8EX
{
public:
	CW2UTF8EX(LPCWSTR psz) throw(...) :	m_psz(m_szBuffer)
	{
		Init(psz);
	}
	~CW2UTF8EX() throw()
	{
		if(m_psz != m_szBuffer)
		{
			delete m_psz;
		}
	}

	operator LPSTR() const throw()
	{
		return(m_psz);
	}

	operator std::string() const throw()
	{
		return std::string(m_psz);
	}

private:
	void Init(LPCWSTR psz) throw(...)
	{
		if(psz == NULL)
		{
			m_psz = NULL;
			return;
		}

		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, psz, lstrlenW(psz), 0, 0, NULL, NULL);

		if(nLen+1 > t_nBufferLength)
		{
			m_psz = new CHAR[nLen+1];
		}

		m_psz[::WideCharToMultiByte(CP_UTF8, 0, psz, lstrlenW(psz), m_psz, nLen, NULL, NULL)] = 0;
	}

public:
	LPSTR m_psz;
	char m_szBuffer[t_nBufferLength];

private:
	CW2UTF8EX(const CW2UTF8EX&) throw();
	CW2UTF8EX& operator=(const CW2UTF8EX&) throw();
};
typedef CW2UTF8EX<> CW2UTF8;

/*
 * CUTF82WEX<> Behaves similar to the ATL character conversion classes.
 */
template<int t_nBufferLength = 128>
class CUTF82WEX
{
public:
	CUTF82WEX(const unsigned char *psz) throw(...) : m_psz(m_szBuffer)
	{
		Init((const char *) psz);
	}
	CUTF82WEX(LPCSTR psz) throw(...) : m_psz(m_szBuffer)
	{
		Init(psz);
	}
	~CUTF82WEX() throw()
	{
		if(m_psz != m_szBuffer)
		{
			delete m_psz;
		}
	}

	operator LPWSTR() const throw()
	{
		return(m_psz);
	}

	operator std::wstring() const throw()
	{
		return std::wstring(m_psz);
	}

private:
	void Init(LPCSTR psz) throw(...)
	{
		if(psz == NULL)
		{
			m_psz = NULL;
			return;
		}
		
		int nLen = ::MultiByteToWideChar(CP_UTF8, 0, psz, lstrlenA(psz), 0, 0);
		
		if(nLen+1 > t_nBufferLength)
		{
			m_psz = new wchar_t[nLen+1];
		}

		m_psz[::MultiByteToWideChar(CP_UTF8, 0, psz, lstrlenA(psz), m_psz, nLen)] = 0;
	}

public:
	LPWSTR m_psz;
	wchar_t m_szBuffer[t_nBufferLength];

private:
	CUTF82WEX(const CUTF82WEX&) throw();
	CUTF82WEX& operator=(const CUTF82WEX&) throw();
};
typedef CUTF82WEX<> CUTF82W;

