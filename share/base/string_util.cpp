/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file string_util.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-07-06
 ** \last modified 2017-09-04 16:54:02
**********************************************************/

#include "string_util.h"

namespace base {
namespace utils {

void StringUtil::Split(char *str, const char *delim, std::vector<char*> &list) {
	if (str == NULL) {
		return;
	}
	if (delim == NULL) {
		list.push_back(str);
		return;
	}

	char* s = str;
	const char* spanp = delim;

start:		while (*s) {
		if (*s == *spanp) {
			char* p = s;
			spanp++;
			s++;
			while (*spanp) {
				if (*s != *spanp) {
					spanp = delim;
					goto start;
				}
				spanp++;
				s++;
			}
			*p = '\0';
			list.push_back(str);
			str = s;
			spanp = delim;
		} else {
			s++;
		}
	}
	if (*str) {
		list.push_back(str);
	}
}

bool StringUtil::StrReplaceAll(std::string &str, const std::string &replace, const std::string &dest) {
	std::string::size_type pos = 0;
	std::string::size_type replace_size = replace.size();
	std::string::size_type dest_size = dest.size();
	while ((pos = str.find(replace, pos)) != std::string::npos) {
		str.replace(pos, replace_size, dest);
		pos += dest_size;
	}
	return true;
}

bool StringUtil::SplitString(const std::string &str, const std::string &delim, std::vector<std::string> &result) {
	result.clear();
	if (str.empty()) {
		return false;
	}
	int count = 0;
	std::string::size_type curindex = 0;
	std::string::size_type preindex = 0;
	while (1) {
		if (++count > static_cast<int32>(str.size())) {
			break;
		}
		curindex = str.find_first_of(delim, preindex);
		if (curindex == std::string::npos) {
			if (preindex != str.size()) {
				result.push_back(str.substr(preindex));
			}
			return true;
		}
		if (preindex != curindex) {
			result.push_back(str.substr(preindex, curindex - preindex));
		}
		else {
			result.push_back("");
		}
		preindex = curindex + delim.size();
	}
	return false;
}

bool StringUtil::SplitString(const std::string &str, const std::string &delim, std::list<std::string> &result) {
	result.clear();
	if (str.empty()) {
		return false;
	}
	int count = 0;
	std::string::size_type curindex = 0;
	std::string::size_type preindex = 0;
	while (1) {
		if (++count > static_cast<int32>(str.size())) {
			break;
		}
		curindex = str.find_first_of(delim, preindex);
		if (curindex == std::string::npos) {
			if (preindex != str.size()) {
				result.push_back(str.substr(preindex));
			}
			return true;
		}
		if (preindex != curindex) {
			result.push_back(str.substr(preindex, curindex - preindex));
		}
		preindex = curindex + delim.size();
	}
	return false;
}

std::string StringUtil::MergeString(const std::vector<std::string> &vector, const std::string &delim) {
	std::string tmp;
	for (std::vector<std::string>::const_iterator const_iter = vector.begin(); const_iter != vector.end();) {
		tmp += *const_iter;
		if (++const_iter != vector.end())
			tmp += delim;
	}
	return tmp;
}

std::string StringUtil::TrimRight(const std::string &str) {
	std::string trim_str = str.substr(0, str.find_last_not_of(" ")+1);
	trim_str = trim_str.substr(0, trim_str.find_last_not_of("\t")+1);
	trim_str = trim_str.substr(0, trim_str.find_last_not_of("\r\n")+1);
	trim_str = trim_str.substr(0, trim_str.find_last_not_of("\n")+1);
	return trim_str;
}

std::string StringUtil::TrimLeft(const std::string &str) {
	std::string trim_str = str.substr(str.find_first_not_of(" ") == std::string::npos ? str.size()-1 : str.find_first_not_of(" "));
	trim_str = trim_str.substr(trim_str.find_first_not_of("\t") == std::string::npos ? str.size()-1 : trim_str.find_first_not_of("\r\n"));
	trim_str = trim_str.substr(trim_str.find_first_not_of("\n") == std::string::npos ? str.size()-1 : trim_str.find_first_not_of("\n"));
	return trim_str;
}

std::string StringUtil::Trim(const std::string &str) {
	return str.empty() ? str : TrimRight(TrimLeft(str));
}

char *StringUtil::StrToLower(char *str) {
	if (str == NULL)
		return str;

	char *p = str;
	while (*p) {
		if ((*p) & 0x80)
			p++;
		else if ((*p) >= 'A' && (*p) <= 'Z')
			(*p) += 32;
		p++;
	}
	return str;
}

char *StringUtil::StrToUpper(char *str) {
	if (str == NULL)
		return str;

	char  *p = str;
	while (*p) {
		if ((*p) & 0x80)
			p++;
		else if ((*p) >= 'a' && (*p) <= 'z')
			(*p) -= 32;
		p++;
	}
	return str;
}

int32 StringUtil::IsInt(const char* p) {
	if (p == NULL || (*p) == '\0')
		return 0;
	if ((*p) == '-')
		p++;
	while ((*p)) {
		if ((*p) < '0' || (*p) > '9')
			return 0;
		p++;
	}
	return 1;
}

std::string StringUtil::IntToStr(const long &num) {
	std::stringstream str_stream;
	str_stream << num;
	return str_stream.str();
}

int32 StringUtil::StrToInt(const char *str, int32 dest) {
	if (IsInt(str)) {
		return atoi(str);
	} else {
		return dest;
	}
}

bool StringUtil::CheckNumber(const std::string &num) {
	if (num.empty()) {
		return false;
	}
	std::stringstream sin(num);
	double double_tmp;
	char char_tmp;
	if (!(sin >> double_tmp))
		return false;
	if (sin >> char_tmp)
		return false;

	return true;
}

void StringUtil::FormatTimeStr(char *str) {
	time_t rawtime;
	struct tm* curtime;
	time(&rawtime);
	curtime = localtime(&rawtime);
	char  tmp[10];
	char  *p = strstr(str, "YYYY");
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

} // utils
} // base
