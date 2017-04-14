#include "MQSession.h"

bool MQSession::init(Connection *conn)
{
	try {
		m_pConn = conn;
		m_session = m_pConn->createSession();
		if ( m_session.isValid() ){
			return true;
		} else {
			m_logger->warning("create Session failed(m_session is null).");
			return false;
		}
	} catch(const qpid::types::Exception& error) {
		m_logger->warning("create Session failed(%s).",error.what());
		return false;
	}
}

void MQSession::close()
{
	try {
    	m_session.close();
    	m_logger->debug("the session is closed.");
 	} catch(const qpid::types::Exception& error) {			
		m_logger->warning("closing session falied(%s).",error.what());
	}
}

int MQSession::send(const string& mqName, MsgPack &msgpack)
{
	Message msg;
	msgpack.toMessage(msg);
	Sender sender;
	int ret = 0;
	if(! m_session.isValid() ){
		m_logger->warning("send msg failed!(session not exist)");
		return -3;
	}
	try {
		sender = m_session.createSender(mqName);
		sender.send(msg, true);
		m_logger->debug("send msg [%s] to (%s).", msgpack.m_op.c_str(), mqName.c_str());
 	} catch(const qpid::types::Exception& error) {
		m_logger->warning("send msg failed![%s](%s).", mqName.c_str(), error.what());
		ret = -2; //与mqclient.h中宏对应
	}
	if(!sender.isNull()) {
		sender.close();
	} else {
		ret=-2;
	}
	return ret;
}

int MQSession::receive(const string& mqName, MsgPack& msgpack, int timeout)
{
	m_logger->debug("start message listenning...");
	Receiver receiver;
	int ret = 0;
	
	if(!(m_session.isValid())){
		m_logger->warning("message receive failed!session is NULL!");
		return -3;
	}
			
	try {	
		receiver = m_session.createReceiver(mqName);
        if(!receiver.isValid()){
        	m_logger->warning("createReceiver is error!");
			//return -3;
        }
        Message msg;
        bool isRecv = false;
        bool isTimeout = false;
        
        int cnt = 0;
        if (timeout>0){
        	int s = timeout*1000/MQSession::SECD;
			cnt = (s>1)?s:1; 
	    }
        while (m_isRecv && !isRecv && !isTimeout ){
			if(m_isPause) {
				sleep(1);
				continue;
			}
			isRecv = receiver.fetch(msg,Duration(MQSession::SECD));
			if( cnt > 1){
				cnt--;
			}
			if(cnt==1){
				isTimeout = true;
			}
		}
		if (isRecv) {
			msgpack.fromMessage(msg);
			m_session.acknowledge();
		}else if (!m_isRecv){
			ret = 1;
		}else {
			ret = -7; //与mqclient.h中宏对应
		}
    } catch(const qpid::types::Exception& error) {
        m_logger->warning("message receive failed[%s](%s).", mqName.c_str(), error.what());
        ret = -3; //与mqclient.h中宏对应
    }
	
    receiver.close();

	return ret;
}

int MQSession::receiveAddress(Address &responseQueue, MsgPack& msgpack, int timeout)
{
	m_logger->debug("start message listenning...");
	Receiver receiver;
	int ret = 0;
	
	try {
        receiver = m_session.createReceiver(responseQueue);
        Message msg;
        bool isRecv = false;
        bool isTimeout = false;
        
        int cnt = 0;
        if (timeout>0){
        	int s = timeout*1000/MQSession::SECD;
			cnt = (s>1)?s:1; 
	    }
        while (m_isRecv && !isRecv && !isTimeout){
			isRecv = receiver.fetch(msg,Duration(MQSession::SECD));
			if( cnt > 1){
				cnt--;
			}
			if(cnt==1){
				isTimeout = true;
			}
		}
		if (isRecv) {
			msgpack.fromMessage(msg);
			m_session.acknowledge();
			m_logger->debug("receive message.");
			ret = 0;
		}else if (!m_isRecv){
			ret = 1;
			m_logger->debug("pasue receive.");
		}else {
			ret = -7; //与mqclient.h中宏对应
			m_logger->debug("timeout cause stop receive message.");
		}
    } catch(const qpid::types::Exception& error) {
        m_logger->warning("receive message failed.[%s](%s).", responseQueue.getName().c_str(), error.what());
        ret = -3; //与mqclient.h中宏对应
    }
    if(receiver.isValid()) {
		receiver.close();
    }else{
		ret=-3;
    }
	return ret;
}

void MQSession::stopReceive()
{
	m_logger->debug("stop receive message");
	m_isRecv = false;
}

void MQSession::pauseReceive(bool flag)
{
	if (flag) {
		m_logger->debug("pause receive message!");
	}else {
		m_logger->debug("restart receive message!");
	}
	m_isPause = flag;
}
