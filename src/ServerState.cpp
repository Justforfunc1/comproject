/**********************************************************
 *  \file ServerState.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author Allen.L
**********************************************************/
#include "ServerState.h"

ServerState::ServerState() {
	m_state = END_MODE;
	strcpy(m_chargeDate, "YYYYMMDD");
	m_last_start_time = LAST_START_TIME;
	m_last_end_time = LAST_END_TIME;
}

ServerState::~ServerState() {
}

//获取当前unix时间戳(秒)
long getSeconds() {
	time_t rawtime;
	struct tm* curtime;
	time(&rawtime);
	curtime = localtime(&rawtime);
	return (curtime->tm_hour * 3600 + curtime->tm_min * 60 + curtime->tm_sec);
}

//当前服务器状态检查
void ServerState::run() {
	long ctime = getSeconds();

	if ((ctime - m_last_start_time) > 0 && (ctime - m_last_end_time) <= 60) { //日终时间段
		char tmp[20];
		strcpy(tmp, "YYYYMMDD");
		if (strcmp(m_chargeDate, tmp) != 0 && m_state >= DAY_MODE) { //前日，启动日终过程
			m_state = END_MODE;  //切换至日终
			/*日志记录*/
		}
		if (strcmp(m_chargeDate, tmp) != 0 && (ctime - m_last_end_time) > 0 && m_state != EMG_MODE) { //未进入当日
			m_state = EMG_MODE;	//切换至紧急
			/*日志记录*/
		}
	}
}

//配置参数
void ServerState::config() {

}
