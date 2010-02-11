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
 * Filename:    UTF.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: This file declares two functions that convert UTF-16 strings
 *              to and from UTF-32.
 */

#include "stdafx.h"
#include "UTF.h"

UTF16String UTF::utf8to16(const UTF8String fromString)
{
	UTF16String toString;

	UTF8String::const_iterator it = fromString.begin();
	while(it != fromString.end())
	{
		unsigned codepoint = getNextUTF8Codepoint(it, fromString.end());
		appendCodepointToUTF16String(toString, codepoint);
	}

	return toString;
}
UTF8String UTF::utf16to8(const UTF16String fromString)
{
	UTF8String toString;

	UTF16String::const_iterator it = fromString.begin();
	while(it != fromString.end())
	{
		unsigned codepoint = getNextUTF16Codepoint(it, fromString.end());
		appendCodepointToUTF8String(toString, codepoint);
	}

	return toString;
}
UTF32String UTF::utf8to32(const UTF8String fromString)
{
	UTF32String toString;

	UTF8String::const_iterator it = fromString.begin();
	while(it != fromString.end())
	{
		unsigned codepoint = getNextUTF8Codepoint(it, fromString.end());
		appendCodepointToUTF32String(toString, codepoint);
	}

	return toString;
}
UTF8String UTF::utf32to8(const UTF32String fromString)
{
	UTF8String toString;

	UTF32String::const_iterator it = fromString.begin();
	while(it != fromString.end())
	{
		unsigned codepoint = getNextUTF32Codepoint(it, fromString.end());
		appendCodepointToUTF8String(toString, codepoint);
	}

	return toString;
}
UTF32String UTF::utf16to32(const UTF16String fromString)
{
	UTF32String toString;

	UTF16String::const_iterator it = fromString.begin();
	while(it != fromString.end())
	{
		unsigned codepoint = getNextUTF16Codepoint(it, fromString.end());
		appendCodepointToUTF32String(toString, codepoint);
	}

	return toString;
}
UTF16String UTF::utf32to16(const UTF32String fromString)
{
	UTF16String toString;

	UTF32String::const_iterator it = fromString.begin();
	while(it != fromString.end())
	{
		unsigned codepoint = getNextUTF32Codepoint(it, fromString.end());
		appendCodepointToUTF16String(toString, codepoint);
	}

	return toString;
}


unsigned UTF::getNextUTF8Codepoint(
	UTF8String::const_iterator& from, UTF8String::const_iterator& end)
{
	unsigned codepoint = 0;

	if((*from & 0x80) == 0x00)
	{
		codepoint = *from;
		from += 1;
	}
	else if((from + 1) != end)
	{
		if( (*(from + 0) & 0xE0) == 0xC0 &&
			(*(from + 1) & 0xC0) == 0x80)
		{
			unsigned y = ((((unsigned) *(from + 0)) & 0x3C) >> 2);
			unsigned x = ((((unsigned) *(from + 0)) & 0x03) << 6) +
						 ((((unsigned) *(from + 1)) & 0x3F) >> 0);
			codepoint = (y << 8) + x;
			from += 2;
		}
		else if((from + 2) != end)
		{
			if( (*(from + 0) & 0xF0) == 0xE0 &&
				(*(from + 1) & 0xC0) == 0x80 &&
				(*(from + 2) & 0xC0) == 0x80)
			{
				unsigned y = ((((unsigned) *(from + 0)) & 0x0F) << 4) +
							 ((((unsigned) *(from + 1)) & 0x3C) >> 2);
				unsigned x = ((((unsigned) *(from + 1)) & 0x03) << 6) +
							 ((((unsigned) *(from + 2)) & 0x3F) >> 0);
				codepoint = (y << 8) + x;
				from += 3;
				if(codepoint >= 0xD800 && codepoint <= 0xDFFF)
				{
					codepoint = INVALID_CODEPOINT;
				}
			}
			else if((from + 3) != end)
			{
				if( (*(from + 0) & 0xF8) == 0xF0 &&
					(*(from + 1) & 0xC0) == 0x80 &&
					(*(from + 2) & 0xC0) == 0x80 &&
					(*(from + 3) & 0xC0) == 0x80)
				{
					unsigned z = ((((unsigned) *(from + 0)) & 0x07) << 2) +
								 ((((unsigned) *(from + 1)) & 0x30) >> 4);
					unsigned y = ((((unsigned) *(from + 1)) & 0x0F) << 4) +
								 ((((unsigned) *(from + 2)) & 0x3C) >> 2);
					unsigned x = ((((unsigned) *(from + 2)) & 0x03) << 6) +
								 ((((unsigned) *(from + 3)) & 0x3F) >> 0);
					codepoint = (z << 16) + (y << 8) + x;
					from += 4;
				}
				else
				{
					codepoint = INVALID_CODEPOINT;
					from += 1;
				}
			}
			else
			{
				codepoint = INVALID_CODEPOINT;
				from += 1;
			}
		}
		else
		{
			codepoint = INVALID_CODEPOINT;
			from += 1;
		}
	}
	else
	{
		codepoint = INVALID_CODEPOINT;
		from += 1;
	}

	return codepoint;
}
unsigned UTF::getNextUTF16Codepoint(
	UTF16String::const_iterator& from, UTF16String::const_iterator& end)
{
	unsigned codepoint = 0;
	if(*from == 0xFFFE || *from == 0xFFFF)
	{
		codepoint = INVALID_CODEPOINT;
		from += 1;
	}
	else if(*from < HI_SURROGATE_START || *from > HI_SURROGATE_END)
	{
		codepoint = *from;
		from += 1;
	}
	else 
	{
		if((from + 1) == end)
		{
			codepoint = INVALID_CODEPOINT;
			from += 1;
		}
		else if(*(from + 1) < LO_SURROGATE_START || *(from + 1) > LO_SURROGATE_END)
		{
			codepoint = INVALID_CODEPOINT;
			from += 1;
		}
		else
		{
			unsigned hi = ((unsigned) *(from + 0)) - HI_SURROGATE_START;
			unsigned lo = ((unsigned) *(from + 1)) - LO_SURROGATE_START;
			codepoint = lo + (hi << 10) + 0x10000;
			from += 2;
		}
	}
	return codepoint;
}
unsigned UTF::getNextUTF32Codepoint(
	UTF32String::const_iterator& from, UTF32String::const_iterator& end)
{
	unsigned codepoint = 0;
	if(from != end && isValidCodepoint(*from))
	{
		codepoint = *from;
		from += 1;
	}
	else
	{
		codepoint = INVALID_CODEPOINT;
		from += 1;
	}
	return codepoint;
}
void UTF::appendCodepointToUTF8String(UTF8String& toString, unsigned codepoint)
{
	if(isValidCodepoint(codepoint))
	{
		if(codepoint <= 0x00007F)
		{
			toString.push_back(codepoint);
		}
		else if(codepoint <= 0x0007FF)
		{
			toString.push_back(0xC0 + ((codepoint & 0x0007C0) >> 0x06));
			toString.push_back(0x80 + ((codepoint & 0x00003F) >> 0x00));
		}
		else if(codepoint <= 0x00FFFF)
		{
			toString.push_back(0xE0 + ((codepoint & 0x00F000) >> 0x0C));
			toString.push_back(0x80 + ((codepoint & 0x000FC0) >> 0x06));
			toString.push_back(0x80 + ((codepoint & 0x00003F) >> 0x00));
		}
		else
		{
			toString.push_back(0xF0 + ((codepoint & 0x1C0000) >> 0x12));
			toString.push_back(0x80 + ((codepoint & 0x03F000) >> 0x0C));
			toString.push_back(0x80 + ((codepoint & 0x000FC0) >> 0x06));
			toString.push_back(0x80 + ((codepoint & 0x00003F) >> 0x00));
		}
	}
	else
	{
		appendCodepointToUTF8String(toString, INVALID_CODEPOINT);
	}
}
void UTF::appendCodepointToUTF16String(UTF16String& toString, unsigned codepoint)
{
	if(isValidCodepoint(codepoint))
	{
		if(codepoint < 0x10000)
		{
			toString.push_back(codepoint);
		}
		else
		{
			unsigned surrogateLo = 
				((codepoint - 0x10000) & 0x003FF) >> 0x00;
			unsigned surrogateHi = 
				((codepoint - 0x10000) & 0xFFC00) >> 0x0A;
			toString.push_back(surrogateHi + HI_SURROGATE_START);
			toString.push_back(surrogateLo + LO_SURROGATE_START);
		}
	}
	else
	{
		appendCodepointToUTF16String(toString, INVALID_CODEPOINT);
	}
}
void UTF::appendCodepointToUTF32String(UTF32String& toString, unsigned codepoint)
{
	if(isValidCodepoint(codepoint))
	{
		toString.push_back(codepoint);
	}
	else
	{
		appendCodepointToUTF32String(toString, INVALID_CODEPOINT);
	}
}

bool UTF::isValidCodepoint(unsigned codepoint)
{
	return 
		(codepoint <= HI_SURROGATE_START || codepoint >= LO_SURROGATE_END) &&
		(codepoint <= 0xFDD0 || codepoint >= 0xFDEF) &&
		codepoint <= 0x10FFFF &&
		codepoint != 0xFFFE &&
		codepoint != 0xFFFF;
}