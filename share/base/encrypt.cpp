/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file encrypt.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-07-03
 ** \last modified 2017-07-19 15:47:44
**********************************************************/

#include "base/encrypt.h"

namespace base {
namespace utils {

std::string Encrypt::Get32BitMd5(const std::string str) {
	return MD5(str).md5();
}

std::string Encrypt::Get16BitMd5(const std::string str) {
	return MD5(str).md5().substr(8,16);
}

bool Encrypt::Rc4Encrypt(unsigned char *content, uint32 size, const std::string &skey) {
	RC4_KEY key;
	RC4Init(skey.c_str(), skey.size(), &key);
	RC4Works(content, size, &key);
	return true;
}

} //utils
} //base
