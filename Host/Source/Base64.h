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
 * Filename:    External.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#pragma once

class Base64
{
public:
	static inline std::wstring Base64Encode(
		const wchar_t* data, unsigned int dataLength, bool addCRLF = true)
	{
		static const wchar_t alphabet[] =
			L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"
			L"ghijklmnopqrstuvwxyz0123456789+/";

		std::wstring base64data;
		unsigned int i = 0;

		while(dataLength - i > 2)
		{
			base64data += alphabet[data[i] >> 2];
			base64data += alphabet[((data[i] & 3) << 4) | (data[i+1] >> 4)];
			base64data += alphabet[((data[i+1] & 15) << 2) | (data[i+2] >> 6)];
			base64data += alphabet[data[i+2] & 63];
			if(addCRLF && (i / 3 * 4) % 72 == 0)
			{
				base64data += L"\r\n";
			}
			i += 3;
		}
		if(dataLength - i == 2)
		{
			base64data += alphabet[data[i] >> 2];
			base64data += alphabet[((data[i] & 3) << 4) | (data[i+1] >> 4)];
			base64data += alphabet[((data[i+1] & 15) << 2) | (data[i+2] >> 6)];
			base64data += L'=';
		}
		else if(dataLength - i == 1)
		{
			base64data += alphabet[data[i] >> 2];
			base64data += alphabet[((data[i] & 3) << 4) | (data[i+1] >> 4)];
			base64data += L'=';
			base64data += L'=';
		}
		if(addCRLF && (i / 3 * 4) % 72 == 0)
		{
			base64data += L"\r\n";
		}

		return base64data;
	}
};