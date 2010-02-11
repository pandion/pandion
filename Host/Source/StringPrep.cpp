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

std::wstring StringPrep::PrepareString(const std::wstring UTF16String)
{
/*	std::vector<unsigned> UTF32String = UTF16ToUTF32(UTF16String);
	return CheckBiDi(Prohibit(Normalize(Map(UTF32String))));
*/
	return UTF16String;
}

std::map<unsigned, std::vector<unsigned>> StringPrep::GetStringPrepMap()
{
	typedef std::pair<unsigned, std::vector<unsigned>> pair;
	std::map<unsigned, std::vector<unsigned>> m;
	unsigned i = 0;

	while(i < sizeof(RFC3454AppendixB))
	{
		unsigned key = RFC3454AppendixB[i];
		i += 2;

		std::vector<unsigned> value;
		for(;RFC3454AppendixB[i] != 0; i++)
			value.push_back(RFC3454AppendixB[i]);

		m.insert(pair(key, value));
	}
	return m;
}

std::vector<unsigned> StringPrep::Map(const std::vector<unsigned> str)
{
	std::vector<unsigned> mappedString;

	std::map<unsigned, std::vector<unsigned>> stringPrepMap =
		GetStringPrepMap();

	for(unsigned i = 0; i != str.size(); i++)
	{
		if(stringPrepMap.find(str[i]) != stringPrepMap.end())
		{
//			mappedString += stringPrepMap[str[i]];
		}
		else
		{
//			mappedString += str[i];
		}
	}
	return mappedString;
}

std::vector<unsigned> StringPrep::Normalize(const std::vector<unsigned> str)
{
	return std::vector<unsigned>();
}

std::vector<unsigned> StringPrep::Prohibit(const std::vector<unsigned> str)
{
	return std::vector<unsigned>();
}

std::vector<unsigned> StringPrep::CheckBiDi(const std::vector<unsigned> str)
{
	return std::vector<unsigned>();
}