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
 * Filename:    XMPPHandlers.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#pragma once

class CMainWnd;

class XMPPHandlers
{
	_bstr_t m_ConnectedHandler;
	_bstr_t m_DisconnectedHandler;
	_bstr_t m_IQHandler;
	_bstr_t m_MessageHandler;
	_bstr_t m_PresenceHandler;
	_bstr_t m_DocumentStartHandler;
	_bstr_t m_DocumentEndHandler;
	_bstr_t m_StreamHandler;
	_bstr_t m_StartTLSSucceededHandler;
	_bstr_t m_StartTLSFailedHandler;
	_bstr_t m_StartSCSucceededHandler;
	_bstr_t m_StartSCFailedHandler;

	CMainWnd* m_pMainWnd;
public:
	XMPPHandlers();
	~XMPPHandlers();

	void SetMainWindow(CMainWnd* mainWindow);

	void OnConnected();
	void OnDisconnected();
	void OnStanza(MSXML2::IXMLDOMDocument *pXMLDoc, const BSTR stanzaName);
	void OnDocumentStart(const _bstr_t& xmlFragment);
	void OnDocumentEnd(const _bstr_t& xmlFragment);
	void OnStartTLSSucceeded();
	void OnStartTLSFailed();
	void OnStartSCSucceeded();
	void OnStartSCFailed();

	void SetConnectedHandler(const BSTR handler);
	void SetDisconnectedHandler(const BSTR handler);
	void SetIQHandler(const BSTR handler);
	void SetMessageHandler(const BSTR handler);
	void SetPresenceHandler(const BSTR handler);
	void SetDocumentStartHandler(const BSTR handler);
	void SetDocumentEndHandler(const BSTR handler);
	void SetStreamHandler(const BSTR handler);
	void SetStartTLSSucceededHandler(const BSTR handler);
	void SetStartTLSFailedHandler(const BSTR handler);
	void SetStartSCSucceededHandler(const BSTR handler);
	void SetStartSCFailedHandler(const BSTR handler);

};