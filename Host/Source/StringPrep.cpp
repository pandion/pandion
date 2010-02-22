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
#include "StringPrepTables.h"

StringPrepException::StringPrepException(std::wstring text)
{
	this->text = text;
}

StringPrep::StringPrep()
{
	/* Load raw RFC3454 data into data structures */
	m_Aranges = GenerateRangeTable(RFC3454TableA1, sizeof(RFC3454TableA1));

	m_B1map =
		GenerateMappingTable(RFC3454TableB1, sizeof(RFC3454TableB1), 0xFFFFF);
	m_B2map =
		GenerateMappingTable(RFC3454TableB2, sizeof(RFC3454TableB2), 0xFFFFF);
	m_B3map =
		GenerateMappingTable(RFC3454TableB3, sizeof(RFC3454TableB3), 0xFFFFF);

	m_C11ranges = GenerateRangeTable(RFC3454TableC11, sizeof(RFC3454TableC11));
	m_C12ranges = GenerateRangeTable(RFC3454TableC12, sizeof(RFC3454TableC12));
	m_C21ranges = GenerateRangeTable(RFC3454TableC21, sizeof(RFC3454TableC21));
	m_C22ranges = GenerateRangeTable(RFC3454TableC22, sizeof(RFC3454TableC22));
	m_C3ranges  = GenerateRangeTable(RFC3454TableC3, sizeof(RFC3454TableC3));
	m_C4ranges  = GenerateRangeTable(RFC3454TableC4, sizeof(RFC3454TableC4));
	m_C5ranges  = GenerateRangeTable(RFC3454TableC5, sizeof(RFC3454TableC5));
	m_C6ranges  = GenerateRangeTable(RFC3454TableC6, sizeof(RFC3454TableC6));
	m_C7ranges  = GenerateRangeTable(RFC3454TableC7, sizeof(RFC3454TableC7));
	m_C8ranges  = GenerateRangeTable(RFC3454TableC8, sizeof(RFC3454TableC8));
	m_C9ranges  = GenerateRangeTable(RFC3454TableC9, sizeof(RFC3454TableC9));

	m_D1ranges  = GenerateRangeTable(RFC3454TableD1, sizeof(RFC3454TableD1));
	m_D2ranges  = GenerateRangeTable(RFC3454TableD2, sizeof(RFC3454TableD2));
}

StringPrep::~StringPrep()
{
}

UTF8String StringPrep::SASLPrep(UTF8String str)
{
	UTF32String str32 = UTF::utf8to32(str);

	std::vector<MappingTable> maps;
	maps.push_back(m_B1map);
	maps.push_back(RangeTableToMappingTable(m_C12ranges, UTF32String()));

	std::vector<RangeTable> prohibitRanges;
	prohibitRanges.push_back(m_C11ranges);
	prohibitRanges.push_back(m_C12ranges);
	prohibitRanges.push_back(m_C21ranges);
	prohibitRanges.push_back(m_C22ranges);
	prohibitRanges.push_back(m_C3ranges);
	prohibitRanges.push_back(m_C4ranges);
	prohibitRanges.push_back(m_C5ranges);
	prohibitRanges.push_back(m_C6ranges);
	prohibitRanges.push_back(m_C7ranges);
	prohibitRanges.push_back(m_C8ranges);
	prohibitRanges.push_back(m_C9ranges);

	UTF32String preparedString = 
		PrepareString(str32, true, true, true, maps, prohibitRanges);

	return UTF::utf32to8(preparedString);
}

UTF32String StringPrep::PrepareString(
	const UTF32String str,
	bool checkUnassigned,
	bool normalizeWithKC,
	bool checkBidi,
	std::vector<MappingTable> maps,
	std::vector<RangeTable> prohibitRanges)
{
	/* Filter away invalid Unicode 3.2 */
	UTF32String filteredString;
	if(checkUnassigned)
	{
		filteredString = Unicode3_2Filter(str);
	}
	else
	{
		filteredString = str;
	}

	/* Perform StringPrep mapping */
	UTF32String mappedString(filteredString);
	for(std::vector<MappingTable>::const_iterator i = maps.begin();
		i != maps.end(); i++)
	{
		mappedString = Map(mappedString, *i);
	}

	/* Normalize Unicode with KC */
	UTF32String normalizedString;
	if(normalizeWithKC)
	{
		normalizedString = NormalizeWithKC(filteredString);
	}
	else
	{
		normalizedString = filteredString;
	}

	/* Fail when string contains prohibited characters. */
	for(std::vector<RangeTable>::const_iterator i = prohibitRanges.begin();
		i != prohibitRanges.end(); i++)
	{
		if(IsProhibited(normalizedString, *i))
		{
			throw StringPrepException(
				L"String contains prohibited character.");
		}
	}

	/* Check bidirectionality */
	if(checkBidi && !IsBidiOK(normalizedString))
	{
		throw StringPrepException(
			L"String does not meet bidirectionality requirements.");
	}

	return normalizedString;
}

RangeTable StringPrep::GenerateRangeTable(
	const unsigned* rangeArray,
	unsigned rangeArraySize)
{
	typedef std::pair<unsigned, unsigned> range;
	RangeTable rt;
	for(unsigned i = 0; i < rangeArraySize; i += 2)
	{
		rt.push_back(range(rangeArray[i], rangeArray[i+1]));
	}
	return rt;
}

MappingTable StringPrep::GenerateMappingTable(
	const unsigned* mappingArray,
	unsigned mappingArraySize,
	unsigned separator)
{
	typedef std::pair<unsigned, UTF32String> pair;
	MappingTable m;
	unsigned i = 0;

	while(i < mappingArraySize)
	{
		unsigned key = mappingArray[i];
		i += 2;

		UTF32String value;
		for(;mappingArray[i] != separator && i < mappingArraySize; i++)
			value.push_back(mappingArray[i]);

		m.insert(pair(key, value));
	}
	return m;
}
MappingTable StringPrep::RangeTableToMappingTable(
	RangeTable rangeTable,
	UTF32String mapTo)
{
	typedef std::pair<unsigned, UTF32String> pair;
	MappingTable m;

	for(RangeTable::const_iterator i = rangeTable.begin();
		i != rangeTable.end(); i++)
	{
		for(unsigned j = i->first; j <= i->second; j++)
		{
			m.insert(pair(j,mapTo));
		}
	}
	return m;
}

bool StringPrep::IsCharacterInRange(unsigned c, RangeTable table)
{
	bool inTable = false;
	for(RangeTable::const_iterator i = table.begin(); i != table.end(); i++)
	{
		if(c <= i->second)
		{
			if(c >= i->first)
			{
				inTable = true;
			}
			break;
		}
	}
	return inTable;
}

UTF32String StringPrep::MapCharacter(unsigned c, MappingTable map)
{
	if(map.find(c) != map.end())
	{
		return map[c];
	}
	else
	{
		return UTF32String(1, c);
	}
}


UTF32String StringPrep::Unicode3_2Filter(const UTF32String str)
{
	UTF32String filteredString;
	RangeTable invalidUnicodeRanges = 
		GenerateRangeTable(RFC3454TableA1, sizeof(RFC3454TableA1));

	for(UTF32String::const_iterator i = str.begin(); i != str.end(); i++)
	{
		if(!IsCharacterInRange(*i, invalidUnicodeRanges))
		{
			filteredString.push_back(*i);
		}
	}

	return filteredString;
}

UTF32String StringPrep::Map(
	const UTF32String str,
	std::map<unsigned, UTF32String> stringPrepMap)
{
	UTF32String mappedString;

	for(unsigned i = 0; i != str.size(); i++)
	{
		mappedString += MapCharacter(str[i], stringPrepMap);
	}
	return mappedString;
}

UTF32String StringPrep::NormalizeWithKC(const UTF32String str)
{
	// TODO: implement this
	return str;
}

bool StringPrep::IsProhibited(const UTF32String str, RangeTable ranges)
{
	for(UTF32String::const_iterator i = str.begin(); i != str.end(); i++)
	{
		if(IsCharacterInRange(*i, ranges))
		{
			return true;
		}
	}
	return false;
}

bool StringPrep::IsBidiOK(const UTF32String str)
{
	/* Prohibit characters from table C.8 */
	RangeTable tableC8 = 
		GenerateRangeTable(RFC3454TableC8, sizeof(RFC3454TableC8));
	for(UTF32String::const_iterator i = str.begin(); i != str.end(); i++)
	{
		if(IsCharacterInRange(*i, tableC8))
		{
			return false;
		}
	}

	/* See if string contains RandALCat characters */
	bool containsRandALCat = false;
	RangeTable tableD1 =
		GenerateRangeTable(RFC3454TableD1, sizeof(RFC3454TableD1));
	for(UTF32String::const_iterator i = str.begin(); i != str.end(); i++)
	{
		if(IsCharacterInRange(*i, tableD1))
		{
			containsRandALCat = true;
		}
	}

	if(containsRandALCat)
	{
		/* RandALCat strings shouldn't contain LCat characters */
		RangeTable tableD2 =
			GenerateRangeTable(RFC3454TableD2, sizeof(RFC3454TableD2));
		for(UTF32String::const_iterator i = str.begin(); i != str.end(); i++)
		{
			if(IsCharacterInRange(*i, tableD2))
			{
				return false;
			}
		}

		/* First and last character of a RandALCat string should be a
		 * a RandALCat character */
		if( IsCharacterInRange(*(str.begin()), tableD2) &&
			IsCharacterInRange(*(str.end()-1), tableD2))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}