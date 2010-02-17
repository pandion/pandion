/**
 * This file is part of Pandion.
 *
 * Pandion is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pandion is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY); without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Pandion.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Filename:    StringPrep.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: This file implements the StringPrep algorithm as described in
 *              RFC 3454.
 *              See http://www.ietf.org/rfc/rfc3454.txt
 */

#include "stdafx.h"
#include "StringPrep.h"
#include "StringPrepProfiles.h"

StringPrepException::StringPrepException(std::wstring text)
{
	this->text = text;
}

UTF16String StringPrep::PrepareString(const UTF16String str)
{
	return UTF::utf32to16(
		CheckBiDi(Prohibit(Normalize(Map(UTF::utf16to32(str))))));
}

std::map<unsigned, UTF32String> StringPrep::GetStringPrepMap()
{
	typedef std::pair<unsigned, UTF32String> pair;
	std::map<unsigned, UTF32String> m;
	unsigned i = 0;

	while(i < sizeof(RFC3454AppendixB))
	{
		unsigned key = RFC3454AppendixB[i];
		i += 2;

		UTF32String value;
		for(;RFC3454AppendixB[i] != 0 && i < sizeof(RFC3454AppendixB); i++)
			value.push_back(RFC3454AppendixB[i]);

		m.insert(pair(key, value));
	}
	return m;
}

UTF32String StringPrep::Map(const UTF32String str)
{
	UTF32String mappedString;

	std::map<unsigned, UTF32String> stringPrepMap = GetStringPrepMap();

	for(unsigned i = 0; i != str.size(); i++)
	{
		if(stringPrepMap.find(str[i]) != stringPrepMap.end())
		{
			mappedString += stringPrepMap[str[i]];
		}
		else
		{
			mappedString += str[i];
		}
	}
	return mappedString;
}

UTF32String StringPrep::Normalize(const UTF32String str)
{
	return UTF32String();
}

UTF32String StringPrep::Prohibit(const UTF32String str)
{
	return UTF32String();
}

UTF32String StringPrep::CheckBiDi(const UTF32String str)
{
	return UTF32String();
}