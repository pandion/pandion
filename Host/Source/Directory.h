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
 * Filename:    Directory.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#pragma once

class CListEntry :
	public CComObjectRootEx< CComSingleThreadModel >,
	public IDispatchImpl< IListEntry >
{
private:
	WIN32_FIND_DATAW m_Data;
public:
	CListEntry();
	~CListEntry();

BEGIN_COM_MAP(CListEntry)
	COM_INTERFACE_ENTRY( IDispatch )
	COM_INTERFACE_ENTRY( IListEntry )
END_COM_MAP()

	void SetData( WIN32_FIND_DATAW *pData );

	STDMETHOD(get_IsArchive)( BOOL *b );
	STDMETHOD(get_IsCompressed)( BOOL *b );
	STDMETHOD(get_IsDirectory)( BOOL *b );
	STDMETHOD(get_IsEncrypted)( BOOL *b );
	STDMETHOD(get_IsHidden)( BOOL *b );
	STDMETHOD(get_IsNormal)( BOOL *b );
	STDMETHOD(get_IsOffline)( BOOL *b );
	STDMETHOD(get_IsReadOnly)( BOOL *b );
	STDMETHOD(get_HasReparsePoint)( BOOL *b );
	STDMETHOD(get_IsSparseFile)( BOOL *b );
	STDMETHOD(get_IsSystem)( BOOL *b );
	STDMETHOD(get_IsTemporary)( BOOL *b );

	STDMETHOD(get_CreationTime)( BSTR *str );
	STDMETHOD(get_LastAccessTime)( BSTR *str );
	STDMETHOD(get_LastWriteTime)( BSTR *str );

	STDMETHOD(get_FileSizeHigh)( DWORD *dw );
	STDMETHOD(get_FileSizeLow)( DWORD *dw );

	STDMETHOD(get_ReparseTag)( DWORD *dw );
	STDMETHOD(get_Reserved1)( DWORD *dw );

	STDMETHOD(get_Name)( BSTR *str );
	STDMETHOD(get_AlternateName)( BSTR *str );
};

class CDirectory :
	public CComObjectRootEx< CComSingleThreadModel >,
	public IDispatchImpl< IPdnDirectory >
{
public:
	CDirectory();
	~CDirectory();

	void Clear( BSTR path );
	STDMETHOD(List)( BSTR path, DWORD dwFlag, VARIANT *list );
	int FindSlash( BSTR path );

BEGIN_COM_MAP(CDirectory)
	COM_INTERFACE_ENTRY( IDispatch )
	COM_INTERFACE_ENTRY( IPdnDirectory )
END_COM_MAP()

	STDMETHOD(Create)( BSTR path );
	STDMETHOD(Exists)( BSTR path, BOOL *bExists );
	STDMETHOD(ListDirs)( BSTR path, VARIANT *list ); 
	STDMETHOD(ListFiles)( BSTR path, VARIANT *list );
	STDMETHOD(Delete)( BSTR path );
};

