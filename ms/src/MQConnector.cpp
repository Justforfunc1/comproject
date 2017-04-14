#include "MQConnector.h"

bool MQConnector::init(const char* fname, const char* group_name)
{
	boost::property_tree::ptree pt;
	//¶ÁÈ¡ÅäÖÃÎÄ¼þ
	try {
		boost::property_tree::ini_parser::read_ini(fname, pt);
	}catch(exception & e) {
		m_logger->warning("read profile %s[%s]failed!(%s)",fname,group_name,e.what());
		return false;
	}
	
	string groupName = group_name + string(".");
	
	string ip;
	try {
		ip = pt.get<string>(groupName+"ip");
	} catch(exception & e) {
		m_logger->warning("profile %s lost[%s][%s]. use defalut value!",fname,group_name,"ip");
	}

	string port;
	try {
		port = pt.get<string>(groupName+"port");
	} catch(exception & e) {
		m_logger->warning("profile %s lost[%s][%s]. use defalut value!",fname,group_name,"port");
	}
	m_connStr = ip +":" +port;
    //m_connStr = string("amqp://")+string(ip)+string(":")+string(port);
	//m_connStr += string("@")+string(user)+string(":")+string(passwd);
	
	try {
		m_option = pt.get<string>(groupName+"option");
	} catch(exception & e) {
		m_logger->warning("profile %s lost[%s][%s]. use defalut value!",fname,group_name,"option");
	}
    m_logger->debug("read profile %s[%s] is finished.",fname,group_name);
	return true;
}

bool MQConnector::open()
{
	m_logger->debug("connecting message queue[%s][%s].......",m_connStr.c_str(),m_option.c_str());
	m_pConn = new Connection(m_connStr, m_option);
	//m_pConn = new Connection(m_connStr);
	try	{
		m_pConn->open();
		m_logger->info("message queue succeed. [%s][%s]",m_connStr.c_str(),m_option.c_str());
		return true;
	} catch(const std::exception& error) {
		m_logger->warning("message queue failed.[%s][%s](%s)¡£",m_connStr.c_str(),m_option.c_str(),error.what());
		return false;
	}
}
		
void MQConnector::close(){
	if (m_pConn != NULL ){
		if(m_pConn->isValid())	
			m_pConn->close();
		delete m_pConn;
		m_pConn = NULL;
		m_logger->debug("message queue connection is closed.");
	}
}
