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
 * Filename:    HTTPDownload.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#include "stdafx.h"
#include "HTTPDownload.h"
#include "HTTPEventRouter.h"

HTTPDownload::HTTPDownload(HINTERNET hInternet, HTTPEventRouter* eventRouter,
	DWORD sessionID) :
	m_hInternet(hInternet), m_Progress(0), m_Length(-1), m_Abort(false),
	m_EventRouter(eventRouter), m_SessionID(sessionID)
{
}
HTTPDownload::~HTTPDownload()
{
}
void HTTPDownload::Get(
	std::wstring filename,
	std::wstring URI,
	DWORD offset,
	DWORD len,
	std::wstring address,
	USHORT port)
{
	m_URI = URI;
	m_FileName = filename;
	m_Address = address;
	m_Port = port;

	DWORD threadID;
	::CreateThread(NULL, 0, DownloadFileProc, (LPVOID) this, 0, &threadID);
}
void HTTPDownload::Abort()
{
	m_Abort = true;
}
DWORD HTTPDownload::GetProgress()
{
	return m_Progress;
}
DWORD HTTPDownload::GetLength()
{
	return m_Length;
}
DWORD __stdcall HTTPDownload::DownloadFileProc(void *pThis)
{
	return ((HTTPDownload*) pThis)->DownloadFile();
}
DWORD HTTPDownload::DownloadFile()
{
	HANDLE hLocalFile = ::CreateFile(m_FileName.c_str(), GENERIC_WRITE,
		FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hLocalFile)
	{
		HINTERNET hRemoteFile = ::InternetConnect(m_hInternet,
			m_Address.c_str(), m_Port, L"", L"", INTERNET_SERVICE_HTTP, 0,
			NULL);
		if(hRemoteFile)
		{
			HINTERNET hRequest = ::HttpOpenRequest(hRemoteFile, NULL,
				m_URI.c_str(), NULL, NULL, NULL, INTERNET_FLAG_RELOAD,
				NULL);
			if(hRequest)
			{
				wchar_t contentLength[32];
				DWORD contentLengthSize = sizeof(contentLength), index = 0;
				BOOL success = ::HttpSendRequest(hRequest, NULL, 0, NULL, 0);
				success = ::HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH,
					&contentLength, &contentLengthSize, &index);
				m_Length = ::_wtoi(contentLength);

				if(success)
				{
					m_EventRouter->OnConnected(m_SessionID,
						_bstr_t(m_Address.c_str()),	m_Port, 0);
					DWORD bytesRead = 0, bytesWritten = 0;
					BYTE buffer[4096];
					
					while(!m_Abort)
					{
						success = ::InternetReadFile(hRequest, buffer,
							sizeof(buffer), &bytesRead);
						if(!success)
						{
							m_EventRouter->OnSockErr(m_SessionID,
								::GetLastError());
							m_Abort = true;
							break;
						}
						else if(bytesRead == 0)
						{
							m_EventRouter->OnTransferComplete(m_SessionID);
							break;
						}
						else
						{
							::WriteFile(hLocalFile, buffer, bytesRead,
								&bytesWritten, NULL);
							m_Progress += bytesWritten;
						}
					}
				}
				::InternetCloseHandle(hRequest);
			}
			::InternetCloseHandle(hRemoteFile);
		}
		::CloseHandle(hLocalFile);
	}

	if(m_Abort)
	{
		m_EventRouter->OnTransferAborted(m_SessionID);
	}
	else
	{
	}

	return 0;
}
