#include "TargetParser.h"
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

map<string,string> TargetParser::m_opTable;
 
bool TargetParser::initOP(const char* fname, const char* group_name){
	boost::property_tree::ptree pt;
	//读取配置文件
	try {
		boost::property_tree::ini_parser::read_ini(fname, pt);
	}catch(exception & e) {
		m_logger->warning("read profile %s[%s]failed!(%s)",fname,group_name,e.what());
		return false;
	}
	
	try {
		ptree &subtree = pt.get_child(group_name);
		for(ptree::const_iterator iter = subtree.begin(); iter != subtree.end(); iter++  ) {
			string key = iter->first;
			string value = iter->second.get<string>("");
			boost::algorithm::trim(key);
			boost::algorithm::trim(value);
			m_opTable[key] = value;
		}
	} catch(exception & e) {
		m_logger->warning("profile %s lost[%s][%s]. use defalut value!",fname,group_name,"level");
	}
	return true;
}

vector<string> TargetParser::parse(string op)
{
	vector<string> targets;
	if( op == "" ){
		m_logger ->warning("OP is empty. cann't find path.");
		return targets;
	}

	map<string,string>::iterator iter = m_opTable.find(op);
	if(iter == m_opTable.end() ){
		m_logger ->warning("parse target path failed. OP[%s] is unknown。",op.c_str());
		return targets;
	}

	string path = iter->second;
	if( path == "" ){
		m_logger ->warning("parse target path failed. OP[%s]target path didn't set。",op.c_str());
		return targets;
	}

	//切分路径标识
	vector<string> pathTag;
	boost::split( pathTag, path, boost::is_any_of(","));
	for (unsigned int i = 0; i < pathTag.size(); i++) {
		string toPath = m_path.get(pathTag[i]);
		if ( toPath == "" ) {
			m_logger->warning("the pathTag[%s] in path[%s] is not exist!", pathTag[i].c_str(), path.c_str() );
		}else 
			targets.push_back(toPath);
	}
	return targets;
}

string TargetParser::parseTag(string op)
{
    if(op == "" ){
        m_logger ->warning("OP is empty. cann't find path.");
        return "";
    }

    map<string,string>::iterator iter = m_opTable.find(op);
    if(iter == m_opTable.end() ){
        m_logger ->warning("parse target path failed. OP[%s] is unknown。",op.c_str());
        return "";
    }
	
	string path = iter->second;
    if( path == "" ){
         m_logger ->warning("parse target path failed. OP[%s]target path didn't set。",op.c_str());
        return "";
    }
    return path;
}
