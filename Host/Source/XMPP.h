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
 * Description: Declaration of the XMPP class.
 */
#pragma once

#include "XMPPLogger.h"
#include "XMPPHandlers.h"
#include "XMPPConnectionManager.h"
#include "DispInterfaceImpl.h"

/*
 * The XMPP class implements the (COM) interface to the XMPP functionality. 
 */
class XMPP :
	public DispInterfaceImpl<IXMPP>
{
private:
	/*
	 * The XMPP event handler.
	 */
	XMPPHandlers			m_Handlers;
	/*
	 * The XMPP event logger.
	 */
	XMPPLogger				m_Logger;
	/*
	 * The XMPP connection manager.
	 */
	XMPPConnectionManager	m_ConnectionManager;
public:
	XMPP();
	~XMPP();

	/* IXMPP implementation */
	STDMETHOD(Connect)(BSTR server, USHORT port, BOOL useSSL = FALSE);
	STDMETHOD(Disconnect)();
	STDMETHOD(StartTLS)();
	STDMETHOD(StartSC)();

	STDMETHOD(SendXML)(IDispatch* pDisp);
	STDMETHOD(SendText)(BSTR strText);

	STDMETHOD(get_ConnectionIP)(BSTR *strIP);
	STDMETHOD(get_ConnectionFQDN)(BSTR *fqdn);

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

	STDMETHOD(get_ServerFQDN)(LPBSTR serverFQDN);

	/* Other methods */
	virtual void SetMainWnd(MainWnd* pMainWnd);
};
