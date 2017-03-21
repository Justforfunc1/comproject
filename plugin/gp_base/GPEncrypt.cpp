/**********************************************************
 * \file GPEncrypt.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include "gp_base/GPEncrypt.h"

#ifdef WIN32
#ifdef _MFC
#include <afxwin.h>
#else
#include <windows.h>
#endif
#else
#include <unistd.h>
#endif

using namespace std;

namespace GPUniversal
{


GPString GPEncrypt::ConvertUnicodeToUtf8(GPInt32 &unicode){
	// unciode to utf8
	int partHigh = (unicode & 0x0000F000) >> 12;  // XX XX 11110000 XX
	int partMiddle = (unicode & 0x00000FC0) >> 6; // XX XX X1111 1100X
	int partLow = (unicode & 0x0000003F) ; // XX XX XX 00111111
	partHigh += 0xE0;
	partMiddle += 0x80;
	partLow += 0x80;

	GPString oneWord;
	oneWord += (char)(partHigh);
	oneWord += (char)(partMiddle);
	oneWord += (char)(partLow);

	return oneWord;
}


GPVOID GPEncrypt::ConvertToUtf8(GPString &str, const eCharacterEncoding &originalEncoding){ ; }


GPVOID GPEncrypt::convertFullToHalf(GPString &str, eCharacterEncoding encoding){
	ConvertToUtf8(str, encoding);

    GPString temp;
	for (size_t i = 0; i < str.size(); i++) {
		if (((str[i] & 0xF0) ^ 0xE0) == 0) {
			int old_char = (str[i] & 0xF) << 12 | ((str[i + 1] & 0x3F) << 6 | (str[i + 2] & 0x3F));

			if (old_char == 0x3000) { // blank
				char new_char = 0x20;
				temp += new_char;
			}
			else if (old_char >= 0xFF01 && old_char <= 0xFF5E) { // full char
				char new_char = old_char - 0xFEE0;
				temp += new_char;
			}
			else { // other 3 bytes char
				temp += str[i];
				temp += str[i + 1];
				temp += str[i + 2];
			}
			i = i + 2;
		} else {
			temp += str[i];
		}
	}
	str = temp;
}


GPVOID GPEncrypt::convertHalfToFull(GPString &str, eCharacterEncoding encoding){
	ConvertToUtf8(str, encoding);

	GPString temp;
	int new_char;
	for (size_t i = 0; i < str.size(); ++i) {
		new_char = str[i];
		if(str[i] >= 0x21 && str[i] <= 0x7E){
			new_char += 0xFEE0;
			temp += ConvertUnicodeToUtf8(new_char);
		}
		else if(str[i] == 0x20){
			new_char = 0x3000;
			temp += ConvertUnicodeToUtf8(new_char);
		}
		else{
			temp += str[i];
		}
	}
	str = temp;
}

//获取字符串长度
size_t GPEncrypt::getStrSize(const GPString &str, eCharacterEncoding encoding){
	if(!str.empty()){
		size_t size = 0;

		size_t currentPos = 0;
		while(currentPos < str.size()){
			if((str[currentPos] & 0xFE) == 0xFC){ currentPos += 6; }
			else if((str[currentPos] & 0xFC) == 0xF8){ currentPos += 5; }
			else if((str[currentPos] & 0xF8) == 0xF0){ currentPos += 4; }
			else if((str[currentPos] & 0xF0) == 0xE0){ currentPos += 3; }
			else if((str[currentPos] & 0xE0) == 0xC0){ currentPos += 2; }
			else{ currentPos += 1; }

			++size;
		}

		return size;
	}
	return 0;
}

//把urldecode
GPInt32 GPEncrypt::urlDecode(const GPChar *src, GPChar *dest) {
	if (src == NULL || dest == NULL) {
		return -1;
	}

	const GPChar *psrc = src;
	GPChar *pdest = dest;

	while (*psrc) {
		if (*psrc == '+') {
			*pdest = ' ';
		} else if (*psrc == '%' && isxdigit(*(psrc + 1)) && isxdigit(*(psrc + 2))) {
			GPInt32 c = 0;
			for (GPInt32 i = 1; i <= 2; i++) {
				c <<= 4;
				if (psrc[i] >= '0' && psrc[i] <= '9') {
					c |= (psrc[i] - '0');
				} else if (psrc[i] >= 'a' && psrc[i] <= 'f') {
					c |= (psrc[i] - 'a') + 10;
				} else if (psrc[i] >= 'A' && psrc[i] <= 'F') {
					c |= (psrc[i] - 'A') + 10;
				}
			}
			*pdest = (GPChar) (c & 0xff);
			psrc += 2;
		} else {
			*pdest = *psrc;
		}
		psrc++;
		pdest++;
	}
	*pdest = '\0';

	return 0;
}

//urlEncode
GPInt32 GPEncrypt::urlEncode(const GPChar* src, GPChar* dest) {
	GPInt32 i;
	GPChar *wp = dest;

	for (i = 0; i < (GPInt32) strlen(src); i++) {
		GPInt32 c = ((GPByte *) src)[i];
		if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || (c != '\0' && strchr("_-.!~*'()", c))) {
			*(wp++) = c;
		} else {
			wp += sprintf(wp, "%%%02X", c);
		}
	}
	*wp = '\0';

	return 0;
}

//base64码表
static const GPChar base_trad[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
//base64码表(url)
static const GPChar base_url[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_=";


//base64 conde
GPChar *base64_encode(const GPChar* data, GPInt32 data_len, GPBOOL urlencode) {
	GPChar *base;
	if (!urlencode)
		base = (GPChar*) base_trad;
	else
		base = (GPChar*) base_url;
	GPInt32 prepare = 0;
	GPInt32 ret_len;
	GPInt32 temp = 0;
	GPChar *ret = NULL;
	GPChar *f = NULL;
	GPInt32 tmp = 0;
	GPChar changed[4];
	GPInt32 i = 0;
	ret_len = data_len / 3;
	temp = data_len % 3;
	if (temp > 0) {
		ret_len += 1;
	}
	ret_len = ret_len * 4 + 1;
	ret = (GPChar *) malloc(ret_len);
	if (ret == NULL) {
		printf("No enough memory.\n");
		exit(0);
	}
	memset(ret, 0, ret_len);
	f = ret;
	while (tmp < data_len) {
		temp = 0;
		prepare = 0;
		memset(changed, '\0', 4);
		while (temp < 3) {
			if (tmp >= data_len) {
				break;
			}
			prepare = ((prepare << 8) | (data[tmp] & 0xFF));
			tmp++;
			temp++;
		}
		prepare = (prepare << ((3 - temp) * 8));
		for (i = 0; i < 4; i++) {
			if (temp < i) {
				changed[i] = 0x40;
			} else {
				changed[i] = (prepare >> ((3 - i) * 6)) & 0x3F;
			}
			*f = base[changed[i]];
			f++;
		}
	}
	*f = '\0';
	return ret;
}

//find char
static GPChar find_pos(GPChar ch, GPBOOL urlencode) {
	GPChar *base;
	if (!urlencode)
		base = (GPChar*) base_trad;
	else
		base = (GPChar*) base_url;
	GPChar *ptr = (GPChar*) strrchr(base, ch);
	return (ptr - base);
}

//base64_decode
GPChar *base64_decode(const GPChar *data, GPInt32 data_len, GPBOOL urlencode) {
	GPChar *base;
	if (!urlencode)
		base = (GPChar*) base_trad;
	else
		base = (GPChar*) base_url;
	GPInt32 ret_len = (data_len / 4) * 3;
	GPInt32 equal_count = 0;
	GPChar *ret = NULL;
	GPChar *f = NULL;
	GPInt32 tmp = 0;
	GPInt32 temp = 0;
	GPChar need[3];
	GPInt32 prepare = 0;
	GPInt32 i = 0;
	if (*(data + data_len - 1) == '=') {
		equal_count += 1;
	}
	if (*(data + data_len - 2) == '=') {
		equal_count += 1;
	}
	if (*(data + data_len - 3) == '=') {
		equal_count += 1;
	}
	switch (equal_count) {
	case 0:
		ret_len += 4; //3 + 1 [1 for NULL]
		break;
	case 1:
		ret_len += 4; //Ceil((6*3)/8)+1
		break;
	case 2:
		ret_len += 3; //Ceil((6*2)/8)+1
		break;
	case 3:
		ret_len += 2; //Ceil((6*1)/8)+1
		break;
	}
	ret = (GPChar *) malloc(ret_len);
	if (ret == NULL) {
		printf("No enough memory.\n");
		exit(0);
	}
	memset(ret, 0, ret_len);
	f = ret;
	while (tmp < (data_len - equal_count)) {
		temp = 0;
		prepare = 0;
		memset(need, 0, 4);
		while (temp < 4) {
			if (tmp >= (data_len - equal_count)) {
				break;
			}
			prepare = (prepare << 6) | (find_pos(data[tmp], urlencode));
			temp++;
			tmp++;
		}
		prepare = prepare << ((4 - temp) * 6);
		for (i = 0; i < 3; i++) {
			if (i == temp) {
				break;
			}
			*f = (GPChar) ((prepare >> ((2 - i) * 8)) & 0xFF);
			f++;
		}
	}
	*f = '\0';
	return ret;
}

//base64编码
GPString GPEncrypt::base64Encode(const GPString& str, GPBOOL urlencode) {
	GPChar *ret = base64_encode(str.c_str(), str.length(), urlencode);
	GPString sret(ret);
	free(ret);
	return sret;
}

//base64解码
GPString GPEncrypt::base64Decode(const GPString& str, GPBOOL urlencode) {
	GPChar *ret = base64_decode(str.c_str(), str.length(), urlencode);
	GPString sret(ret);
	free(ret);
	return sret;
}

//rc4加密
GPBOOL GPEncrypt::rc4_encrypt(GPString &content, const GPString &sKey){
	return rc4_encrypt(const_cast<char*>(content.c_str()), (GPInt32)content.size(), sKey.c_str(), (GPInt32)sKey.size());
}


GPBOOL GPEncrypt::rc4_encrypt(GPByte* content, GPUInt32 size, const GPString &sKey){
	RC4_KEY key;
	RC4Init(sKey.c_str(), sKey.size(), &key);
	RC4Works(content, size, &key);
	return true;
}


#ifndef SWAP_VALUE
#define SWAP_VALUE(x, y) { t = *x; *x = *y; *y = t; }
#endif
GPBOOL GPEncrypt::rc4_encrypt(GPChar *content, GPInt32 contentLength, const GPChar* sKey, GPInt32 ketLength){
	GPChar t;
	GPByte box[256];
	GPInt32 index = 0;

	//生成对应的Key
	GPInt32 seed_len = ketLength;
	GPByte *seed = (GPByte *)sKey;

	GPInt32 src_len = contentLength;
	GPByte *src = (GPByte *)content;

	GPInt32 i;
	for(i = 0; i < 256; i++){
		box[i] = i;
	}

	GPInt32 j = 0;
	for (GPInt32 i = 0; i < 256; i++) {
		j = (j + seed[i % seed_len] + box[i]) % 256;
		SWAP_VALUE(&box[i], &box[j]);
	}

	GPInt32 x = 0; GPInt32 y = 0;
	for (GPInt32 i = 0; i < src_len; i++) {
		x = 0; y = 0;
		x = (x + 1) % 256;
		y = (box[x] + y) % 256;
		SWAP_VALUE(&box[x], &box[y]);
		index = (box[x] + box[y]) % 256;
		t = *(src + i);
		t ^= box[index];
		*(src + i) = t;
	}
	return true;
}

//字符串版本的rc4(加密)
GPBOOL GPEncrypt::myStrEncrypt(GPString &content, const GPString &key){
	if(rc4_encrypt(content, key)){
		return convertBinaryToHexString(content);
	}
	return false;
}

//字符串版本的rc4(解密)
GPBOOL GPEncrypt::myStrDecrypt(GPString &content, const GPString &key){
	if(convertHexStringToBinary(content)){
		return rc4_encrypt(content, key);
	}
	return false;
}

//二进制转字符串
GPBOOL GPEncrypt::convertBinaryToHexString(GPString &content){
	auto funcBinaryToStr = [](GPChar c)->GPChar{
		if(0 <= c && c <= 9){
			return c + '0';
		}
		else if(10 <= c && c <= 15){
			return c - 10 + 'A';
		}
		return 'G';
	};

	GPString tmp;
	for(auto iterContent = content.begin(); iterContent != content.end(); ++iterContent){
		tmp += funcBinaryToStr((*iterContent & 0xF0) >> 4);
		tmp += funcBinaryToStr(*iterContent & 0x0F);
	}
	content = tmp;
	return true;
}

//字符串转二进制
GPBOOL GPEncrypt::convertHexStringToBinary(GPString &content){
	auto funcHexCharToChar = [](char c)->char{
		switch(c){
			case '1': return 1; case '2': return 2; case '3': return 3; case '4': return 4; case '5': return 5; case '6': return 6; case '7': return 7; case '8': return 8; case '9': return 9;
			case 'A': return 10; case 'B': return 11; case 'C': return 12; case 'D': return 13; case 'E': return 14; case 'F': return 15;
			default: return 0;
		}
	};

	GPString tmp;
	auto iterContent = content.begin();
	while(iterContent != content.end()){
		GPChar high = iterContent != content.end() ? *(iterContent++) : 0;
		GPChar low = iterContent != content.end() ? *(iterContent++) : 0;
		GPChar data = (funcHexCharToChar(high) << 4) + funcHexCharToChar(low);

		tmp += data;
	}
	content = tmp;
	return true;
}

//MD5加密(32位)
GPString GPEncrypt::md5(const GPString str){
	return MD5(str).md5();
}

//MD5加密(16位)
GPString GPEncrypt::md5_16(const GPString str){
	return MD5(str).md5().substr(8,16);
}

//uint32 转二进制
inline GPUInt32 binary(GPUInt32 swap) {
	swap = ((swap & 0x55555555) << 1) | ((swap & 0xAAAAAAAA) >> 1);
	swap = ((swap & 0x33333333) << 2) | ((swap & 0xCCCCCCCC) >> 2);
	swap = ((swap & 0x0F0F0F0F) << 4) | ((swap & 0xF0F0F0F0) >> 4);
	swap = ((swap & 0x00FF00FF) << 8) | ((swap & 0xFF00FF00) >> 8);
	swap = ((swap & 0x0000FFFF) << 16) | ((swap & 0xFFFF0000) >> 16);
	return swap;
}

//uint64 转二进制
inline GPUInt64 binary64(GPUInt64 swap) {
	swap = ((swap & 0x5555555555555555) << 1) | ((swap & 0xAAAAAAAAAAAAAAAA) >> 1);
	swap = ((swap & 0x3333333333333333) << 2) | ((swap & 0xCCCCCCCCCCCCCCCC) >> 2);
	swap = ((swap & 0x0F0F0F0F0F0F0F0F) << 4) | ((swap & 0xF0F0F0F0F0F0F0F0) >> 4);
	swap = ((swap & 0x00FF00FF00FF00FF) << 8) | ((swap & 0xFF00FF00FF00FF00) >> 8);
	swap = ((swap & 0x0000FFFF0000FFFF) << 16) | ((swap & 0xFFFF0000FFFF0000) >> 16);
	swap = ((swap & 0x00000000FFFFFFFF) << 32) | ((swap & 0xFFFFFFFF00000000) >> 32);
	return swap;
}

//uint32 ID加密
GPUInt32 GPEncrypt::encryptId32(GPUInt32 id, GPUInt32 key) {
	GPUInt32 temp = 0;
	GPUInt32 mod = 0;
	id += key;
	GPUInt32 temp_id = id * 10;
	for (; temp_id != 0;) {
		mod = ((temp_id = temp_id / 10)) % 10;
		temp += mod;
	}
	temp = id * 10 + (10 - temp % 10);
	return binary(temp);
}

//uint32 ID解密
GPUInt32 GPEncrypt::decryptId32(GPUInt32 id, GPUInt32 key) {
	GPUInt32 temp = binary(id);
	if (temp % 10 == 0)
		temp -= 1;
	temp /= 10;
	temp -= key;
	return temp;
}

//uint64 ID加密
GPUInt64 GPEncrypt::encryptId64(GPUInt64 id, GPUInt64 key) {
	GPUInt64 temp = 0;
	GPUInt64 mod = 0;
	id += key;
	GPUInt64 temp_id = id * 10;
	for (; temp_id != 0;) {
		mod = ((temp_id = temp_id / 10)) % 10;
		temp += mod;
	}
	temp = id * 10 + (10 - temp % 10);
	return binary64(temp);
}

//uint64 ID解密
GPUInt64 GPEncrypt::decryptId64(GPUInt64 id, GPUInt64 key) {
	GPUInt64 temp = binary64(id);
	if (temp % 10 == 0)
		temp -= 1;
	temp /= 10;
	temp -= key;
	return temp;
}

}//namespace GPUniversal
