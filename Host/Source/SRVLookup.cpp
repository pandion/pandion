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
 * Filename:    SRVLookup.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This file implements a class that looks up SRV records, orders
 *              them according to the SRV specification and can supply the
 *              records to agents.
 *              See http://www.ietf.org/rfc/rfc2782.txt for additional info
 *              about SRV.
 */
#include <stdafx.h>
#include "SRVLookup.h"

/*
 * SRVRecord constructor
 */
SRVRecord::SRVRecord(_bstr_t targetName, WORD port, WORD priority, WORD weight)
{
	this->targetName = targetName;
	this->port = port;
	this->priority = priority;
	this->weight = weight;
}

/*
 * SRVRecord destructor
 */
SRVRecord::~SRVRecord()
{
}

/*
 * Getter for the SRV server name
 */
_bstr_t SRVRecord::getTargetName()
{
	return targetName;
}

/*
 * Getter for the SRV port
 */
WORD SRVRecord::getPort()
{
	return port;
}

/*
 * Getter for the SRV priority
 */
WORD SRVRecord::getPriority()
{
	return priority;
}

/*
 * Getter for the SRV weight
 */
WORD SRVRecord::getWeight()
{
	return weight;
}

/*
 * SRVLookup constructor, takes a service, protocol and domain string and ties
 * them together into an SRV request string. This value is stored in m_srvname.
 */
SRVLookup::SRVLookup(_bstr_t service, _bstr_t protocol, _bstr_t domain)
{
	m_srvname = 
		_bstr_t(L"_") + service + 
		_bstr_t(L"._") + protocol + 
		_bstr_t(L".") + domain;
}

/*
 * SRVLookup destructor
 */
SRVLookup::~SRVLookup()
{
}

/*
 * DoLookup method queries the DNS server and sorts and stores the resulting
 * records with the SortAndStoreRecords helper method.
 */
HRESULT SRVLookup::DoLookup()
{
	PDNS_RECORD dnsRecordList = NULL;
	DNS_STATUS dnsStatus = ::DnsQuery(m_srvname, DNS_TYPE_SRV,
		DNS_QUERY_STANDARD, NULL, &dnsRecordList, NULL);
	if(DNS_RCODE_NOERROR == dnsStatus)
	{
		HRESULT hr = SortAndStoreRecords(dnsRecordList);
		::DnsRecordListFree(dnsRecordList, DnsFreeRecordList);
		return hr;
	}
	else
	{
		return E_FAIL;
	}
}

/*
 * Comparison functor to sort SRVRecords on ascending priority.
 */
struct SRVPrioritySort
{
	// TODO: Figure out why pass-by-reference doesn't work with stable_sort.
	bool operator()(SRVRecord a, SRVRecord b) 
	{ 
		return a.getPriority() < b.getPriority();
	}
};

/*
 * Comparison functor to sort SRVRecords on descending weight.
 */
struct SRVWeightSort
{
	bool operator()(SRVRecord &a, SRVRecord &b) 
	{ 
		return a.getWeight() > b.getWeight(); 
	}
};

/*
 * SortAndStoreRecords filters out all SRV records, stores them in the
 * m_records vector and then sorts that vector.
 */
HRESULT SRVLookup::SortAndStoreRecords(const PDNS_RECORD dnsRecordList)
{
	m_records.clear();

	for(PDNS_RECORD iterator = dnsRecordList;
		iterator;
		iterator = iterator->pNext)
	{
		if(iterator->wType == DNS_TYPE_SRV)
		{
			SRVRecord rec = SRVRecord(iterator->Data.Srv.pNameTarget,
				iterator->Data.Srv.wPort,
				iterator->Data.Srv.wPriority,
				iterator->Data.Srv.wWeight);
			m_records.push_back(rec);
		}
	}
	if(m_records.empty())
	{
		return E_FAIL;
	}
	else if(m_records.size() == 1)
	{
		return S_OK;
	}
	// TODO: implement correct weight selection algorithm
	else
	{
		std::sort(m_records.begin(), 
			m_records.end(), 
			SRVWeightSort());
		std::stable_sort(m_records.begin(), 
			m_records.end(), 
			SRVPrioritySort());
		return S_OK;
	}
}

/*
 * Getter that supplies an iterator on the sorted SRV records to the agent.
 */
std::vector<SRVRecord>::iterator SRVLookup::getRecordsIterator()
{
	return m_records.begin();
}
