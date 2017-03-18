/**********************************************************
 * \file GPRc4.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include "gp_base/GPRc4.h"

using namespace std;

static GPByte xyzzy_tmpc;

#define SWAP_GPByte(a,b) xyzzy_tmpc=a; a=b; b=xyzzy_tmpc

GPBOOL RC4Init(const GPChar *pszKey, GPUInt32 nKeyLen, RC4_KEY *key)
{
	GPByte by1, by2;
	GPByte* bySTab;
    GPUInt32  nCount;
	if((strlen(pszKey)<1)||(nKeyLen<1)){
		return false;
	}
	nKeyLen=(nKeyLen>256)?(256):nKeyLen; //口令最多只能256字节

    bySTab = &key->bySTab[0];
	for (nCount = 0; nCount < 256; nCount++){
		bySTab[nCount] = (GPByte)nCount;
	}
    key->byIt=0;
    key->byJt=0;
    by1=by2=0;
    for (nCount=0; nCount<256; nCount++)
    {
		by2 = (GPByte)(pszKey[by1] + bySTab[nCount] + by2);
        SWAP_GPByte(bySTab[nCount], bySTab[by2]);
		by1 = (GPByte)(by1 + 1) % nKeyLen;
    }
 return true;
}

GPBOOL RC4Works (GPByte* content, GPUInt32 size, RC4_KEY *key ){
	GPByte byIt, byJt;
	GPByte* bySTab;
	GPUInt32 nCount;

	byIt = key->byIt;
	byJt = key->byJt;

	bySTab = &key->bySTab[0];
	for (nCount = 0; nCount < size; nCount++)
	{
		byIt++;
		byJt += bySTab[byIt];
		SWAP_GPByte(bySTab[byIt], bySTab[byJt]);
		content[nCount] ^= bySTab[(bySTab[byIt] + bySTab[byJt]) & 0xFF];
	}
	key->byIt = byIt;
	key->byJt = byJt;
	return true;
}
