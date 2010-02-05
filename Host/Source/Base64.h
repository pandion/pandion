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
 * Filename:    Base64.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This file contains a class template that contains two methods
 *              for decoding and encoding base64 strings.
 */

#pragma once

class Base64
{
public:
	template<class T>
	static std::wstring Encode(
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
			i += 3;
			if(addCRLF && ((i / 3 * 4) % 72 == 0))
			{
				base64data += L"\r\n";
			}
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
		if(addCRLF && ((i / 3 * 4) % 72 == 0))
		{
			base64data += L"\r\n";
		}

		return base64data;
	}

	static std::vector<unsigned char> Decode(const std::string base64Data)
	{
		static const unsigned char d[] = {
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, 62, -1, 63,
			52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
			-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
			15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
			-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
			41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, 63,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		};

		unsigned char s[4];
		std::vector<unsigned char> decodedData;
		
		for(unsigned i = 0, j = 0, padding = 0; i < base64Data.length(); i++)
		{
			if(d[base64Data[i]] != 0xFF)
			{
				s[j] = d[base64Data[i]];
				j++;
			}
			else if(base64Data[i] == '=')
			{
				padding++;
				j++;
			}
			if(j == 4)
			{
				if(padding == 0)
				{
					decodedData.push_back(s[0] << 2 | s[1] >> 4);
					decodedData.push_back(s[1] << 4 | s[2] >> 2);
					decodedData.push_back(s[2] << 6 | s[3]);
					j = 0;
				}
				else if(padding == 1)
				{
					decodedData.push_back(s[0] << 2 | s[1] >> 4);
					decodedData.push_back(s[1] << 4 | s[2] >> 2);
					break;
				}
				else if(padding == 2)
				{
					decodedData.push_back(s[0] << 2 | s[1] >> 4);
					break;
				}
				else
				{
					break;
				}
			}
		}
		return decodedData;
	}
};