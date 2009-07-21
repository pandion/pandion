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
 * Filename:    XMPPSendQueue.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This file implements the send queue for the XMPP connection.
 *              Data to be sent is stored in an internal queue and sent by a 
 *              separate thread also managed by this class.
 */

#include "stdafx.h"
#include "XMPPSendQueue.h"
#include "Socket.h"

/*
 * This constructor initializes the XMPPSendQueue as disconnected with an empty
 * queue. The main thread and the event are initialized.
 */
XMPPSendQueue::XMPPSendQueue()
{
	m_bConnected = false;
	m_hDataAvailable = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	DWORD ThreadID;
	m_hThread = ::CreateThread(NULL, NULL, SendQueueProc, this, 0, &ThreadID);
}

/*
 * The destructor terminates the sending thread and closes the thread and
 * event handles.
 */
XMPPSendQueue::~XMPPSendQueue()
{
	::TerminateThread(m_hThread, 0);
	::CloseHandle(m_hThread);
	::CloseHandle(m_hDataAvailable);
}

/*
 * This method connects the queue to a given socket. All data added to the
 * queue after calling this method will be sent over the specified socket.
 */
void XMPPSendQueue::SetConnected(Socket* socket)
{
	while(!m_Buffers.empty())
	{
		m_Buffers.pop();
	}
	m_bConnected = true;
	m_Socket = socket;
}

/*
 * This method disconnects the queue from a given socket. After calling this
 * method no new data can be successfully added to the queue and no data will
 * be sent.
 */
void XMPPSendQueue::SetDisconnected()
{
	m_bConnected = false;
}

/*
 * This method adds the given data to the send queue where it waits for
 * transmission over the network. Data can only be added to the queue when
 * the queue is connected by calling SetConnected(). After adding the data
 * to the queue an event is set so the send queue thread can wake up to
 * transmit the data.
 */
void XMPPSendQueue::AddData(BYTE *dataBuffer, DWORD dataBufferSize)
{
	if(m_bConnected)
	{
		m_Buffers.push(std::vector<BYTE>(dataBuffer, 
			dataBuffer + dataBufferSize));
		::SetEvent(m_hDataAvailable);
	}
}

/*
 * This windows callback procedure is called by windows as an entry point to
 * the send queue thread. The this pointer is given as a user parameter and
 * used to call WaitForData().
 */
DWORD __stdcall XMPPSendQueue::SendQueueProc(void* pThis)
{
	((XMPPSendQueue *)pThis)->WaitForData();
	return 0;
}
/*
 * This is the main loop for the send queue thread. We wait for the event 
 * signalling new data in the queue and then attempt to send it when we're
 * connected.
 */
void XMPPSendQueue::WaitForData()
{
	while(::WaitForSingleObject(m_hDataAvailable, INFINITE) == WAIT_OBJECT_0)
	{
		if(m_bConnected && !m_Buffers.empty())
		{
			if(!m_Buffers.front().empty())
			{
				m_Socket->Send(&(m_Buffers.front())[0], m_Buffers.front().size());
			}
			
			m_Buffers.pop();

			if(m_Buffers.empty())
			{
				::ResetEvent(m_hDataAvailable);
			}
		}
		else // not connected or the buffer is empty
		{
			::ResetEvent(m_hDataAvailable);
		}
	}
}