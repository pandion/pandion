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
 * Filename:    XMPPHandlers.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#include "stdafx.h"
#include "XMPPHandlers.h"
#include "MainWnd.h"
XMPPHandlers::XMPPHandlers()
{
	m_pMainWnd = NULL;
}
XMPPHandlers::~XMPPHandlers()
{
	if(m_pMainWnd != NULL)
	{
		m_pMainWnd->Release();
	}
}


void XMPPHandlers::SetMainWindow(CMainWnd* mainWnd)
{
	m_pMainWnd = mainWnd;
	mainWnd->AddRef();
}

void XMPPHandlers::OnConnected()
{
	m_pMainWnd->Do(m_ConnectedHandler, &CComVariant(""));
}
void XMPPHandlers::OnDisconnected()
{
	m_pMainWnd->Do(m_DisconnectedHandler, &CComVariant(""));
}
void XMPPHandlers::OnStanza(MSXML2::IXMLDOMDocument *pXMLDoc, 
							const BSTR stanzaName)
{
	_bstr_t Handler;
	if(!wcscmp(stanzaName, L"iq"))
		Handler = m_IQHandler;
	else if(!wcscmp(stanzaName, L"message"))
		Handler = m_MessageHandler;
	else if(!wcscmp(stanzaName, L"presence"))
		Handler = m_PresenceHandler;
	else
		Handler = m_StreamHandler;

    m_pMainWnd->Do(Handler, &CComVariant(CComQIPtr<IDispatch>(pXMLDoc)));
}
void XMPPHandlers::OnDocumentStart(const _bstr_t& xmlFragment)
{
	m_pMainWnd->Do(m_DocumentStartHandler, &_variant_t(xmlFragment));
}
void XMPPHandlers::OnDocumentEnd(const _bstr_t& xmlFragment)
{
	m_pMainWnd->Do(m_DocumentEndHandler, &_variant_t(xmlFragment));
}
void XMPPHandlers::OnStartTLSSucceeded()
{
	m_pMainWnd->Do(m_StartTLSSucceededHandler, &CComVariant(""));
}
void XMPPHandlers::OnStartTLSFailed()
{
	m_pMainWnd->Do(m_StartTLSFailedHandler, &CComVariant(""));
}
void XMPPHandlers::OnStartSCSucceeded()
{
	m_pMainWnd->Do(m_StartSCSucceededHandler, &CComVariant(""));
}
void XMPPHandlers::OnStartSCFailed()
{
	m_pMainWnd->Do(m_StartSCFailedHandler, &CComVariant(""));
}

void XMPPHandlers::SetConnectedHandler(const BSTR handler)
{
	m_ConnectedHandler = handler;
}
void XMPPHandlers::SetDisconnectedHandler(const BSTR handler)
{
	m_DisconnectedHandler = handler;
}
void XMPPHandlers::SetIQHandler(const BSTR handler)
{
	m_IQHandler = handler;
}
void XMPPHandlers::SetMessageHandler(const BSTR handler)
{
	m_MessageHandler = handler;
}
void XMPPHandlers::SetPresenceHandler(const BSTR handler)
{
	m_PresenceHandler = handler;
}
void XMPPHandlers::SetDocumentStartHandler(const BSTR handler)
{
	m_DocumentStartHandler = handler;
}
void XMPPHandlers::SetDocumentEndHandler(const BSTR handler)
{
	m_DocumentEndHandler = handler;
}
void XMPPHandlers::SetStreamHandler(const BSTR handler)
{
	m_StreamHandler = handler;
}
void XMPPHandlers::SetStartTLSSucceededHandler(const BSTR handler)
{
	m_StartTLSSucceededHandler = handler;
}
void XMPPHandlers::SetStartTLSFailedHandler(const BSTR handler)
{
	m_StartTLSFailedHandler = handler;
}
void XMPPHandlers::SetStartSCSucceededHandler(const BSTR handler)
{
	m_StartSCSucceededHandler = handler;
}
void XMPPHandlers::SetStartSCFailedHandler(const BSTR handler)
{
	m_StartSCFailedHandler = handler;
}