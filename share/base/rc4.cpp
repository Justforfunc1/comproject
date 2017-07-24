/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file rc4.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-07-03
 ** \last modified 2017-07-06 17:44:35
**********************************************************/

#include "base/rc4.h"

using namespace std;

static Byte xyzzy_tmpc;

#define SWAP_BYTE(a,b) xyzzy_tmpc=a; a=b; b=xyzzy_tmpc

bool RC4Init(const char *pszKey, uint32 nKeyLen, RC4_KEY *key) {
	Byte by1, by2;
	Byte* bySTab;
    uint32  nCount;
	if ((strlen(pszKey)<1)||(nKeyLen<1)){
		return false;
	}
	nKeyLen=(nKeyLen>256)?(256):nKeyLen; //口令最多只能256字节

    bySTab = &key->bySTab[0];
	for (nCount = 0; nCount < 256; nCount++){
		bySTab[nCount] = (Byte)nCount;
	}
    key->byIt=0;
    key->byJt=0;
    by1=by2=0;
    for (nCount=0; nCount<256; nCount++)
    {
		by2 = (Byte)(pszKey[by1] + bySTab[nCount] + by2);
        SWAP_BYTE(bySTab[nCount], bySTab[by2]);
		by1 = (Byte)(by1 + 1) % nKeyLen;
    }
 return true;
}

bool RC4Works (Byte *content, uint32 size, RC4_KEY *key ) {
	Byte byIt, byJt;
	Byte* bySTab;
	uint32 nCount;

	byIt = key->byIt;
	byJt = key->byJt;

	bySTab = &key->bySTab[0];
	for (nCount = 0; nCount < size; nCount++)
	{
		byIt++;
		byJt += bySTab[byIt];
		SWAP_BYTE(bySTab[byIt], bySTab[byJt]);
		content[nCount] ^= bySTab[(bySTab[byIt] + bySTab[byJt]) & 0xFF];
	}
	key->byIt = byIt;
	key->byJt = byJt;
	return true;
}

