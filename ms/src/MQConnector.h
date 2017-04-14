
#ifndef _MQCONNECTION_H_
#define _MQCONNECTION_H_

#pragma once

#include "MQCommon.h"

class MQConnector:public MQLogging {
private:
	Connection *m_pConn;
	string m_connStr;
	string m_option;
public:
	~MQConnector() { close(); };

	bool init(const char* fname, const char* group_name);
	bool open();
	void close();
	inline Connection *getConnection() { return m_pConn; };
};

#endif
