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
 * Filename:    File.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This file declares a COM class that offers the agent
 *              facilities for creating, editing and deleting files.
 */
#pragma once

/*
 * This COM class provides the agent with facilities for creating, editing
 * deleting files.
 */
class CFile :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IPdnFile>
{
private:
	HANDLE   m_FileHandle;
	_bstr_t  m_FileName;

	BOOL     m_HasWriteAccess;
	BOOL	 m_AtEnd;

	LPSTR    m_DataBuffer;
	LPSTR    m_CurrentPosition;
public:
	CFile();
	~CFile();

BEGIN_COM_MAP(CFile)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IPdnFile)
END_COM_MAP()

	HRESULT GetWriteAccess();

	STDMETHOD(Create)(BSTR strFileName, unsigned dwAccess, unsigned dwShare,
		unsigned dwDisposition);

	STDMETHOD(Seek)(int fpLow, int fpHigh, unsigned dwMethod);
	STDMETHOD(SetEOF)();

	STDMETHOD(Read)(BYTE* pBuffer, unsigned nRead);
	STDMETHOD(Write)(BYTE* pBuffer, unsigned nWrite);

	STDMETHOD(ReadLine)(BSTR* strLine);
	STDMETHOD(WriteLine)(BSTR strLine);

	STDMETHOD(ReadBase64)(BSTR* strBase64);
	STDMETHOD(WriteBase64)(BSTR strBase64);

	STDMETHOD(get_AtEnd)(BOOL* bAtEnd);
	STDMETHOD(get_Size)(unsigned* nSize);
	STDMETHOD(get_FileName)(BSTR* strFileName);
	STDMETHOD(get_SHA1)(BSTR* strSHA1);

	STDMETHOD(Move)(BSTR strTo);
	STDMETHOD(Copy)(BSTR strTo, BOOL bOverWrite, IPdnFile** retCopy);
	STDMETHOD(Delete)();
	STDMETHOD(Flush)();
	STDMETHOD(Close)();

	STDMETHOD(get_LastWriteTime)(VARIANT* vLastWriteTime);

private:
	void fixSlashes(_bstr_t &str);
	_bstr_t getTempFileName();
	void createFileDirectory(_bstr_t &str);
};
