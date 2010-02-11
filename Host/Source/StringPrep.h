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

class StringPrepException
{
	std::wstring text;
public:
	StringPrepException(std::wstring text);
};

class StringPrep
{
public:
	StringPrep();
	~StringPrep();

	static std::wstring PrepareString(
		const std::wstring str) throw(StringPrepException);

private:
	static std::vector<unsigned> StringPrep::UTF16toUTF32(
		const std::wstring UTF16String);
	static std::map<unsigned,std::vector<unsigned>> GetStringPrepMap();

	static std::vector<unsigned> Map(const std::vector<unsigned> str)
		throw(StringPrepException);
	static std::vector<unsigned> Normalize(const std::vector<unsigned> str)
		throw(StringPrepException);
	static std::vector<unsigned> Prohibit(const std::vector<unsigned> str)
		throw(StringPrepException);
	static std::vector<unsigned> CheckBiDi(const std::vector<unsigned> str)
		throw(StringPrepException);
};