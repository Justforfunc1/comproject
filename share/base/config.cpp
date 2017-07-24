/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file config.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-07-06
 ** \last modified 2017-07-19 17:24:34
**********************************************************/

#include "base/config.h"
#include "base/string_util.h"

namespace base {

Config::Config() {;}

Config::~Config() {
	for (STR_MAP_ITER it = config_map_.begin(); it != config_map_.end(); ++it) {
		delete it->second;
	}
}

Config &Config::GetConfig() {
	static Config static_config;
	return static_config;
}

int32 Config::ParseValue(char *str, char *key, char *val) {
	char *p, *p1, *name, *value;

	if (str == NULL)
		return -1;

	p = str;
	// 去前置空格
	while ((*p) == ' ' || (*p) == '\t' || (*p) == '\r' || (*p) == '\n')
		p++;
	p1 = p + strlen(p);
	// 去后置空格
	while (p1 > p) {
		p1--;
		if (*p1 == ' ' || *p1 == '\t' || *p1 == '\r' || *p1 == '\n')
			continue;
		p1++;
		break;
	}
	(*p1) = '\0';
	// 是注释行或空行
	if (*p == '#' || *p == '\0')
		return -1;
	p1 = strchr(str, '=');
	if (p1 == NULL)
		return -1;
	name = p;
	value = p1 + 1;
	while ((*(p1 - 1)) == ' ')
		p1--;
	(*p1) = '\0';

	while ((*value) == ' ')
		value++;
	p = strchr(value, '#');
	if (p == NULL)
		p = value + strlen(value);
	while ((*(p - 1)) <= ' ')
		p--;
	(*p) = '\0';
	if (name[0] == '\0')
		return -1;

	strcpy(key, name);
	strcpy(val, value);

	return 0;
}

char *Config::IsSectionName(char *str) {
	if (str == NULL || strlen(str) <= 2 || (*str) != '[')
		return NULL;

	char *p = str + strlen(str);
	while ((*(p - 1)) == ' ' || (*(p - 1)) == '\t' || (*(p - 1)) == '\r' || (*(p - 1)) == '\n')
		p--;
	if (*(p - 1) != ']')
		return NULL;
	*(p - 1) = '\0';

	p = str + 1;
	while (*p) {
		if ((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z') || (*p >= '0' && *p <= '9') || (*p == '_')) {
		} else {
			return NULL;
		}
		p++;
	}
	return (str + 1);
}

int32 Config::Load(const char *filename) {
	FILE *fp;
	char key[128], value[4096], data[4096];
	int32 ret = 0;
	int32 line = 0;

	if ((fp = fopen(filename, "rb")) == NULL) {
		std::cout << "不能打开配置文件:" << filename << std::endl;
		return -1;
	}

	STR_STR_MAP *m = NULL;
	while (fgets(data, 4096, fp)) {
		line++;
		char *section_name = IsSectionName(data);
		// 是段名
		if (section_name != NULL) {
			STR_MAP_ITER it = config_map_.find(section_name);
			if (it == config_map_.end()) {
				m = new STR_STR_MAP();
				config_map_.insert(STR_MAP::value_type(section_name, m));
			} else {
				m = it->second;
			}
			continue;
		}
		ret = ParseValue(data, key, value);
		if (ret == -2) {
			std::cout << "解析错误, Line:" << line << ", " << data << std::endl;
			fclose(fp);
			return -1;
		}
		if (ret < 0) {
			continue;
		}
		if (m == NULL) {
			std::cout << "没在配置section, Line:" << line << ", " << data << std::endl;
			fclose(fp);
			return -1;
		}

		STR_STR_MAP_ITER it1 = m->find(key);
		if (it1 != m->end()) {
			it1->second += '\0';
			it1->second += value;
		} else {
			m->insert(STR_STR_MAP::value_type(key, value));
		}
	}
	fclose(fp);
	return 0;
}

const char *Config::GetString(const char *section, const std::string &key, const char *d) {
	STR_MAP_ITER it = config_map_.find(section);
	if (it == config_map_.end()) {
		return d;
	}
	STR_STR_MAP_ITER it1 = it->second->find(key);
	if (it1 == it->second->end()) {
		return d;
	}
	return it1->second.c_str();
}

std::vector<const char*> Config::GetStringList(const char *section, const std::string &key) {
	std::vector<const char*> ret;
	STR_MAP_ITER it = config_map_.find(section);
	if (it == config_map_.end()) {
		return ret;
	}
	STR_STR_MAP_ITER it1 = it->second->find(key);
	if (it1 == it->second->end()) {
		return ret;
	}
	const char *data = it1->second.data();
	const char *p = data;
	for (int32 i = 0; i < static_cast<int32>(it1->second.size()); i++) {
		if (data[i] == '\0') {
			ret.push_back(p);
			p = data + i + 1;
		}
	}
	ret.push_back(p);
	return ret;
}

int32 Config::GetInt(const char *section, const std::string &key, int32 d) {
	const char *str = GetString(section, key);
	return base::utils::StringUtil::StrToInt(str, d);
}

std::vector<int32> Config::GetIntList(const char *section, const std::string &key) {
	std::vector<int32> ret;
	STR_MAP_ITER it = config_map_.find(section);
	if (it == config_map_.end()) {
		return ret;
	}
	STR_STR_MAP_ITER it1 = it->second->find(key);
	if (it1 == it->second->end()) {
		return ret;
	}
	const char *data = it1->second.data();
	const char *p = data;
	for (int32 i = 0; i < static_cast<int32>(it1->second.size()); i++) {
		if (data[i] == '\0') {
			ret.push_back(atoi(p));
			p = data + i + 1;
		}
	}
	ret.push_back(atoi(p));
	return ret;
}

int32 Config::GetSectionKey(const char *section, std::vector<std::string> &keys) {
	STR_MAP_ITER it = config_map_.find(section);
	if (it == config_map_.end()) {
		return -1;
	}
	STR_STR_MAP_ITER it1;
	for (it1 = it->second->begin(); it1 != it->second->end(); ++it1) {
		keys.push_back(it1->first);
	}
	return static_cast<int32>(keys.size());
}

int32 Config::GetSectionName(std::vector<std::string> &sections) {
	STR_MAP_ITER it;
	for (it = config_map_.begin(); it != config_map_.end(); ++it) {
		sections.push_back(it->first);
	}
	return static_cast<int32>(sections.size());
}

std::string Config::ConfigToString() {
	std::string result;
	STR_MAP_ITER it;
	STR_STR_MAP_ITER it1;
	for (it = config_map_.begin(); it != config_map_.end(); ++it) {
		result += "[" + it->first + "]\n";
		for (it1 = it->second->begin(); it1 != it->second->end(); ++it1) {
			std::string s = it1->second.c_str();
			result += "    " + it1->first + " = " + s + "\n";
			if (s.size() != it1->second.size()) {
				char *data = const_cast<char*>(it1->second.data());
				char *p = NULL;
				for (int32 i = 0; i < static_cast<int32>(it1->second.size()); i++) {
					if (data[i] != '\0')
						continue;
					if (p)
						result += "    " + it1->first + " = " + p + "\n";
						p = data + i + 1;
				}
				if (p) {
					result += "    " + it1->first + " = " + p + "\n";
				}
			}
		}
	}
	result += "\n";
	return result;
}

} //base
