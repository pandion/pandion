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
 * Filename:    XMPPSocketCompressor.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This file declares a class that provides stream compression
 *              functionality.
 */
#pragma once

/*
 * This class provides compression functionality for stream-oriented XMPPSockets.
 */
class XMPPSocketCompressor
{
private:
	/*
	 * These objects hold the state of the (de)compression stream.
	 */
	z_stream    m_CompressionStream;
	z_stream    m_DecompressionStream;

	/*
	 * Data that has yet to be decompressed.
	 */
	std::vector<BYTE> m_PendingCompressed;

	/*
	 * Flag that is set when there is more data available for decompression
	 */
	bool m_CanDecompressMore;
public:
	XMPPSocketCompressor();
	~XMPPSocketCompressor();
private:
	void Reset();
public:

	void OnConnected();
	void OnDisconnected();

	std::vector<BYTE> Compress(std::vector<BYTE>& data);
	std::vector<BYTE> Decompress(std::vector<BYTE>& data);

	bool Select(bool read, bool write);
};