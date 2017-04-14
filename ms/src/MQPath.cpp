#include "MQPath.h"
#include <boost/algorithm/string/trim.hpp>

bool MQPath::init(const char* fname, const char* group_name)
{
	boost::property_tree::ptree pt;
	//¶ÁÈ¡ÅäÖÃÎÄ¼þ
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
			m_table[key] = value;
		}
	} catch(exception & e) {
		m_logger->warning("profile %s lost[%s][%s]. use defalut value!",fname,group_name,"level");
	}
	return true;
}

string MQPath::get(const string pathTag){
	map<string,string>::iterator iter = m_table.find(pathTag);
	if (iter != m_table.end() ) {
		return iter->second;
	}else{
		return "";
	}
}
