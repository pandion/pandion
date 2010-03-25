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
 * Filename:    XMPPSocketCompressor.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This file defines a class that provides stream compression
 *              functionality.
 */
#include "stdafx.h"
#include "XMPPSocketCompressor.h"

/*
 * Constructor
 */
XMPPSocketCompressor::XMPPSocketCompressor()
{
	Reset();
}

/*
 * Destructor
 */
XMPPSocketCompressor::~XMPPSocketCompressor()
{
}

/*
 * Resets all fields.
 */
void XMPPSocketCompressor::Reset()
{
	m_CanDecompressMore = false;
	m_PendingCompressed.clear();
	::ZeroMemory(&m_CompressionStream, sizeof(m_CompressionStream));
	::ZeroMemory(&m_DecompressionStream, sizeof(m_DecompressionStream));
}

/*
 * This method is called when the XMPPSocket wants to notify this object that a
 * connection has been established.
 */
void XMPPSocketCompressor::OnConnected()
{
	Reset();
	deflateInit(&m_CompressionStream, 9);
	inflateInit(&m_DecompressionStream);
}

/*
 * This method is called when the XMPPSocket wants to notify this object that it's
 * connection was terminated.
 */
void XMPPSocketCompressor::OnDisconnected()
{
	Reset();
	deflateEnd(&m_CompressionStream);
	inflateEnd(&m_DecompressionStream);
}

/*
 * This method compresses the given data.
 */
std::vector<BYTE> XMPPSocketCompressor::Compress(std::vector<BYTE>& data)
{
	std::vector<BYTE> compressedData(data.size() + data.size() / 1000 + 12);

	m_CompressionStream.next_in = &data[0];
	m_CompressionStream.avail_in = data.size();
	m_CompressionStream.next_out = &compressedData[0];
	m_CompressionStream.avail_out = compressedData.size();
	deflate(&m_CompressionStream, Z_SYNC_FLUSH);

	compressedData.resize(compressedData.size() - m_CompressionStream.avail_out);
	return compressedData;
}

/*
 * This method decompresses the given data.
 */
std::vector<BYTE> XMPPSocketCompressor::Decompress(std::vector<BYTE>& data)
{
	std::vector<BYTE> decompressedData(0x1000);

	m_PendingCompressed.insert(m_PendingCompressed.end(), data.begin(), data.end());
	if(m_PendingCompressed.size() == 0)
	{
		return data;
	}

	m_DecompressionStream.avail_in = m_PendingCompressed.size();
	m_DecompressionStream.next_in = &m_PendingCompressed[0];
	m_DecompressionStream.avail_out = decompressedData.size();
	m_DecompressionStream.next_out = &decompressedData[0];

	inflate(&m_DecompressionStream, Z_SYNC_FLUSH);

	if(m_PendingCompressed.size() != m_DecompressionStream.avail_in)
	{
		decompressedData.resize(decompressedData.size() - 
			m_DecompressionStream.avail_out);
		if(m_DecompressionStream.avail_in == 0)
		{
			m_PendingCompressed.clear();
			m_CanDecompressMore = false;
		}
		else
		{
			m_PendingCompressed.erase(m_PendingCompressed.begin(),
				m_PendingCompressed.end() - m_DecompressionStream.avail_in);
			m_CanDecompressMore = true;
		}
	}
	else
	{
		decompressedData.clear();
		m_CanDecompressMore = false;
		/* nothing was decompressed, queue up some more bytes */
	}

	return decompressedData;
}

/*
 * This method returns true when there is still data pending to be read.
 */
bool XMPPSocketCompressor::Select(bool read, bool write)
{
	return read && m_CanDecompressMore;
}