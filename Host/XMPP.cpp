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
 * Filename:    XMPP.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "XMPP.h"
#include "MainWnd.h"

/*
 * Constructor
 */
CXMPP::CXMPP() :
	m_Handlers(), m_Logger(), m_ConnectionManager(m_Handlers, m_Logger)
{
}

/*
 * Destructor
 */
CXMPP::~CXMPP()
{
}

/*
 *
 */
STDMETHODIMP CXMPP::SetMainWnd(void* pMainWnd)
{
	m_Handlers.SetMainWindow((CMainWnd*) pMainWnd);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::SetProxyServer(BSTR server, USHORT port, 
								   BSTR username, BSTR password)
{
	m_ConnectionManager.SetProxyServer(server, port, username, password);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::SetProxyPollURL(BSTR pollURL)
{
	m_ConnectionManager.SetProxyPollURL(std::wstring(pollURL));
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::get_ConnectionIP(BSTR *strIP)
{
	*strIP = ::SysAllocString(m_ConnectionManager.GetConnectionIP().c_str());
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::Connect(BSTR server,
	USHORT port, BOOL useSSL, DWORD proxyMethod)
{
	if(proxyMethod == 0)
	{
		m_ConnectionManager.Connect(server, port, useSSL, ProxyMethodDontUse);
	}
	else if(proxyMethod == 1)
	{
		m_ConnectionManager.Connect(server, port, useSSL, ProxyMethodConnect);
	}
	else if(proxyMethod == 3)
	{
		m_ConnectionManager.Connect(server, port, useSSL, ProxyMethodPoll);
	}
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::Disconnect()
{
	m_ConnectionManager.Disconnect();
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::StartTLS()
{
	m_ConnectionManager.StartTLS();
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::StartSC()
{
	m_ConnectionManager.StartSC();
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::SendXML(VARIANT pDisp)
{
	MSXML2::IXMLDOMDocument *pXMLDoc = NULL;
	BSTR xmlString;

	if(pDisp.vt == VT_DISPATCH && 
		pDisp.pdispVal != NULL &&
		SUCCEEDED(pDisp.pdispVal->QueryInterface(IID_IXMLDOMDocument,
			(LPVOID*) &pXMLDoc)) &&
		SUCCEEDED(pXMLDoc->get_xml(&xmlString)))
	{
		m_ConnectionManager.SendText(xmlString);

		::SysFreeString(xmlString);
		pXMLDoc->Release();
	}
	else
	{
		return E_FAIL;
	}
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::SendText(BSTR strText)
{
	m_ConnectionManager.SendText(std::wstring(strText));
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_ConnectedHandler(BSTR handler)
{
	m_Handlers.SetConnectedHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_DisconnectedHandler(BSTR handler)
{
	m_Handlers.SetDisconnectedHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_IQHandler(BSTR handler)
{
	m_Handlers.SetIQHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_MessageHandler(BSTR handler)
{
	m_Handlers.SetMessageHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_PresenceHandler(BSTR handler)
{
	m_Handlers.SetPresenceHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_DocumentStartHandler(BSTR handler)
{
	m_Handlers.SetDocumentStartHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_DocumentEndHandler(BSTR handler)
{
	m_Handlers.SetDocumentEndHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_StreamHandler(BSTR handler)
{
	m_Handlers.SetStreamHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_StartTLSSucceededHandler(BSTR handler)
{
	m_Handlers.SetStartTLSSucceededHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_StartTLSFailedHandler(BSTR handler)
{
	m_Handlers.SetStartTLSFailedHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_StartSCSucceededHandler(BSTR handler)
{
	m_Handlers.SetStartSCSucceededHandler(handler);
	return S_OK;
}

/*
 *
 */
STDMETHODIMP CXMPP::put_StartSCFailedHandler(BSTR handler)
{
	m_Handlers.SetStartSCFailedHandler(handler);
	return S_OK;
}
