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
	template<class T>
	static inline std::wstring Base64Encode(
		const T* data, unsigned int dataLength, bool addCRLF = true)
	{
		static const wchar_t alphabet[] =
			L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"
			L"ghijklmnopqrstuvwxyz0123456789+/";

		std::wstring base64data;
		unsigned int i = 0;

		while(dataLength - i > 2)
		{
			base64data += alphabet[(data[i] >> 2) & 63];
			base64data += alphabet[(((data[i] & 3) << 4) | (data[i+1] >> 4) & 15) & 63];
			base64data += alphabet[(((data[i+1] & 15) << 2) | (data[i+2] >> 6) & 3) & 63];
			base64data += alphabet[data[i+2] & 63];
			if(addCRLF && (i / 3 * 4) % 72 == 0)
			{
				base64data += L"\r\n";
			}
			i += 3;
		}
		if(dataLength - i == 2)
		{
			base64data += alphabet[(data[i] >> 2) & 63];
			base64data += alphabet[(((data[i] & 3) << 4) | (data[i+1] >> 4) & 15) & 63];
			base64data += alphabet[((data[i+1] & 15) << 2)];
			base64data += L'=';
			i += 3;
		}
		else if(dataLength - i == 1)
		{
			base64data += alphabet[(data[i] >> 2) & 63];
			base64data += alphabet[((data[i] & 3) << 4)];
			base64data += L'=';
			base64data += L'=';
			i += 3;
		}
		if(addCRLF && (i / 3 * 4) % 72 == 0)
		{
			base64data += L"\r\n";
		}

		return base64data;
	}

	static inline std::vector<BYTE> Base64Decode(const std::wstring base64Data)
	{
		static const unsigned char d[] = 
			{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
			 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
			 -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
			 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
			 -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
			 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1};

		std::vector<BYTE> decodedData(base64Data.length() / 4 * 3);
		unsigned int i = 0, j = 0;

		while(i < base64Data.length())
		{
			decodedData[j] = (unsigned char)
				(d[base64Data[i]] << 2 | d[base64Data[i+1]] >> 4);
			decodedData[j+1] = (unsigned char)
				(d[base64Data[i+1]] << 4 | d[base64Data[i+2]] >> 2);
			decodedData[j+2] = (unsigned char)
				(((d[base64Data[i+2]] << 6)) | d[base64Data[i+3]] >> 4);

			i += 4;
			j += 3;

			if(base64Data.length() - i >= 2 &&
				base64Data[i] == '\r' && base64Data[i+1] == '\n')
			{
				i += 2;
			}
			if(base64Data.length() - i >= 1 && base64Data[i] == '\n')
			{
				i += 1;
			}

		}

		decodedData.resize(j);

		return decodedData;
	}
};