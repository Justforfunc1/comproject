/**********************************************************
 *  \file StringUtil.h
 *  \brief	
 *  \note	注意事项： 
 * 
 * \version 
 * * \author Allen.L
 ***********************************************************/

#ifndef STRINGUTIL_H_
#define STRINGUTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>
#include <vector>

namespace lpsys {

/**
 * 字符串操作，以及转换的封装
 */
class StringUtil {
public:
	// 把string转成int
	static int strToInt(const char *str, int d);
	// 是整数
	static int isInt(const char *str);
	// 转成小写
	static char *strToLower(char *str);
	// 转成大写
	static char *strToUpper(char *str);
	// trim
	static char *trim(char *str, const char *what = " ", int mode = 3);
	// hash_value
	static int hashCode(const char *str);
	// 得到一个str的hash值的素数
	static int getPrimeHash(const char *str);
	// 把string以delim分隔开,放到list中
	static void split(char *str, const char *delim, std::vector<char*> &list);
	// urldecode
	static int urlDecode(const char *src, char *dest);
	// urlencode
	static int urlEncode(const char* src, char* dest);
	// http://murmurhash.googlepages.com/
	static unsigned int murMurHash(const void *key, int len);
	// 把bytes转成可读的, 如 10K 12M 等
	static std::string formatByteSize(double bytes);
	// 格式化日期
	static void formatTimeStr(char *str);
};

}

#endif /* STRINGUTIL_H_ */
