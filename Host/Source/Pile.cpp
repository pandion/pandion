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
 * Filename:    Pile.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "Pile.h"
#include "Session.h"

CPile::CPile()
{
}
CPile::~CPile()
{
}
_bstr_t CPile::GetFileNameByURL( _bstr_t URL )
{
	for( DWORD i = 0; i < m_SharedFiles.size(); i++ )
	{
		BSTR myURL = m_SharedFiles[i]->m_URL;
		if( m_SharedFiles[i]->m_URL == URL )
		{
			return m_SharedFiles[i]->m_FileName;
		}
	}
	return "";
}
DWORD CPile::GetCookieByURL( _bstr_t URL )
{
	for( DWORD i = 0; i < m_SharedFiles.size(); i++ )
	{
		BSTR myURL = m_SharedFiles[i]->m_URL;
		if( m_SharedFiles[i]->m_URL == URL )
		{
			return m_SharedFiles[i]->GetCookie();
		}
	}
	return 0;
}
void CPile::AddSharedFile( CSharedFile* pSF )
{
	m_SharedFiles.push_back( pSF );
}
HRESULT CPile::DelSharedFile( DWORD sfCookie )
{
	for( DWORD i = 0; i < m_SharedFiles.size(); i++ )
	{
		if( m_SharedFiles[i]->GetCookie() == sfCookie )
		{
			delete m_SharedFiles[i];
			m_SharedFiles.erase( m_SharedFiles.begin()+i );
			return S_OK;
		}
	}
	return S_FALSE;
}
void CPile::AddSession( CSession* pS )
{
	m_Sessions.push_back( pS );
}
HRESULT CPile::DelSession(  DWORD sessionID )
{
	for( DWORD i = 0; i < m_Sessions.size(); i++ )
	{
		if( m_Sessions[i]->GetSessionID() == sessionID )
		{	
			HANDLE h = m_Sessions[i]->GetHandle();
			delete m_Sessions[i];
			::TerminateThread( h, 0 );
			m_Sessions.erase( m_Sessions.begin()+i );
			return S_OK;
		}
	}
	return S_FALSE;
}
CSession* CPile::GetSession( DWORD sessionID )
{
	CSession* retval = 0;
	for( DWORD i = 0; i < m_Sessions.size(); i++ )
		if( m_Sessions[i]->GetSessionID() == sessionID )
			retval = m_Sessions[i];
	return retval;
}