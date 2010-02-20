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
 * Filename:    StringPrep.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: This header defines the StringPrep algorithm as described in
 *              RFC 3454.
 *              See http://www.ietf.org/rfc/rfc3454.txt
 */

#pragma once

#include "UTF.h"

typedef std::vector<std::pair<unsigned,unsigned>> RangeTable;
typedef std::map<unsigned, UTF32String> MappingTable;

class StringPrepException
{
	std::wstring text;
public:
	StringPrepException(std::wstring text);
};

class StringPrep
{
	RangeTable m_Aranges;
	MappingTable m_B1map, m_B2map, m_B3map;
	RangeTable 
		m_C11ranges, m_C12ranges, m_C21ranges, m_C22ranges,
		m_C3ranges,  m_C4ranges,  m_C5ranges,  m_C6ranges,
		m_C7ranges,  m_C8ranges,  m_C9ranges;
	RangeTable m_D1ranges, m_D2ranges;

public:
	StringPrep();
	~StringPrep();

	UTF8String SASLPrep(UTF8String str)
		throw(StringPrepException);

private:
	UTF32String PrepareString(
		const UTF32String str,
		bool checkUnassigned,
		bool normalizeWithKC,
		bool checkBiDi,
		std::vector<MappingTable> maps,
		std::vector<RangeTable> prohibitRanges)
			throw(StringPrepException);

	static RangeTable GenerateRangeTable(
		const unsigned* rangeArray,
		unsigned rangeArraySize);
	static MappingTable GenerateMappingTable(
		const unsigned* mappingArray,
		unsigned mappingArraySize,
		unsigned separator);
	static MappingTable RangeTableToMappingTable(
		RangeTable rangeTable,
		UTF32String mapTo);

	static bool IsCharacterInRange(unsigned c, RangeTable table);
	static UTF32String MapCharacter(unsigned c, MappingTable table);

	UTF32String Unicode3_2Filter(const UTF32String str);
	UTF32String Map(const UTF32String str, MappingTable map);
	UTF32String NormalizeWithKC(const UTF32String str);
	bool IsProhibited(const UTF32String str, RangeTable ranges);
	bool IsBidiOK(const UTF32String str);
};