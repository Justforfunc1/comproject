#include "MQCommon.h"
#include "MQPath.h"
#include "MsgPack.h"
#include "MQSession.h"
#include "MQConnector.h"
#include "TargetParser.h"

#include <time.h>
#include <sys/time.h>
#include <boost/thread/thread.hpp>
#include <boost/algorithm/string.hpp>

#define PROFILE "./mc.ini"
#define OP_PROFILE "./mc_op.ini"

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

//
void mc_handler(MsgPack& pack,TargetParser& parser,MQConnector& conn){
	MQLogger *logger = MQLogger::getInstance();
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
		sendPack(conn,toPath,pack);
	}else { //OP路由
		logger->debug("OP route message[%s]", pack.m_op.c_str());
		vector<string> pathList = parser.parse(pack.m_op);
		if ( pathList.size() == 0 ){
			logger->warning("there is no path, ignor the messsage!!!");
			return;
		}
		for(unsigned int i=0; i < pathList.size(); i++){
			//对每条路径
			logger->debug("[%s] route to %s",pack.m_op.c_str(), pathList[i].c_str() );
			sendPack(conn, pathList[i], pack);
		}
	}
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

	//加载消息路径
	MQPath recvPaths;
	recvPaths.init(PROFILE,"receiver");
	MQPath sendPaths;
	sendPaths.init(PROFILE,"sender");
	TargetParser parser(sendPaths);
	parser.initOP(OP_PROFILE,"op");

	string listenPath = recvPaths.get("DEFAULT");
	if ( listenPath == "" ){
		logger->warning("mc.ini error. (receiver[DEFAULT] is not exist! ) ");
		return -1;
	}
	
	MQConnector conn;
	conn.init(PROFILE,"mc_server");
	conn.open();
	MQSession session;
	if (!session.init(conn.getConnection())) {
		return -1;
	}
	
	bool isRun = true;
	while (isRun){
		MsgPack pack;
		try {
			logger->debug("mc start listenning...");
			int ret = session.receive(listenPath, pack);
			if ( ret ==0 ) {
				logger->debug("mc receive message[\n msgId=%s\n sendertime=%s\n source=%s\n target=%s\n op=%s\n content=%s\n]",
							pack.m_msgId.c_str(),pack.m_sendertime.c_str(),pack.m_source.c_str(),pack.m_target.c_str(),pack.m_op.c_str(),pack.m_content.c_str());
				mc_handler( pack, parser, conn);

				// 获取当前时间
				char currentDate[20] = "YYYYMMDD";
				formatTimeStr(currentDate);
				if(atoi(g_workingDate) != atoi(currentDate)) { // 时间不相同时
					// 切换日志 
					MQLogger::getInstance()->createLog();
					strcpy(g_workingDate,"YYYYMMDD");
					formatTimeStr(g_workingDate); // 初始化时间
				}

			}else if(ret ==1) {
				isRun = false; // 停止接收消息
			}
		}catch (...){
		
		}
	}// end while

	session.close();
	conn.close();
}


