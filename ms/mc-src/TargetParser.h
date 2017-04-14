
#ifndef _TARGETPARSER_H_
#define _TARGETPASER_H_

#include <vector>
#include "MQPath.h"

class TargetParser:public MQLogging{
private:
    static map<string,string> m_opTable;
public:
	MQPath& m_path;
public:
	TargetParser(MQPath& path):m_path(path) {};
    bool initOP(const char* fname, const char* group_name);
    vector<string> parse(string op);
    string parseTag(string op);
};

#endif
