#include "MsgPack.h"
//#include <pthread.h>

MsgPack::MsgPack(string &moduleName)
{
	m_moduleName = moduleName;
	m_replyTo = "";
}

void MsgPack::fromMessage(Message &message)
{
	Variant::Map cont;
	decode(message, cont);
	m_msgId = cont["msgId"].asString();
	m_sendertime = cont["sendertime"].asString();
	m_source = cont["source"].asString();
	m_target = cont["target"].asString();
	m_content = cont["content"].asString();
	m_op = cont["op"].asString();
	m_replyTo = cont["reply-to"].asString();
}

void MsgPack::initSenderTime()
{
	struct tm *ptm; 
	char timeday[20];
	long ts;
	ts =  time( NULL );  
	ptm = localtime( &ts );
	sprintf(timeday,"%04d-%02d-%02d %02d:%02d:%02d", 
	      ptm -> tm_year + 1900, ptm->tm_mon + 1, ptm -> tm_mday,  
	      ptm -> tm_hour,  ptm->tm_min, ptm->tm_sec);
	m_sendertime = timeday;
        
}

void MsgPack::initMsgId()
{
	char buf[100];
	long ts =  time( NULL );  

	pthread_t tid = pthread_self();
	sprintf(buf,"%lu-%ld", tid,ts);
	      
	m_msgId = m_moduleName + "-"+string(buf);
}

void MsgPack::toMessage(Message &message)
{
	if ( m_sendertime.empty()){
		initSenderTime();
	}
	if (m_msgId.empty()){
		initMsgId();
	}
		
	Variant::Map content;
	content["msgId"] = m_msgId;
	content["sendertime"] = m_sendertime;
	content["source"] = m_source;
	content["target"] = m_target;
	content["op"] = m_op;
	content["content"] = m_content;
	if ( m_replyTo != "" ) {
		content["reply-to"] = m_replyTo;
	}
	encode(content, message);
	if(m_op!="QUERY") {
		message.setDurable(true);
	}
}
