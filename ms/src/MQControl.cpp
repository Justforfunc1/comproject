#include "MQControl.h"

bool MQControl::init(const char* fname, const char* group_name)
{
	boost::property_tree::ptree pt;
	//读取配置文件
	try {
		boost::property_tree::ini_parser::read_ini(fname, pt);
	}catch(exception & e) {
		m_logger->warning("read profile %s[%s]failed!(%s)",fname,group_name,e.what());
		return false;
	}
	
	string groupName = group_name + string(".");
	try {
		m_moduleId = pt.get<string>(groupName+"name");
	} catch(exception & e) {
		m_logger->warning("profile %s lost[%s][%s]. use defalut value!",fname,group_name,"name");
		return false;
	}
	return true;
}

bool MQControl::init(const char *path)
{
	if ( path == NULL ) {
		path = MQ_PROFILE;
	}
	m_logger->init(path,"logging");
	//加载模块名
	if (!init(path, "local")) {
		return false;
	}
	//加载消息路径
	m_sendPaths.init(path,"sender");
	m_recvPaths.init(path,"receiver");
	m_conn.init(path,"mq_server");
	
	return m_conn.open();
}

int MQControl::send(const char* op,const char* content,const char* targetPath)
{
	MQSession session;
	if ( !session.init(m_conn.getConnection()) ) {
		return -10;
	}

	MsgPack pack(m_moduleId);
	pack.m_source = m_moduleId;
	pack.m_op = string(op);
	pack.m_content = string(content);
	
	//目标路径分析
	string toPath;
	if( targetPath == NULL || strcmp(targetPath,"") == 0 ){
		pack.m_target = ""; //后继路径
		string default_path = "DEFAULT";
		toPath = m_sendPaths.get(default_path);
		if (toPath == "" ) {
			m_logger->warning("mq_client_sendini is error!([sender] DEFALUT is not set)");
			session.close();
			return -6;
		}
	}else{ //指定路径
		string strTargetPath = targetPath;
		string pathTag;
		size_t pos=strTargetPath.find(";");
		if (pos!= string::npos) {
			pathTag = strTargetPath.substr(0, pos);
			pack.m_target = strTargetPath.substr(pos+1); //后继路径
		}else {
			pathTag = strTargetPath;
			pack.m_target = "";
		}
			
		if ( pathTag.c_str()[0] == '$' ){
			toPath = pathTag.substr(1);
		}else{
			toPath = m_sendPaths.get(pathTag);
		}
		
		if (toPath == "") {
			m_logger->warning("mq_client_send ini is error!([sender] %s is not set)",pathTag.c_str());
			session.close();
			return -6;
		}
	}

	int ret = session.send(toPath,pack);
	session.close();
 	m_logger->debug("mq_client_send[\n msgId=%s\n sendertime=%s\n source=%s\n target=%s\n op=%s\n content=%s\n]",
		pack.m_msgId.c_str(),pack.m_sendertime.c_str(),pack.m_source.c_str(),pack.m_target.c_str(),pack.m_op.c_str(),pack.m_content.c_str());

	return ret;
}

int MQControl::dialog(const char* op,const char* content,const char* targetPath,int timeout,char **respContent)
{
	MQSession session;
	if ( !session.init(m_conn.getConnection()) ) {
		return -10;
	}
	MsgPack pack(m_moduleId);
	pack.m_source = m_moduleId;
	pack.m_op = string(op);
	pack.m_content = string(content);
	pack.initMsgId();
	
	//回复队列名
	char addrName[300];
	sprintf(addrName,"#response-%s; {create:always, delete:always}",m_moduleId.c_str());
	Address respQ(addrName);

	pack.m_replyTo = "$"+respQ.getName();
	m_logger->debug("--->reply-to:%s",pack.m_replyTo.c_str());
	
	//目标路径分析
	string strTargetPath = targetPath;
	string pathTag;
	size_t pos=strTargetPath.find(";");
	if (pos!= string::npos) {
		pathTag = strTargetPath.substr(0, pos);
		pack.m_target = strTargetPath.substr(pos+1); //后继路径
	}else {
		pathTag = strTargetPath;
		pack.m_target = "";
	}
	
	string toPath;	
	if ( pathTag.c_str()[0] == '$' ){
		toPath = pathTag.substr(1);
	}else{
		toPath = m_sendPaths.get(pathTag);
	}
	
	if (toPath == "") {
		m_logger->warning("mq_dialog_wait ini is error([sender] %s is not set)",pathTag.c_str());
		session.close();
		return -6;
	}

	int ret = session.send(toPath,pack);
	if (ret != 0 ){
		session.close();
		return ret;
	}
	
	//接收
	MsgPack respPack;
	ret = session.receiveAddress(respQ, respPack, timeout);
	if (ret == 0){
		*respContent = strdup(respPack.m_content.c_str());
	}
	
	session.close();
 	m_logger->debug("dialog_wait is over!");
	return ret;
}

//侦听处理函数
void MQControl::handleMsg(string listenPath, mq_client *client) {
	MQSession session;
	session.init(m_conn.getConnection());
	m_pRecvSeesion = &session;
	
	bool isRun = true;
	while (isRun){
		MsgPack pack;
		try {
			m_logger->debug("mq_listener_add message listenning...");
			int ret = session.receive(listenPath, pack);
			if ( ret ==0 ) {
				m_logger->debug("mq_listener receive message[\n msgId=%s\n sendertime=%s\n source=%s\n target=%s\n op=%s\n content=%s\n]",
							pack.m_msgId.c_str(),pack.m_sendertime.c_str(),pack.m_source.c_str(),pack.m_target.c_str(),pack.m_op.c_str(),pack.m_content.c_str());
				client->exec_callback(pack.m_op.c_str(),pack.m_content.c_str(),pack.m_replyTo.c_str());
			}else if(ret ==1) {
				isRun = false;
			}
		}catch (...){
		}
	}
	session.close();
	m_pRecvSeesion = NULL;
}


int MQControl::add_listener(string listenPathTag, mq_client *client)
{
	string listenPath = m_recvPaths.get(listenPathTag);
	if ( listenPath == "" ){
		m_logger->warning("mq_client_listener argument is error!(listenPathTag[%s] is not exist)",listenPathTag.c_str());
		return -5;
	}
	
	try {
		m_pRecvThread = new boost::thread(boost::bind(&MQControl::handleMsg, this, listenPath, client));
		m_logger->info("mq_listener_add[%s] is over!",listenPathTag.c_str());
		sleep(1);
		return 0;
	} catch(...) {
		return -9;
	}
}

int MQControl::stop_listener()
{
	int ret;
	if (m_pRecvSeesion != NULL) {
		m_pRecvSeesion->stopReceive();
		try {
			bool isOver = m_pRecvThread->timed_join(boost::posix_time::microseconds(2000));
			if (!isOver)
				m_pRecvThread->interrupt();
			ret = 0;
		}catch(exception & e) {
			m_logger->warning("interrupt recv msg thread error![%s]",e.what());
			ret = -9;
		}
	}
	delete m_pRecvThread;
	m_pRecvThread = NULL;
	return ret;
}

//暂停侦听
int MQControl::pause_listener(bool flag)
{
	if (m_pRecvSeesion != NULL) {
		m_pRecvSeesion->pauseReceive(flag);
		if(flag)
			sleep(1);
	}
	return 0;
}

