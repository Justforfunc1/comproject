#ifndef _MQCONTROL_H_
#define _MQCONTROL_H_

#pragma once

#include <boost/thread/thread.hpp>
#include "MQCommon.h"
#include "MQPath.h"
#include "MsgPack.h"
#include "MQSession.h"
#include "MQConnector.h"
#include "MQControl.h"
#include "mqclient2.h"

#define MQ_PROFILE "./mqclient.ini"

class MQControl:public MQLogging {
protected:
	MQPath m_sendPaths;
	MQPath m_recvPaths;
	string m_moduleId;
	
	MQConnector m_conn;
	MQSession *m_pRecvSeesion;

	//boost::mutex m_mutex;
	boost::thread* m_pRecvThread;
	
	bool init(const char* fname, const char* group_name);
	void handleMsg(string listenPath, mq_client *client);
	
public:
	MQControl():m_pRecvSeesion(NULL),m_pRecvThread(NULL){}

	bool init(const char *path);
	int send(const char* op,const char* content,const char* targetPath);
	int dialog(const char* op, const char* content, const char* targetPath, int timeout, char **respContent);
	int add_listener(string listenPathTag, mq_client *client);
	int stop_listener();
	int pause_listener(bool flag);
};

#endif
