/**********************************************************
 * \file GPRc4.h
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#ifndef _GPRc4_H
#define _GPRc4_H

#include "gp_base/GPDefine.h"


#ifdef __cplusplus
extern "C" {
#endif

struct RC4_KEY{
	GPByte bySTab[256];   //256字节的S表
	GPByte byIt, byJt;     //t时刻的两个指针
};

GPBOOL RC4Init(const char*, GPUInt32, RC4_KEY *);
GPBOOL RC4Works (GPByte* content, GPUInt32 size, RC4_KEY *);

#ifdef __cplusplus
}
#endif


#endif

