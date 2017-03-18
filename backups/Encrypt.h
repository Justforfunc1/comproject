/**********************************************************
 *  \file Encrypt.h
 *  \briefi	数据加密
 *  \note	注意事项： 
 * 
 * \version 
 * * \author Allen.L
 **********************************************************/

#ifndef _Encrypt_H
#define _Encrypt_H
#include "common.h"
uint32 encryptId32(uint32 id, uint32 key);
uint32 decryptId32(uint32 id, uint32 key);
uint64 encryptId64(uint64 id, uint64 key);
uint64 decryptId64(uint64 id, uint64 key);

string encryptPrice(uint64 in);
bool decryptPrice(uint64 in, uint64& out);

string encodeTimeIp(uint32 rand_seed, uint32 show_time, uint32 show_ip);

int encode_str(uint32 seed, char src[], char encode[]);
int decode_str(uint32 seed, char encode[], char src[]);

string bcd_encode(const unsigned char *src);
#endif
