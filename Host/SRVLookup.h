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
 * Filename:    SRVLookup.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This file declares a class that looks up SRV records, orders
 *              them according to the SRV specification and can supply the
 *              records to agents.
 *              See http://www.ietf.org/rfc/rfc2782.txt for additional info
 *              about SRV.
 */
#pragma once

/*
 * This helper class holds an SRV record.
 */
class SRVRecord
{
private:
	_bstr_t targetName;
	WORD port;
	WORD priority;
	WORD weight;
public:
	SRVRecord(_bstr_t targetName, WORD port, WORD priority, WORD weight);
	~SRVRecord();
	_bstr_t getTargetName();
	WORD getPort();
	WORD getPriority();
	WORD getWeight();
};

/*
 * This class does the SRV lookup and can supply an iterator to the agent that
 * iterates through the records.
 */
class SRVLookup
{
private:
	_bstr_t m_srvname;
	std::vector<SRVRecord> m_records;
public:
	SRVLookup(_bstr_t service, _bstr_t protocol, _bstr_t domain);
	~SRVLookup();

	HRESULT DoLookup();

	std::vector<SRVRecord>::iterator getRecordsIterator();
private:
	HRESULT SortAndStoreRecords(const PDNS_RECORD dnsRecordList);
};
