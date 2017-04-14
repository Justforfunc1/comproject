#ifndef _MQPATH_H_
#define _MQPATH_H_

#pragma once


#include "MQCommon.h"

class MQPath:public MQLogging {
	map<string,string> m_table;
public:
	bool init(const char* fname, const char* group_name);
	string get(const string pathTag);
};

#endif


