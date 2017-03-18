/**********************************************************
 *  \file Encrypt.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author Allen.L
 **********************************************************/

#include "Encrypt.h"

const uint8 PRICE_KEY[16] = { 0x3A, 0xDA, 0x75, 0x21, 0xDB, 0xE2, 0xDB, 0xB3, 0x11, 0xB4, 0x49, 0x01, 0xA5, 0xC6, 0xEA, 0xD4 };

//------ ID加解密 --------
inline uint32 binary(uint32 swap) {
	swap = ((swap & 0x55555555) << 1) | ((swap & 0xAAAAAAAA) >> 1);
	swap = ((swap & 0x33333333) << 2) | ((swap & 0xCCCCCCCC) >> 2);
	swap = ((swap & 0x0F0F0F0F) << 4) | ((swap & 0xF0F0F0F0) >> 4);
	swap = ((swap & 0x00FF00FF) << 8) | ((swap & 0xFF00FF00) >> 8);
	swap = ((swap & 0x0000FFFF) << 16) | ((swap & 0xFFFF0000) >> 16);
	return swap;
}

inline uint64 binary64(uint64 swap) {
	swap = ((swap & 0x5555555555555555) << 1) | ((swap & 0xAAAAAAAAAAAAAAAA) >> 1);
	swap = ((swap & 0x3333333333333333) << 2) | ((swap & 0xCCCCCCCCCCCCCCCC) >> 2);
	swap = ((swap & 0x0F0F0F0F0F0F0F0F) << 4) | ((swap & 0xF0F0F0F0F0F0F0F0) >> 4);
	swap = ((swap & 0x00FF00FF00FF00FF) << 8) | ((swap & 0xFF00FF00FF00FF00) >> 8);
	swap = ((swap & 0x0000FFFF0000FFFF) << 16) | ((swap & 0xFFFF0000FFFF0000) >> 16);
	swap = ((swap & 0x00000000FFFFFFFF) << 32) | ((swap & 0xFFFFFFFF00000000) >> 32);
	return swap;
}

uint32 encryptId32(uint32 id, uint32 key) {
	uint32 temp = 0;
	uint32 mod = 0;
	id += key;
	uint32 temp_id = id * 10;
	for (; temp_id != 0;) {
		mod = ((temp_id = temp_id / 10)) % 10;
		temp += mod;
	}
	temp = id * 10 + (10 - temp % 10);
	return binary(temp);
}

uint32 decryptId32(uint32 id, uint32 key) {
	uint32 temp = binary(id);
	if (temp % 10 == 0)
		temp -= 1;
	temp /= 10;
	temp -= key;
	return temp;
}

uint64 encryptId64(uint64 id, uint64 key) {
	uint64 temp = 0;
	uint64 mod = 0;
	id += key;
	uint64 temp_id = id * 10;
	for (; temp_id != 0;) {
		mod = ((temp_id = temp_id / 10)) % 10;
		temp += mod;
	}
	temp = id * 10 + (10 - temp % 10);
	return binary64(temp);
}

uint64 decryptId64(uint64 id, uint64 key) {
	uint64 temp = binary64(id);
	if (temp % 10 == 0)
		temp -= 1;
	temp /= 10;
	temp -= key;
	return temp;
}

//------ 价格加解密 --------
int tea_encrypt(uint32 *in, uint32 *out) {
	if (in == 0 || out == 0)
		return -1;

	// convert from uint8 to uint32
	const uint32 *k = (uint32 *) PRICE_KEY;

	uint32 y = in[0];
	uint32 z = in[1];

	uint32 a = k[0];
	uint32 b = k[1];
	uint32 c = k[2];
	uint32 d = k[3];

	uint32 delta = 0x9E3779B9;
	uint32 sum = 0;
	int round = 32;

	while (round--) {
		sum += delta;
		y += ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b);
		z += ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d);
	}

	out[0] = y;
	out[1] = z;

	return 0;
}

// 64bits to 64bits
static int tea_decrypt(uint32 *in, uint32 *out) {
	if (in == 0 || out == 0)
		return -1;
	// convert from uint8 to uint32
	const uint32 *k = (uint32 *) PRICE_KEY;

	uint32 y = in[0];
	uint32 z = in[1];

	uint32 a = k[0];
	uint32 b = k[1];
	uint32 c = k[2];
	uint32 d = k[3];

	uint32 delta = 0x9E3779B9;
	uint32 sum = 0;
	int round = 32;

	sum = 0xC6EF3720; // delta << 5

	while (round--) {
		z -= ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d);
		y -= ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b);
		sum -= delta;
	}

	out[0] = y;
	out[1] = z;

	return 0;
}

inline int uint64tobytes(uint64 in, uint8 *out) {
	out[0] = (uint8) (in & 0xff);
	out[1] = (uint8) ((in & 0xff00) >> 8);
	out[2] = (uint8) ((in & 0xff0000) >> 16);
	out[3] = (uint8) ((in & 0xff000000) >> 24);
	out[4] = (uint8) ((in & 0xff00000000) >> 32);
	out[5] = (uint8) ((in & 0xff0000000000) >> 40);
	out[6] = (uint8) ((in & 0xff000000000000) >> 48);
	out[7] = (uint8) ((in & 0xff00000000000000) >> 56);
	return 0;
}

inline uint64 bytestouint64(uint8 *in) {
	uint64 ret = 0;

	ret += (uint64) in[0] & 0xff;
	ret += ((uint64) in[1] << 8) & 0xff00;
	ret += ((uint64) in[2] << 16) & 0xff0000;
	ret += ((uint64) in[3] << 24) & 0xff000000;
	ret += ((uint64) in[4] << 32) & 0xff00000000;
	ret += ((uint64) in[5] << 40) & 0xff0000000000;
	ret += ((uint64) in[6] << 48) & 0xff000000000000;
	ret += ((uint64) in[7] << 56) & 0xff00000000000000;
	return ret;
}

string encryptPrice(uint64 in) {
	uint8 temp[8];
	uint8 entemp[8];
	uint64tobytes(in, temp);
	if (tea_encrypt((uint32 *) temp, (uint32 *) entemp) == -1) {
		return "";
	} else {
		char tmp[20];
		sprintf(tmp, "%02x%02x%02x%02x%02x%02x%02x%02x", entemp[7], entemp[6], entemp[5], entemp[4], entemp[3], entemp[2], entemp[1], entemp[0]);
		return string(tmp);
	}
}

bool decryptPrice(uint64 in, uint64& out) {
	uint8 temp[8];
	uint8 detemp[8];
	uint64tobytes(in, temp);
	if (tea_decrypt((uint32 *) temp, (uint32 *) detemp) == -1) {
		return false;
	} else {
		out = bytestouint64(detemp);
		return true;
	}
}

//----- showtime+ip -----------
const int enbox[] = { 122, 114, 106, 98, 90, 82, 74, 66, 58, 50, 42, 34, 26, 18, 10, 2, 124, 116, 108, 100, 92, 84, 76, 68, 60, 52, 44, 36, 28, 20,
		12, 4, 126, 118, 110, 102, 94, 86, 78, 70, 62, 54, 46, 38, 30, 22, 14, 6, 128, 120, 112, 104, 96, 88, 80, 72, 64, 56, 48, 40, 32, 24, 16, 8,
		121, 113, 105, 97, 89, 81, 73, 65, 57, 49, 41, 33, 25, 17, 9, 1, 123, 115, 107, 99, 91, 83, 75, 67, 59, 51, 43, 35, 27, 19, 11, 3, 125, 117,
		109, 101, 93, 85, 77, 69, 61, 53, 45, 37, 29, 21, 13, 5, 127, 119, 111, 103, 95, 87, 79, 71, 63, 55, 47, 39, 31, 23, 15, 7, 256, 248, 240,
		232, 224, 216, 208, 200, 192, 184, 176, 168, 160, 152, 144, 136, // 129 - 144
		250, 242, 234, 226, 218, 210, 202, 194, 186, 178, 170, 162, 154, 146, 138, 130, // 145 - 160
		252, 244, 236, 228, 220, 212, 204, 196, 188, 180, 172, 164, 156, 148, 140, 132, // 161 - 176
		249, 241, 233, 225, 217, 209, 201, 193, 185, 177, 169, 161, 153, 145, 137, 129, // 177 - 192
		253, 245, 237, 229, 221, 213, 205, 197, 189, 181, 173, 165, 157, 149, 141, 133, // 193 - 208
		255, 247, 239, 231, 223, 215, 207, 199, 191, 183, 175, 167, 159, 151, 143, 135, // 209 - 224
		254, 246, 238, 230, 222, 214, 206, 198, 190, 182, 174, 166, 158, 150, 142, 134, // 227 - 240
		251, 243, 235, 227, 219, 211, 203, 195, 187, 179, 171, 163, 155, 147, 139, 131  // 243 - 256
		};

string encodeTimeIp(uint32 rand_seed, uint32 show_time, uint32 show_ip) {
	char result[20];
	uint32 i1;
	uint32 i2;
	uint32 iMixed1 = 0;
	uint32 iMixed2 = 0;
	int i;

	show_ip ^= rand_seed;
	i1 = (show_time & 0xff000000) | ((show_ip >> 8) & 0xff0000) | ((show_time & 0xff0000) >> 8) | (show_ip & 0xff);
	i2 = ((show_time & 0xFFFF) << 16) | (show_ip & 0xFF00) | ((show_ip >> 16) & 0xFF);

	i = 4;
	while (i) {
		i--;
		iMixed1 <<= 8;
		iMixed1 |= enbox[i1 & 0xff] - 1;

		i1 >>= 8;
		i1 &= 0x00FFFFFF;
	}
	i = 4;
	while (i) {
		i--;
		iMixed2 <<= 8;
		iMixed2 |= enbox[i2 & 0xff] - 1;

		i2 >>= 8;
		i2 &= 0x00FFFFFF;
	}
	sprintf(result, "%08x%08x", iMixed1, iMixed2);
	return string(result);
}

//---------- referer url -------------------
int enbox_str[] = { 121, 113, 105, 97, 89, 81, 73, 65, 57, 49, 41, 33, 25, 17, 9, 1, 255, 247, 239, 231, 223, 215, 207, 199, 191, 183, 175, 167, 159,
		151, 143, 135, 123, 115, 107, 99, 91, 83, 75, 67, 59, 51, 43, 35, 27, 19, 11, 3, 249, 241, 233, 225, 217, 209, 201, 193, 185, 177, 169, 161,
		153, 145, 137, 129, 125, 117, 109, 101, 93, 85, 77, 69, 61, 53, 45, 37, 29, 21, 13, 5, 251, 243, 235, 227, 219, 211, 203, 195, 187, 179, 171,
		163, 155, 147, 139, 131, 127, 119, 111, 103, 95, 87, 79, 71, 63, 55, 47, 39, 31, 23, 15, 7, 248, 240, 232, 224, 216, 208, 200, 192, 184, 176,
		168, 160, 152, 144, 136, 128, 120, 112, 104, 96, 88, 80, 72, 64, 56, 48, 40, 32, 24, 16, 8, 0, 252, 244, 236, 228, 220, 212, 204, 196, 188,
		180, 172, 164, 156, 148, 140, 132, 122, 114, 106, 98, 90, 82, 74, 66, 58, 50, 42, 34, 26, 18, 10, 2, 254, 246, 238, 230, 222, 214, 206, 198,
		190, 182, 174, 166, 158, 150, 142, 134, 124, 116, 108, 100, 92, 84, 76, 68, 60, 52, 44, 36, 28, 20, 12, 4, 253, 245, 237, 229, 221, 213, 205,
		197, 189, 181, 173, 165, 157, 149, 141, 133, 126, 118, 110, 102, 94, 86, 78, 70, 62, 54, 46, 38, 30, 22, 14, 6, 250, 242, 234, 226, 218, 210,
		202, 194, 186, 178, 170, 162, 154, 146, 138, 130 };

// code长度 (src+3)/4*8
int encode_str(uint32 seed, char src[], char encode[]) {
	uint8 code;
	uint8 ch;

	uint8 seed_part[4];
	seed_part[0] = (uint8) (seed & 0xff);
	seed_part[1] = (uint8) (seed >> 8 & 0xff);
	seed_part[2] = (uint8) (seed >> 16 & 0xff);
	seed_part[3] = (uint8) (seed >> 24 & 0xff);

	int i = 0, j = 0;
	while (src[i] != '\0') {
		ch = (uint8) src[i];
		ch ^= seed_part[j];
		code = (uint8) enbox_str[ch];
		sprintf(encode + i * 2, "%02X", code);
		j++;
		j %= 4;
		i++;
	}
	return 0;
}

int debox_str[] = { 143, 15, 175, 47, 207, 79, 239, 111, 142, 14, 174, 46, 206, 78, 238, 110, 141, 13, 173, 45, 205, 77, 237, 109, 140, 12, 172, 44,
		204, 76, 236, 108, 139, 11, 171, 43, 203, 75, 235, 107, 138, 10, 170, 42, 202, 74, 234, 106, 137, 9, 169, 41, 201, 73, 233, 105, 136, 8, 168,
		40, 200, 72, 232, 104, 135, 7, 167, 39, 199, 71, 231, 103, 134, 6, 166, 38, 198, 70, 230, 102, 133, 5, 165, 37, 197, 69, 229, 101, 132, 4,
		164, 36, 196, 68, 228, 100, 131, 3, 163, 35, 195, 67, 227, 99, 130, 2, 162, 34, 194, 66, 226, 98, 129, 1, 161, 33, 193, 65, 225, 97, 128, 0,
		160, 32, 192, 64, 224, 96, 127, 63, 255, 95, 159, 223, 191, 31, 126, 62, 254, 94, 158, 222, 190, 30, 125, 61, 253, 93, 157, 221, 189, 29, 124,
		60, 252, 92, 156, 220, 188, 28, 123, 59, 251, 91, 155, 219, 187, 27, 122, 58, 250, 90, 154, 218, 186, 26, 121, 57, 249, 89, 153, 217, 185, 25,
		120, 56, 248, 88, 152, 216, 184, 24, 119, 55, 247, 87, 151, 215, 183, 23, 118, 54, 246, 86, 150, 214, 182, 22, 117, 53, 245, 85, 149, 213,
		181, 21, 116, 52, 244, 84, 148, 212, 180, 20, 115, 51, 243, 83, 147, 211, 179, 19, 114, 50, 242, 82, 146, 210, 178, 18, 113, 49, 241, 81, 145,
		209, 177, 17, 112, 48, 240, 80, 144, 208, 176, 16 };

int decode_str(uint32 seed, char encode[], char src[]) {
	uint8 code;
	uint8 ch;

	uint8 seed_part[4];
	seed_part[0] = (uint8) (seed & 0xff);
	seed_part[1] = (uint8) (seed >> 8 & 0xff);
	seed_part[2] = (uint8) (seed >> 16 & 0xff);
	seed_part[3] = (uint8) (seed >> 24 & 0xff);

	int i = 0, j = 0;
	while (encode[i * 2] != '\0' && encode[i * 2 + 1] != '\0') {
		code = 0;
		if (encode[i * 2] >= 'A' && encode[i * 2] <= 'F') {
			code = encode[i * 2] - 'A' + 10;
		} else if (encode[i * 2] >= '0' && encode[i * 2] <= '9') {
			code = encode[i * 2] - '0';
		}
		code <<= 4;
		if (encode[i * 2 + 1] >= 'A' && encode[i * 2 + 1] <= 'F') {
			code |= encode[i * 2 + 1] - 'A' + 10;
		} else if (encode[i * 2 + 1] >= '0' && encode[i * 2 + 1] <= '9') {
			code |= encode[i * 2 + 1] - '0';
		}
		ch = (uint8) debox_str[code];
		ch ^= seed_part[j];
		src[i] = ch;
		j++;
		j %= 4;
		i++;
	}
	src[i] = '\0';
	return 0;
}

/*用于点击跳转的URL加密*/
string bcd_encode(const unsigned char *src) {
	int srclen = strlen((char*) src);
	char dest[srclen * 2 + 1];
	int i, j;
	for (i = 0, j = 0; i < srclen; i++, j += 2) {
		dest[j] = (src[i] >> 4) > 9 ? (src[i] >> 4) - 10 + 'A' : (src[i] >> 4) + '0';
		dest[j + 1] = (src[i] & 0x0F) > 9 ? (src[i] & 0x0F) - 10 + 'A' : (src[i] & 0x0F) + '0';
	}
	dest[j] = '\0';
	return string(dest);
}
