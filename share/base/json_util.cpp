/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file json_util.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-07-06
 ** \last modified 2017-07-19 18:44:53
**********************************************************/

#include "json_util.h"
#include "logger.h"
#include "string_util.h"

namespace base {
namespace utils {

std::string index_split_path_default = ".";
std::string index_split_key_default = ",";

JsonUtil::JsonUtil() {;}
JsonUtil::~JsonUtil() {;}

bool JsonUtil::ParseJson(const std::string &js_str, Json::Value &js_value) {
	Json::Reader reader;
	return reader.parse(js_str, js_value);
}

Json::Value JsonUtil::GetJsonValue(const Json::Value &js_value, const std::string &path) {
	Json::Value tmp = js_value;
	Json::Value *ptr_tmp = FindJsonValue(tmp, path);
	if (ptr_tmp != nullptr)
		return *ptr_tmp;
	else
		return Json::Value();
}

Json::Value JsonUtil::GetJsonValue(const Json::Value &js_array, const std::string &key, const std::string &value) {
	Json::Value tmp = js_array;
	Json::Value *ptr_tmp = FindJsonValue(tmp, key, value);
	if (ptr_tmp != nullptr)
		return *ptr_tmp;
	else
		return Json::Value();
}

Json::Value *JsonUtil::FindJsonValue(Json::Value &js_value, const std::string &path) {
	if (path.size() == 0)
		return &js_value;
	std::vector<std::string> keys;
	if (!ParseKeyPath(path, keys))
		return nullptr;
	std::string error_info;
	Json::Value *ptr_tmp = &js_value;
	for (std::vector<std::string>::iterator iter = keys.begin(); iter !=keys.end(); ++iter) {
		error_info += " key -->" + *iter +" ||";
		ptr_tmp = FindJsonValueByKey(*ptr_tmp, *iter);
		if (ptr_tmp == nullptr) {
			LOG_DEBUG("found a empty json value.key path is [%s]  already go [%s]", path.c_str(), error_info.c_str());
			return nullptr;
		}
	}
	return ptr_tmp;
}

Json::Value *JsonUtil::FindJsonValue(Json::Value &js_array, const std::string &key, const std::string &value) {
	if (js_array.isArray()) {
		long array_size = js_array.size();
		for (int32 i = 0; i < array_size; ++i) {
			Json::Value::Members keys = js_array[i].getMemberNames();
			for (Json::Value::Members::iterator iter = keys.begin(); iter != keys.end(); ++iter) {
				if (key == *iter) {
					if (js_array[i][*iter].isString() && js_array[i][*iter].asString() == value)
						return &js_array[i];
				}
			}
		}
	}
	return nullptr;
}

bool JsonUtil::ConvertJsonToMap(const Json::Value &js_value, std::map<std::string,std::string> &map_result, std::string key_list) {
	return true;
}

std::string JsonUtil::ConvertMapToJsonStr(const std::map<std::string,std::string> map) {
	return nullptr;
}

bool JsonUtil::ParseKeyPath(const std::string &path, std::vector<std::string> &keys) {
	return base::utils::StringUtil::SplitString(path, index_split_path_default, keys);
}

bool JsonUtil:: ParseKeyList(const std::string &key_list, std::vector<std::string> &keys) {
	if (!base::utils::StringUtil::SplitString(key_list, index_split_key_default, keys)) {
		LOG_DEBUG( "key list format is wrong.key list is [%s]", key_list.c_str());
		return false;
	}
	return true;
}

Json::Value *JsonUtil::FindJsonValueByKey(Json::Value &js_value, std::string &key) {
	Json::Value *ptr_tmp = &js_value;
	if (ptr_tmp->isString() || ptr_tmp->empty() || ptr_tmp->isNull()) {
		return nullptr;
	}
	if (ptr_tmp->isArray()) {
		if (!(base::utils::StringUtil::CheckNumber(key))) {
			LOG_DEBUG( "Key is not a number. Key name is [%s]", key.c_str());
			return nullptr;
		}
		int32 index = base::utils::StringUtil::StrToInt(key.c_str(), 0);
		if (index >= static_cast<int32>(ptr_tmp->size())) {
			return nullptr;
		}
		return &(*ptr_tmp)[index];
	} else {
		Json::Value::Members keys = ptr_tmp->getMemberNames();
		for (Json::Value::Members::iterator iter = keys.begin(); iter != keys.end(); ++iter) {
			if (key == *iter) {
				return &(*ptr_tmp)[key];
			}
		}
		return nullptr;
	}
}

} //utils
} //base
