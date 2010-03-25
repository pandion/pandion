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
 * Filename:    XMPPSendQueue.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This file declares the send queue for the XMPP connection.
 *              Data to be sent is stored in an internal queue and sent by a 
 *              separate thread also managed by this class.
 */

#pragma once

class XMPPSocket; // forward declaration

class XMPPSendQueue
{
	std::queue<std::vector<BYTE>>	m_Buffers;
	bool							m_bConnected;
	XMPPSocket*						m_Socket;
	HANDLE							m_hDataAvailable;
	HANDLE							m_hThread;
public:
	XMPPSendQueue();
	~XMPPSendQueue();

	void SetConnected(XMPPSocket* socket);
	void SetDisconnected();
	void AddData(BYTE *dataBuffer, DWORD dataBufferSize);
private:
	DWORD SendData();
	static DWORD __stdcall SendQueueProc(void* pThis);
	void XMPPSendQueue::WaitForData();
};