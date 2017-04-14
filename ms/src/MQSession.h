
#ifndef _MQSession_H_
#define _MQSession_H_

#pragma once

#include "MQCommon.h"
#include "MsgPack.h"
#include "MQPath.h"


class MQSession:public MQLogging {
	Session m_session;
	Connection *m_pConn;
	MQPath *mqpath;
	const static int SECD = 2*1000; 
	bool m_isRecv;
	bool m_isPause;
	bool m_isCreated;
	
public:
	MQSession():m_isRecv(true),m_isPause(false) {}
	~MQSession() { close();}

	bool init(Connection *conn);
	void close();
	int send(const string& mqName, MsgPack& msgpack);
	int receive(const string& mqName, MsgPack& msgpack, int timeout = 0);
	int receiveAddress(Address &responseQueue, MsgPack& msgpack, int timeout);
	void stopReceive();
	void pauseReceive(bool flag);
};

#endif


