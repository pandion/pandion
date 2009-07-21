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
 * Filename:    EventRouter.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "External.h"
#include "PdnWnd.h"
#include "EventRouter.h"

CEventRouter::CEventRouter()
{ 
}
CEventRouter::~CEventRouter()
{
	UnsubscribeAll();
}

STDMETHODIMP CEventRouter::Subscribe( IDispatch* pDisp )
{
	IPdnWnd* pWnd = 0;
	HRESULT hr = pDisp->QueryInterface( IID_IPdnWnd, (void**)&pWnd );
	if( !pWnd )	return hr;
	m_pTargetWnds.push_back( pWnd );
	return hr;
}
STDMETHODIMP CEventRouter::Unsubscribe( IDispatch* pDisp )
{
	if( !pDisp )
		return UnsubscribeAll();
	IUnknown* pUnk = 0;
	IUnknown* pUnk2 = 0;
	pDisp->QueryInterface( IID_IUnknown, (void**)&pUnk );

	for( DWORD i = 0; i < m_pTargetWnds.size(); i++ )
	{
		m_pTargetWnds[i]->QueryInterface( IID_IUnknown, (void**)&pUnk2 );
		if( pUnk == pUnk2 )
		{
			m_pTargetWnds[i]->Release();
			m_pTargetWnds.erase( m_pTargetWnds.begin()+i );
			pUnk->Release();
			pUnk2->Release();
			return S_OK;
		}
	}
	return S_FALSE;
}
STDMETHODIMP CEventRouter::UnsubscribeAll()
{
/*	std::vector<IPdnWnd*>::iterator i = m_pTargetWnds.begin();

	while(i != m_pTargetWnds.end())
	{
		(*i)->Release();
		i++;
	}
	m_pTargetWnds.clear();
*/
	return S_OK;
}

STDMETHODIMP CEventRouter::OnListening( WORD Port )
{
	for( DWORD i = 0; i < m_pTargetWnds.size(); i++ )
		m_pTargetWnds[i]->Do( CComBSTR( L"HTTPOnListening" ), &CComVariant( Port ) );
	return S_OK;
}
STDMETHODIMP CEventRouter::OnAccept( DWORD sessionID, BSTR remHost, USHORT remPort, USHORT localPort )
{
	VARIANT parameters[4];
	parameters[0].vt      = VT_UI2;
	parameters[1].vt      = VT_UI2;
	parameters[2].vt      = VT_BSTR;
	parameters[3].vt      = VT_UI4;
	parameters[0].uintVal = localPort;
	parameters[1].uintVal = remPort;
	parameters[2].bstrVal = remHost;
	parameters[3].uintVal = sessionID;

	for( DWORD i = 0; i < m_pTargetWnds.size(); i++ )
		m_pTargetWnds[i]->Do( CComBSTR( L"HTTPOnAccept" ), parameters, 4 );
	return S_OK;
}
STDMETHODIMP CEventRouter::OnConnected( DWORD sessionID, BSTR remHost, USHORT remPort, USHORT localPort )
{
	VARIANT parameters[4];
	parameters[0].vt      = VT_UI2;
	parameters[1].vt      = VT_UI2;
	parameters[2].vt      = VT_BSTR;
	parameters[3].vt      = VT_UI4;
	parameters[0].uintVal = localPort;
	parameters[1].uintVal = remPort;
	parameters[2].bstrVal = remHost;
	parameters[3].uintVal = sessionID;

	for( DWORD i = 0; i < m_pTargetWnds.size(); i++ )
		m_pTargetWnds[i]->Do( CComBSTR( L"HTTPOnConnected" ), parameters, 4 );
	return S_OK;
}

STDMETHODIMP CEventRouter::OnGetRequest( DWORD sessionID, DWORD fileID, BSTR URI, BSTR localPath )
{
	VARIANT parameters[4];
	parameters[0].vt      = VT_BSTR;
	parameters[1].vt      = VT_BSTR;
	parameters[2].vt      = VT_UI4;
	parameters[3].vt      = VT_UI4;
	parameters[0].bstrVal = localPath;
	parameters[1].bstrVal = URI;
	parameters[2].uintVal = fileID;
	parameters[3].uintVal = sessionID;

	for( DWORD i = 0; i < m_pTargetWnds.size(); i++ )
		m_pTargetWnds[i]->Do( CComBSTR( L"HTTPOnGetRequest" ), parameters, 4 );
	return S_OK;
}
STDMETHODIMP CEventRouter::OnPostRequest( DWORD sessionID, BSTR URI, BSTR* saveAs )
{
	VARIANT parameters[4];
	parameters[0].vt       = VT_UI2;
	parameters[1].vt       = VT_BSTR | VT_BYREF;
	parameters[2].vt       = VT_UI4;
	parameters[0].pbstrVal = saveAs;
	parameters[1].bstrVal  = URI;
	parameters[2].uintVal  = sessionID;

	for( DWORD i = 0; i < m_pTargetWnds.size(); i++ )
		m_pTargetWnds[i]->Do( CComBSTR( L"HTTPOnPostRequest" ), parameters, 3 );
	return S_OK;
}

STDMETHODIMP CEventRouter::OnTimeout( DWORD sessionID )
{
	VARIANT parameters[1];
	parameters[0].vt      = VT_UI4;
	parameters[0].uintVal = sessionID;

	for( DWORD i = 0; i < m_pTargetWnds.size(); i++ )
		m_pTargetWnds[i]->Do( CComBSTR( L"HTTPOnTimeout" ), parameters, 1 );
	return S_OK;
}
STDMETHODIMP CEventRouter::OnSockErr( DWORD sessionID, DWORD err )
{
	VARIANT parameters[2];
	parameters[0].vt      = VT_UI4;
	parameters[1].vt      = VT_UI4;
	parameters[0].uintVal = err;
	parameters[1].uintVal = sessionID;

	for( DWORD i = 0; i < m_pTargetWnds.size(); i++ )
		m_pTargetWnds[i]->Do( CComBSTR( L"HTTPOnSockErr" ), parameters, 2 );
	return S_OK;
}
STDMETHODIMP CEventRouter::OnHTTPErr( DWORD sessionID, DWORD err )
{
	VARIANT parameters[2];
	parameters[0].vt      = VT_UI4;
	parameters[1].vt      = VT_UI4;
	parameters[0].uintVal = err;
	parameters[1].uintVal = sessionID;

	for( DWORD i = 0; i < m_pTargetWnds.size(); i++ )
		m_pTargetWnds[i]->Do( CComBSTR( L"HTTPOnHTTPError" ), parameters, 2 );
	return S_OK;
}
STDMETHODIMP CEventRouter::OnFileErr( DWORD sessionID, DWORD err )
{
	VARIANT parameters[2];
	parameters[0].vt      = VT_UI4;
	parameters[1].vt      = VT_UI4;
	parameters[0].uintVal = err;
	parameters[1].uintVal = sessionID;

	for( DWORD i = 0; i < m_pTargetWnds.size(); i++ )
		m_pTargetWnds[i]->Do( CComBSTR( L"HTTPOnFileErr" ), parameters, 2 );
	return S_OK;
}

STDMETHODIMP CEventRouter::OnTransferComplete( DWORD sessionID )
{
	VARIANT parameters[1];
	parameters[0].vt      = VT_UI4;
	parameters[0].uintVal = sessionID;

	for( DWORD i = 0; i < m_pTargetWnds.size(); i++ )
		m_pTargetWnds[i]->Do( CComBSTR( L"HTTPOnTransferComplete" ), parameters, 1 );
	return S_OK;
}
STDMETHODIMP CEventRouter::OnTransferAborted( DWORD sessionID )
{
	VARIANT parameters[1];
	parameters[0].vt      = VT_UI4;
	parameters[0].uintVal = sessionID;

	for( DWORD i = 0; i < m_pTargetWnds.size(); i++ )
		m_pTargetWnds[i]->Do( CComBSTR( L"HTTPOnTransferAborted" ), parameters, 1 );
	return S_OK;
}