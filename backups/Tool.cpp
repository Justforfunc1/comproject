/**********************************************************
 *  \file Tool.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author Allen.L
**********************************************************/
#include "Tool.h"

int Tool::mkDir(char *path) {
	if (access(path, 0) != 0) {
		return mkdir(path, 777);
	} else
		return 0;
}

int Tool::mv(char *oldpath, char *newpath) {
	char cmd[300];
	sprintf(cmd, "mv %s %s", oldpath, newpath);
	return system(cmd);
}

long Tool::parseTime(const char *format, const char* buf) {
	long ret = 0;
	char *p = strstr(format, "HH");
	if (p != NULL) {
		char tmp[10];
		snprintf(tmp, 3, buf + (p - format));
		ret += atoi(tmp) * 3600;
	}

	p = strstr(format, "MI");
	if (p != NULL) {
		char tmp[10];
		snprintf(tmp, 3, buf + (p - format));
		ret += atoi(tmp) * 60;
	}

	p = strstr(format, "SS");
	if (p != NULL) {
		char tmp[10];
		snprintf(tmp, 3, buf + (p - format));
		ret += atoi(tmp);
	}
	return ret;
}

long Tool::parseDateTime(const char *format, char* buf) {
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

string Tool::getWeek() {
	int week_no;
	struct timeval tv;
	struct tm curtime;
	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &curtime);

	if (curtime.tm_wday == 0) {
		week_no = 7;
	} else
		week_no = curtime.tm_wday;
	char week[3];
	sprintf(week, "%d", week_no);
	return string(week);
}
