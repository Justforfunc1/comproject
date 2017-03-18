/**********************************************************
 * \file GPEncrypt.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include "gp_base/GPEncrypt.h"

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

//初始化函数
static GPVOID _rc4_init(GPByte *s, GPByte *key, unsigned long Len) {
	GPInt32 i = 0, j = 0;
	GPChar k[256] = { 0 };
	GPByte tmp = 0;
	for (i = 0; i < 256; i++) {
		s[i] = i;
		k[i] = key[i % Len];
	}
	for (i = 0; i < 256; i++) {
		j = (j + s[i] + k[i]) % 256;
		tmp = s[i];
		s[i] = s[j]; //交换s[i]和s[j]
		s[j] = tmp;
	}
}

//加解密
static GPVOID _rc4_crypt(GPByte *s, GPByte *Data, unsigned long Len) {
	GPInt32 i = 0, j = 0, t = 0;
	unsigned long k = 0;
	GPByte tmp;
	for (k = 0; k < Len; k++) {
		i = (i + 1) % 256;
		j = (j + s[i]) % 256;
		tmp = s[i];
		s[i] = s[j]; //交换s[x]和s[y]
		s[j] = tmp;
		t = (s[i] + s[j]) % 256;
		Data[k] ^= s[t];
	}
}


static GPVOID rc4_crypt(GPByte *key, unsigned long keyLen, GPByte *data, unsigned long dataLen) {
	GPByte s[256] = { 0 }; //S-box
	_rc4_init(s, key, keyLen);
	_rc4_crypt(s, data, dataLen);
}

//rc4
GPString GPEncrypt::rc4String(const GPString& key, const GPString& str) {
	GPByte *ret = (GPByte*) malloc(str.length());
	memcpy(ret, str.c_str(), str.length());
	rc4_crypt((GPByte*) key.c_str(), key.length(), ret, str.length());
	GPString sret((GPChar*) ret, str.length());
	free(ret);
	return sret;
}


}//namespace GPUniversal
