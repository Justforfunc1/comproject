/**********************************************************
 * \file GPTool.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include "gp_base/GPTool.h"

namespace GPUniversal
{


//得到str的hash值
GPInt32 GPTool::hashCode(const GPChar *str) {
	GPInt32 h = 0;
	while (*str) {
		h = 31 * h + (*str);
		str++;
	}
	return h;
}

//得到一个str的hash值的素数
GPInt32 GPTool::getPrimeHash(const GPChar *str) {
	GPInt32 h = 0;
	while (*str) {
		h = 31 * h + (*str);
		str++;
	}
	return ((h & 0x07FFFFFFF) % 269597);
}

//比较好的hash算法	http://murmurhash.googlepages.com/
GPUInt32 GPTool::murMurHash(const GPVOID *key, GPInt32 len) {
	const GPUInt32 m = 0x5bd1e995;
	const GPInt32 r = 24;
	const GPInt32 seed = 97;
	GPUInt32 h = seed ^ len;
	// Mix 4 bytes at a time into the hash
	const GPByte *data = (const GPByte *) key;
	while (len >= 4) {
		GPUInt32 k = *(GPUInt32 *) data;
		k *= m;
		k ^= k >> r;
		k *= m;
		h *= m;
		h ^= k;
		data += 4;
		len -= 4;
	}
	// Handle the last few bytes of the input array
	switch (len) {
	case 3:
		h ^= data[2] << 16;
		break;
	case 2:
		h ^= data[1] << 8;
		break;
	case 1:
		h ^= data[0];
		h *= m;
		break;
	};
	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;
	return h;
}

//格式化
GPString GPTool::formatByteSize(GPDouble bytes) {
	 const GPChar _sizeunits[] = "KMGTP";
	GPChar s[16];
	GPInt32 level = 0;
	while (bytes >= 1024.0) {
		bytes /= 1024.0;
		level++;
		if (level >= 5)
			break;
	}

	if (level > 0) {
		snprintf(s, 16, "%.1f%c", bytes, _sizeunits[level - 1]);
	} else {
		snprintf(s, 16, "%d", (GPInt32) bytes);
	}
	return s;
}

//格式字符串和日期
GPVOID GPTool::formatTimeStr(GPChar *str) {
	time_t rawtime;
	struct tm* curtime;

	time(&rawtime);
	curtime = localtime(&rawtime);

	GPChar tmp[10];
	GPChar *p = strstr(str, "YYYY");
	if (p != NULL) {
		sprintf(tmp, "%04d", curtime->tm_year + 1900);
		strncpy(p, tmp, 4);
	}

	p = strstr(str, "MM");
	if (p != NULL) {
		sprintf(tmp, "%02d", curtime->tm_mon + 1);
		strncpy(p, tmp, 2);
	}

	p = strstr(str, "DD");
	if (p != NULL) {
		sprintf(tmp, "%02d", curtime->tm_mday);
		strncpy(p, tmp, 2);
	}

	p = strstr(str, "HH");
	if (p != NULL) {
		sprintf(tmp, "%02d", curtime->tm_hour);
		strncpy(p, tmp, 2);
	}

	p = strstr(str, "MI");
	if (p != NULL) {
		sprintf(tmp, "%02d", curtime->tm_min);
		strncpy(p, tmp, 2);
	}

	p = strstr(str, "SS");
	if (p != NULL) {
		sprintf(tmp, "%02d", curtime->tm_sec);
		strncpy(p, tmp, 2);
	}
}

//获取HHMISS时间戳
long GPTool::parseTime(const GPChar *format, const GPChar* buf) {
	long ret = 0;
	GPChar *p = (GPChar *)strstr(format, "HH");
	if (p != NULL) {
		GPChar tmp[10];
		snprintf(tmp, 3, buf + (p - format));
		ret += atoi(tmp) * 3600;
	}

	p =  (GPChar *)strstr(format, "MI");
	if (p != NULL) {
		GPChar tmp[10];
		snprintf(tmp, 3, buf + (p - format));
		ret += atoi(tmp) * 60;
	}

	p =  (GPChar *)strstr(format, "SS");
	if (p != NULL) {
		GPChar tmp[10];
		snprintf(tmp, 3, buf + (p - format));
		ret += atoi(tmp);
	}
	return ret;
}

//当日时间
long GPTool::parseDateTime(const GPChar *format, GPChar* buf) {
	struct tm tm;
	memset(&tm, 0, sizeof(tm));
	if (buf != NULL) {
		if (strptime(buf, format, &tm) == NULL) {
			return 0;
		}
	} else { //缺省输出当前时间
		struct timeval tv;
		gettimeofday(&tv, NULL);
		localtime_r(&tv.tv_sec, &tm);
	}
	return mktime(&tm);
}

//获取周期时间
GPString GPTool::getWeek() {
	GPInt32 week_no;
	struct timeval tv;
	struct tm curtime;
	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &curtime);

	if (curtime.tm_wday == 0) {
		week_no = 7;
	} else
		week_no = curtime.tm_wday;
	GPChar week[3];
	sprintf(week, "%d", week_no);
	return GPString(week);
}

//执行shell脚本
#ifdef LINUX
	GPBOOL  GPTool::execShellCmd( const GPString &cmd ){
		FILE* fp = NULL;
		if( (fp=popen(cmd.c_str(), "r" ) ) != NULL ){
			pclose( fp);  fp = NULL;
			return true;
		}
		return false;
	}
#endif

//获取本机ip
	GPVOID GPTool::getIp(GPString &ip){
		ip.clear();
#ifdef WIN32
		/*
		// 获得本机主机名
		char hostname[MAX_PATH] = { 0 };
		gethostname(hostname, MAX_PATH);
		struct hostent FAR* lpHostEnt = gethostbyname(hostname);
		if(lpHostEnt == NULL)
		{
			return DEFAULT_IP;
		}

		// 取得IP地址列表中的第一个为返回的IP(因为一台主机可能会绑定多个IP)
		LPSTR lpAddr = lpHostEnt->h_addr_list[0];

		// 将IP地址转化成字符串形式
		struct in_addr inAddr;
		memmove(&inAddr, lpAddr, 4);

		return CString(inet_ntoa(inAddr));
		*/
#else
#endif
}


}//namespace GPUniversal
