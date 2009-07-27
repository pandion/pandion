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
 * Filename:    HTTPHTTPEventRouter.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#include "stdafx.h"

#include "PdnWnd.h"
#include "HTTPEventRouter.h"

HTTPEventRouter::HTTPEventRouter()
{ 
}
HTTPEventRouter::~HTTPEventRouter()
{
	UnsubscribeAll();
}

STDMETHODIMP HTTPEventRouter::Subscribe(IDispatch* pDisp)
{
	IPdnWnd* pdnWnd = NULL;
	pDisp->QueryInterface(__uuidof(IPdnWnd), (LPVOID*) &pdnWnd);
	m_Subscribers.push_back(pdnWnd);
	return S_OK;
}
STDMETHODIMP HTTPEventRouter::Unsubscribe(IDispatch* pDisp)
{
	if(!pDisp)
	{
		return UnsubscribeAll();
	}

	IPdnWnd* pdnWnd = NULL;
	pDisp->QueryInterface(__uuidof(IPdnWnd), (LPVOID*) &pdnWnd);
	std::vector<IPdnWnd*>::iterator it = m_Subscribers.begin();
	while(it != m_Subscribers.end())
	{
		if(*it == pdnWnd)
		{
			(*it)->Release();
			m_Subscribers.erase(it);
			pdnWnd->Release();
			return S_OK;
		}
		it++;
	}
	pdnWnd->Release();
	return S_FALSE;
}
STDMETHODIMP HTTPEventRouter::UnsubscribeAll()
{
	std::vector<IPdnWnd*>::iterator it = m_Subscribers.begin();
	while(it != m_Subscribers.end())
	{
		(*it)->Release();
		it++;
	}
	m_Subscribers.clear();
	return S_OK;
}

STDMETHODIMP HTTPEventRouter::OnListening(WORD Port)
{
	std::vector<IPdnWnd*>::iterator it = m_Subscribers.begin();
	while(it != m_Subscribers.end())
	{
		(*it)->Do(_bstr_t(L"HTTPOnListening"), &_variant_t(Port));
		it++;
	}
	return S_OK;
}
STDMETHODIMP HTTPEventRouter::OnAccept(DWORD sessionID, BSTR remHost, USHORT remPort, USHORT localPort)
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

	std::vector<IPdnWnd*>::iterator it = m_Subscribers.begin();
	while(it != m_Subscribers.end())
	{
		(*it)->Do(_bstr_t(L"HTTPOnAccept"), parameters, 4);
		it++;
	}
	return S_OK;
}
STDMETHODIMP HTTPEventRouter::OnConnected(DWORD sessionID, BSTR remHost, USHORT remPort, USHORT localPort)
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

	std::vector<IPdnWnd*>::iterator it = m_Subscribers.begin();
	while(it != m_Subscribers.end())
	{
		(*it)->Do(_bstr_t(L"HTTPOnConnected"), parameters, 4);
		it++;
	}
	return S_OK;
}
STDMETHODIMP HTTPEventRouter::OnGetRequest(DWORD sessionID, DWORD fileID, BSTR URI, BSTR localPath)
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

	std::vector<IPdnWnd*>::iterator it = m_Subscribers.begin();
	while(it != m_Subscribers.end())
	{
		(*it)->Do(_bstr_t(L"HTTPOnGetRequest"), parameters, 4);
		it++;
	}
	return S_OK;
}
STDMETHODIMP HTTPEventRouter::OnPostRequest(DWORD sessionID, BSTR URI, BSTR* saveAs)
{
	VARIANT parameters[4];
	parameters[0].vt       = VT_UI2;
	parameters[1].vt       = VT_BSTR | VT_BYREF;
	parameters[2].vt       = VT_UI4;
	parameters[0].pbstrVal = saveAs;
	parameters[1].bstrVal  = URI;
	parameters[2].uintVal  = sessionID;

	std::vector<IPdnWnd*>::iterator it = m_Subscribers.begin();
	while(it != m_Subscribers.end())
	{
		(*it)->Do(_bstr_t(L"HTTPOnPostRequest"), parameters, 4);
		it++;
	}
	return S_OK;
}
STDMETHODIMP HTTPEventRouter::OnTimeout(DWORD sessionID)
{
	VARIANT parameters[1];
	parameters[0].vt      = VT_UI4;
	parameters[0].uintVal = sessionID;

	std::vector<IPdnWnd*>::iterator it = m_Subscribers.begin();
	while(it != m_Subscribers.end())
	{
		(*it)->Do(_bstr_t(L"HTTPOnTimeout"), parameters, 1);
		it++;
	}
	return S_OK;
}
STDMETHODIMP HTTPEventRouter::OnSockErr(DWORD sessionID, DWORD err)
{
	VARIANT parameters[2];
	parameters[0].vt      = VT_UI4;
	parameters[1].vt      = VT_UI4;
	parameters[0].uintVal = err;
	parameters[1].uintVal = sessionID;

	std::vector<IPdnWnd*>::iterator it = m_Subscribers.begin();
	while(it != m_Subscribers.end())
	{
		(*it)->Do(_bstr_t(L"OnSockErr"), parameters, 2);
		it++;
	}
	return S_OK;
}
STDMETHODIMP HTTPEventRouter::OnHTTPErr(DWORD sessionID, DWORD err)
{
	VARIANT parameters[2];
	parameters[0].vt      = VT_UI4;
	parameters[1].vt      = VT_UI4;
	parameters[0].uintVal = err;
	parameters[1].uintVal = sessionID;

	std::vector<IPdnWnd*>::iterator it = m_Subscribers.begin();
	while(it != m_Subscribers.end())
	{
		(*it)->Do(_bstr_t(L"OnHTTPErr"), parameters, 2);
		it++;
	}
	return S_OK;
}
STDMETHODIMP HTTPEventRouter::OnFileErr(DWORD sessionID, DWORD err)
{
	VARIANT parameters[2];
	parameters[0].vt      = VT_UI4;
	parameters[1].vt      = VT_UI4;
	parameters[0].uintVal = err;
	parameters[1].uintVal = sessionID;

	std::vector<IPdnWnd*>::iterator it = m_Subscribers.begin();
	while(it != m_Subscribers.end())
	{
		(*it)->Do(_bstr_t(L"OnFileErr"), parameters, 2);
		it++;
	}
	return S_OK;
}

STDMETHODIMP HTTPEventRouter::OnTransferComplete(DWORD sessionID)
{
	VARIANT parameters[1];
	parameters[0].vt      = VT_UI4;
	parameters[0].uintVal = sessionID;

	std::vector<IPdnWnd*>::iterator it = m_Subscribers.begin();
	while(it != m_Subscribers.end())
	{
		(*it)->Do(_bstr_t(L"HTTPOnTransferComplete"), parameters, 1);
		it++;
	}
	return S_OK;
}
STDMETHODIMP HTTPEventRouter::OnTransferAborted(DWORD sessionID)
{
	VARIANT parameters[1];
	parameters[0].vt      = VT_UI4;
	parameters[0].uintVal = sessionID;

	std::vector<IPdnWnd*>::iterator it = m_Subscribers.begin();
	while(it != m_Subscribers.end())
	{
		(*it)->Do(_bstr_t(L"HTTPOnTransferAborted"), parameters, 1);
		it++;
	}
	return S_OK;
}