#include "MQCommon.h"
#include "MQPath.h"
#include "MsgPack.h"
#include "MQSession.h"
#include "MQConnector.h"
#include "TargetParser.h"

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <boost/thread/thread.hpp>
#include <boost/algorithm/string.hpp>

#define PROFILE "./ms.ini"
#define OP_PROFILE "./ms_op.ini"

char g_workingDate[20];

//发送
int sendPack(MQConnector& conn, string toPath, MsgPack& msgpack){
	MQSession session;
	if (!session.init(conn.getConnection())) {
		return -1;
	}

	int ret = session.send(toPath, msgpack);
	session.close();
	return ret;
}

//MC处理函数
void mc_handler(MsgPack& pack,TargetParser& parser, MQConnector& msConn){
	MQLogger* logger = MQLogger::getInstance();
	if (pack.m_replyTo != "") { //回复路径修正
		string path =parser.m_path.get("MS");
		char tmp[500];
		sprintf(tmp,"$%s;$%s",path.c_str(),pack.m_replyTo.c_str());
		pack.m_replyTo = string(tmp);
	}

	if (pack.m_target != "") { //指定路径
		logger->debug("designated path message[%s][%s]",pack.m_op.c_str(),pack.m_target.c_str());
		
		string pathTag;
		size_t found = pack.m_target.find_first_of(";");
		if (found==string::npos) {
			pathTag = pack.m_target;
			pack.m_target = ""; //后继路径
		} else {
			pathTag = pack.m_target.substr(0,found);
			pack.m_target = pack.m_target.substr(found+1); //后继路径
		}
		
		string toPath = parser.m_path.get(pathTag);
		if (toPath == "" ) {
			logger->warning("mc.ini is error!([Sender] %s is not set)!", pathTag.c_str());
			return;
		}
		logger->debug("[%s]is route to [%s]%s",pack.m_op.c_str(), pathTag.c_str(), toPath.c_str());
		//mc来的消息只发ms server的mq
		sendPack(msConn, toPath, pack);
	}else { //OP路由
		logger->debug("OP route message[%s]", pack.m_op.c_str());
		vector<string> pathList = parser.parse(pack.m_op);
		if ( pathList.size() == 0 ){
			logger->warning("there is no path, ignor the messsage!!!");
		}
		for(unsigned int i=0; i < pathList.size(); i++){
			//对每条路径
			logger->debug("[%s] route to %s",pack.m_op.c_str(), pathList[i].c_str() );
			sendPack(msConn, pathList[i], pack);
		}
	}
}

//MC侦听函数
void mc_listener( MQSession* pMcSess, MQConnector* pMsConn)
{
	MQSession& mcSession = *pMcSess;
	MQConnector& msConn = *pMsConn;
	MQLogger* logger = MQLogger::getInstance();
	//加载MC\MS消息路径		
	MQPath mcRecvPaths;
	mcRecvPaths.init(PROFILE,"mc_receiver");
	MQPath msSendPaths;
	msSendPaths.init(PROFILE,"ms_sender");
	
	TargetParser parser(msSendPaths);
	parser.initOP(OP_PROFILE,"op");

	string path = mcRecvPaths.get("DEFAULT");
	if ( path == "" ){
		logger->warning("ms.ini error. (mc_receiver[DEFAULT] is not exist! ) ");
		return;
	}

	bool isRun = true;
	while (isRun){
		MsgPack pack;
		try {
			logger->debug("ms listenning mc ...");
			int ret = mcSession.receive(path, pack);
			while((msConn.getConnection()->isOpen())==false) {
				logger->debug("MS->MC connection is break. try to rebuilding ...");
				sleep(5);
			}
			if ( ret ==0 ) {
				logger->debug("MS recv MC message[\n msgId=%s\n sendertime=%s\n source=%s\n target=%s\n reply-to=%s\n op=%s\n content=%s\n]",
							pack.m_msgId.c_str(),pack.m_sendertime.c_str(),pack.m_source.c_str(),pack.m_target.c_str(),pack.m_replyTo.c_str(),pack.m_op.c_str(),pack.m_content.c_str());
				mc_handler(pack, parser,  msConn);
			}else if(ret ==1) {
				isRun = false;
			}
		}catch (...){
		}
	}
	mcSession.close();
}



//MS处理函数
void ms_handler(MsgPack& pack,TargetParser &parser, MQPath& mcSendPaths, MQPath& msSendPaths, MQConnector& mcConn, MQConnector& msConn){
	MQLogger *logger = MQLogger::getInstance();
	
	if (pack.m_target != "") { //指定路径
		logger->debug("designated path message[op=%s][target=%s]",pack.m_op.c_str(),pack.m_target.c_str());
		string pathTag;
		size_t found = pack.m_target.find_first_of(";");
		if (found==string::npos) {
			pathTag = pack.m_target;
			pack.m_target = ""; //后继路径
		} else {
			pathTag = pack.m_target.substr(0,found);
			pack.m_target = pack.m_target.substr(found+1); //后继路径
		}
		
		string toPath;
		bool isMC = false;
		if ( pathTag.size() >=1 && pathTag.at(0) == '$' ){
			if ( pathTag.size() >=2 && pathTag.at(1)=='$' ) {
				isMC = true;
				toPath = pathTag.substr(2);
			}else{
				toPath = pathTag.substr(1);
			}
		}else{
			toPath = msSendPaths.get(pathTag);
			if ( toPath == "" ) {
				logger->warning("mc.ini is innormal([sender] part %s is not set)!",pathTag.c_str());
				return;
			}
		}
		
		if (isMC){
			sendPack( mcConn, toPath, pack);
		}else{
			sendPack( msConn, toPath, pack);
		}
	}else { //OP路由
		logger->debug("OP route message [%s]",pack.m_op.c_str());
		string path = parser.parseTag(pack.m_op);
		if ( path!= "" ){
			//切分路径标识
			vector<string> pathTag;
			boost::split( pathTag, path, boost::is_any_of(","));
			for (unsigned int i = 0; i < pathTag.size(); i++) {
				//ms解析
				string toPath = msSendPaths.get(pathTag[i]);
				if( toPath != "" ){
					sendPack( msConn, toPath, pack);
					return;
				}
				//mc解析
				toPath = mcSendPaths.get(pathTag[i]);
				if( toPath!="" ){
					sendPack( mcConn, toPath, pack);
				} else {
					logger ->warning("parse target path failed. OP[%s] target path TAG[%s] not set.", pack.m_op.c_str(), toPath.c_str());
				}
			}
		}else{
			logger->warning("no path exist, ignor the message!!!");
		}
	}
}

//MS侦听函数
void ms_listener( MQSession* pMsSess, MQConnector* pMcConn, MQConnector* pMsConn)
{
	MQSession& msSession = *pMsSess;
	MQConnector& mcConn = *pMcConn;
	MQConnector& msConn= *pMsConn;
	MQLogger *logger = MQLogger::getInstance();
	
	//加载MC\MS消息路径		
	MQPath msRecvPaths;
	msRecvPaths.init(PROFILE,"ms_receiver");
	
	MQPath mcSendPaths;
	mcSendPaths.init(PROFILE,"mc_sender");
	MQPath msSendPaths;
	msSendPaths.init(PROFILE,"ms_sender");
	
	TargetParser parser(msSendPaths);
	parser.initOP(OP_PROFILE,"op");

	string path = msRecvPaths.get("DEFAULT");
	if ( path == "" ){
		logger->warning("ms.ini is error( ms_receiver[DEFAULT] is not exist!)");
		return;
	}

	bool isRun = true;
	while (isRun){
		MsgPack pack;
		try {
			logger->debug("MS listenning message from MC ...");
			int ret = msSession.receive(path, pack);
			if ( ret ==0 ) {
				logger->debug("MS receive message [\n msgId=%s\n sendertime=%s\n source=%s\n target=%s\n reply-to=%s\n op=%s\n content=%s\n]",
							pack.m_msgId.c_str(),pack.m_sendertime.c_str(),pack.m_source.c_str(),pack.m_target.c_str(),pack.m_replyTo.c_str(),pack.m_op.c_str(),pack.m_content.c_str());
				ms_handler(pack, parser, mcSendPaths, msSendPaths, mcConn, msConn);
			}else if(ret ==1) {
				isRun = false;
			}
		}catch (...){
		}
	}
	msSession.close();
}

// 获取当前时间
void formatTimeStr(char *buf) {
	time_t rawtime;
	struct tm* curtime;

	time(&rawtime);
	curtime = localtime(&rawtime);

	char tmp[10];
	char *p = strstr(buf,"YYYY");
	if ( p !=NULL ){
		sprintf(tmp,"%04d",curtime->tm_year + 1900);
		strncpy(p,tmp,4);
	}

	p = strstr(buf,"MM");
	if ( p !=NULL ){
		sprintf(tmp,"%02d",curtime->tm_mon + 1);
		strncpy(p,tmp,2);
	}

	p = strstr(buf,"DD");
	if ( p !=NULL ){
		sprintf(tmp,"%02d", curtime->tm_mday);
		strncpy(p,tmp,2);
	}
	
	p = strstr(buf,"HH");
	if ( p !=NULL ){
		sprintf(tmp,"%02d", curtime->tm_hour);
		strncpy(p,tmp,2);
	}

	p = strstr(buf,"MI");
	if ( p !=NULL ){
		sprintf(tmp,"%02d", curtime->tm_min);
		strncpy(p,tmp,2);
	}
	
	p = strstr(buf,"SS");
	if ( p !=NULL ){
		sprintf(tmp,"%02d",curtime->tm_sec);
		strncpy(p,tmp,2);
	}
}

int main(int argc, char **argv)
{
	MQLogger *logger = MQLogger::getInstance();
	logger->init(PROFILE,"logging");
	strcpy(g_workingDate,"YYYYMMDD");
	formatTimeStr(g_workingDate); // 初始化时间

	MQConnector mcConn;
	mcConn.init(PROFILE,"mc_server");
	mcConn.open();
	MQSession mcSession;
	if (!mcSession.init(mcConn.getConnection())) {
		mcConn.close();
		return -1;
	}

	MQConnector msConn;
	msConn.init(PROFILE,"ms_server");
	msConn.open();
	MQSession msSession;
	if (!msSession.init(msConn.getConnection())) {
		msConn.close();
		mcConn.close();
		return -1;
	}
	
	boost::thread mcThread(boost::bind(mc_listener, &mcSession, &msConn));
	boost::thread msThread(boost::bind(ms_listener, &msSession, &mcConn, &msConn));

	while(1){
		sleep(100);

		// 获取当前时间
		char currentDate[20] = "YYYYMMDD";
		formatTimeStr(currentDate);
		if(atoi(g_workingDate) != atoi(currentDate)) { // 时间不相同时
			// 切换日志 
			MQLogger::getInstance()->createLog();
			strcpy(g_workingDate,"YYYYMMDD");
			formatTimeStr(g_workingDate); // 初始化时间
		}
	}

	msConn.close();
	mcConn.close();
}


