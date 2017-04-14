/**********************************************************
 * \file GPConfig.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include "gp_base/GPConfig.h"
#include "gp_base/GPStringUtil.h"
using namespace std;


namespace GPBase {

static GPConfig _config;


GPConfig::GPConfig() {
}


GPConfig::~GPConfig() {
	for (STR_MAP_ITER it = m_configMap.begin(); it != m_configMap.end(); ++it) {
		delete it->second;
	}
}


//得到静态实例
GPConfig& GPConfig::getConfig() {
	return _config;
}


//解析字符串
GPInt32 GPConfig::parseValue(GPChar *str, GPChar *key, GPChar *val) {
	GPChar *p, *p1, *name, *value;

	if (str == NULL)
		return GP_ERROR;

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
		return GP_ERROR;
	p1 = strchr(str, '=');
	if (p1 == NULL)
		return GP_ERROR;
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
		return GP_ERROR;

	strcpy(key, name);
	strcpy(val, value);

	return GP_SUCCESS;
}

// 是段名
GPChar *GPConfig::isSectionName(GPChar *str) {
	if (str == NULL || strlen(str) <= 2 || (*str) != '[')
		return NULL;

	GPChar *p = str + strlen(str);
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


// 加载文件
GPInt32 GPConfig::load(const GPChar *filename) {
	FILE *fp;
	GPChar key[128], value[4096], data[4096];
	GPInt32 ret, line = 0;

	if ((fp = fopen(filename, "rb")) == NULL) {
		cout << "不能打开配置文件:" << filename << endl;
		return GP_ERROR;
	}

	STR_STR_MAP *m = NULL;
	while (fgets(data, 4096, fp)) {
		line++;
		GPChar *sName = isSectionName(data);
		// 是段名
		if (sName != NULL) {
			STR_MAP_ITER it = m_configMap.find(sName);
			if (it == m_configMap.end()) {
				m = new STR_STR_MAP();
				m_configMap.insert(STR_MAP::value_type(sName, m));
			} else {
				m = it->second;
			}
			continue;
		}
		ret = parseValue(data, key, value);
		if (ret == -2) {
			cout << "解析错误, Line:" << line << ", " << data << endl;
			fclose(fp);
			return GP_ERROR;
		}
		if (ret < 0) {
			continue;
		}
		if (m == NULL) {
			cout << "没在配置section, Line:" << line << ", " << data << endl;
			fclose(fp);
			return GP_ERROR;
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
	return GP_SUCCESS;
}


//取一个string
const GPChar *GPConfig::getString(const GPChar *section, const GPString& key, const GPChar *d) {
	STR_MAP_ITER it = m_configMap.find(section);
	if (it == m_configMap.end()) {
		return d;
	}
	STR_STR_MAP_ITER it1 = it->second->find(key);
	if (it1 == it->second->end()) {
		return d;
	}
	return it1->second.c_str();
}


 //取一string列表
vector<const GPChar*> GPConfig::getStringList(const GPChar *section, const GPString& key) {
	vector<const GPChar*> ret;
	STR_MAP_ITER it = m_configMap.find(section);
	if (it == m_configMap.end()) {
		return ret;
	}
	STR_STR_MAP_ITER it1 = it->second->find(key);
	if (it1 == it->second->end()) {
		return ret;
	}
	const GPChar *data = it1->second.data();
	const GPChar *p = data;
	for (GPInt32 i = 0; i < (GPInt32) it1->second.size(); i++) {
		if (data[i] == '\0') {
			ret.push_back(p);
			p = data + i + 1;
		}
	}
	ret.push_back(p);
	return ret;
}


 //取一整型
GPInt32 GPConfig::getInt(const GPChar *section, const GPString& key, GPInt32 d) {
	const GPChar *str = getString(section, key);
	return GPUniversal::GPStringUtil::strToInt(str, d);
}

//取一int list
GPInt32Vector GPConfig::getIntList(const GPChar *section, const GPString& key) {
	GPInt32Vector ret;
	STR_MAP_ITER it = m_configMap.find(section);
	if (it == m_configMap.end()) {
		return ret;
	}
	STR_STR_MAP_ITER it1 = it->second->find(key);
	if (it1 == it->second->end()) {
		return ret;
	}
	const GPChar *data = it1->second.data();
	const GPChar *p = data;
	for (GPInt32 i = 0; i < (GPInt32) it1->second.size(); i++) {
		if (data[i] == '\0') {
			ret.push_back(atoi(p));
			p = data + i + 1;
		}
	}
	ret.push_back(atoi(p));
	return ret;
}

// 取一section下所有的key
GPInt32 GPConfig::getSectionKey(const GPChar *section, GPStringVector &keys) {
	STR_MAP_ITER it = m_configMap.find(section);
	if (it == m_configMap.end()) {
		return GP_ERROR;
	}
	STR_STR_MAP_ITER it1;
	for (it1 = it->second->begin(); it1 != it->second->end(); ++it1) {
		keys.push_back(it1->first);
	}
	return (GPInt32) keys.size();
}

// 得到所有section的名字
GPInt32 GPConfig::getSectionName(GPStringVector &sections) {
	STR_MAP_ITER it;
	for (it = m_configMap.begin(); it != m_configMap.end(); ++it) {
		sections.push_back(it->first);
	}
	return (int) sections.size();
}

// toString
GPString GPConfig::toString() {
	GPString result;
	STR_MAP_ITER it;
	STR_STR_MAP_ITER it1;
	for (it = m_configMap.begin(); it != m_configMap.end(); ++it) {
		result += "[" + it->first + "]\n";
		for (it1 = it->second->begin(); it1 != it->second->end(); ++it1) {
			GPString s = it1->second.c_str();
			result += "    " + it1->first + " = " + s + "\n";
			if (s.size() != it1->second.size()) {
				GPChar *data = (GPChar*) it1->second.data();
				GPChar *p = NULL;
				for (GPInt32 i = 0; i < (GPInt32) it1->second.size(); i++) {
					if (data[i] != '\0')
						continue;
					if (p)
						result += "    " + it1->first + " = " + p + "\n";
					p = data + i + 1;
				}
				if (p)
					result += "    " + it1->first + " = " + p + "\n";
			}
		}
	}
	result += "\n";
	return result;
}

}

