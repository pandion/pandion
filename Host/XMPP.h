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
 * Filename:    XMPP.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#pragma once

#include "XMPPLogger.h"
#include "XMPPHandlers.h"
#include "XMPPConnectionManager.h"

class CXMPP :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IXMPP>
{
private:
	XMPPHandlers			m_Handlers;
	XMPPLogger				m_Logger;
	XMPPConnectionManager	m_ConnectionManager;
public:
	CXMPP();
	~CXMPP();

	/* IUnknown implementation */
BEGIN_COM_MAP(CXMPP)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IXMPP)
END_COM_MAP()

	/* IXMPP implementation */
	STDMETHOD(SetMainWnd)(void *pMainWnd);
	STDMETHOD(SetProxyServer)(BSTR server, 
		USHORT port, BSTR username, BSTR password);
	STDMETHOD(SetProxyPollURL)(BSTR pollURL);

	STDMETHOD(get_ConnectionIP)(BSTR *strIP);

	STDMETHOD(Connect)(BSTR server, 
		USHORT port, BOOL useSSL = FALSE, DWORD proxyMethod = 0);
	STDMETHOD(Disconnect)();
	STDMETHOD(StartTLS)();
	STDMETHOD(StartSC)();

	STDMETHOD(SendXML)(VARIANT pDisp);
	STDMETHOD(SendText)(BSTR strText);

	STDMETHOD(put_ConnectedHandler)(BSTR handler);
	STDMETHOD(put_DisconnectedHandler)(BSTR handler);
	STDMETHOD(put_IQHandler)(BSTR handler);
	STDMETHOD(put_MessageHandler)(BSTR handler);
	STDMETHOD(put_PresenceHandler)(BSTR handler);
	STDMETHOD(put_DocumentStartHandler)(BSTR handler);
	STDMETHOD(put_DocumentEndHandler)(BSTR handler);
	STDMETHOD(put_StreamHandler)(BSTR handler);
	STDMETHOD(put_StartTLSSucceededHandler)(BSTR handler);
	STDMETHOD(put_StartTLSFailedHandler)(BSTR handler);
	STDMETHOD(put_StartSCSucceededHandler)(BSTR handler);
	STDMETHOD(put_StartSCFailedHandler)(BSTR handler);
};
