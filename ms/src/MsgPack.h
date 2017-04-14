
#ifndef _MQPACK_H_
#define _MQPACK_H_
#pragma once

#include "MQCommon.h"

class MsgPack:public MQLogging {

	string m_moduleName;
public:
	string m_msgId;
	string m_sendertime;
	string m_source;
	string m_target;
	string m_op;
	string m_content;
	string m_replyTo;
	
private:
	void initSenderTime();
public:
	MsgPack() {};
	MsgPack(string &moduleName);
	void fromMessage(Message &message);
	void initMsgId();
	void toMessage(Message &message);
};

#endif
