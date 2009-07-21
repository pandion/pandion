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
 * Filename:    Hash.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: This file implements two functions for computing MD5 and SHA1
 *              hashes.
 */

#include "stdafx.h"
#include "Hash.h"

void Hash::MD5(const unsigned char* message,
			   size_t messageSize,
			   unsigned char digest[16])
{
	HCRYPTPROV cP       = NULL;
	HCRYPTHASH hash     = NULL;
	DWORD      hashSize = 16;

	CryptAcquireContext(&cP, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
	CryptCreateHash(cP, CALG_MD5, 0, 0, &hash);
	CryptHashData(hash, message, messageSize, 0);
	CryptGetHashParam(hash, HP_HASHVAL, digest, &hashSize, 0);

    if(hash)
	{
        CryptDestroyHash(hash);
	}
    if(cP)
	{
        CryptReleaseContext(cP, 0);
	}
}


void Hash::SHA1(const unsigned char* message,
			    size_t messageSize,
			    unsigned char digest[20])
{
	HCRYPTPROV cP       = NULL;
	HCRYPTHASH hash     = NULL;
	DWORD      hashSize = 20;

	CryptAcquireContext(&cP, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
	CryptCreateHash(cP, CALG_SHA1, 0, 0, &hash);
	CryptHashData(hash, message, messageSize, 0);
	CryptGetHashParam(hash, HP_HASHVAL, digest, &hashSize, 0);

    if(hash)
	{
        CryptDestroyHash(hash);
	}
    if(cP)
	{
        CryptReleaseContext(cP, 0);
	}
}
