/**********************************************************
 *  \file ServerState.h
 *  \brief 全局运行函数
 *  \note	注意事项：
 *
 * \version
 * * \author Allen.L
**********************************************************/
#ifndef _ServerState_H
#define _ServerState_H

#define DAY_MODE 1  //日初模式
#define END_MODE 2  //日终模式
#define EMG_MODE 0	//紧急模式

#define LAST_START_TIME 10
#define	LAST_END_TIME (30*60)

class ServerState {
public
	int m_state;				//状态
	char m_chargeDate[20];		//时间
	long m_last_start_time;
	long m_last_end_time;

	ServerState();
	~ServerState();

	void config(const char* start_time, const char* end_time);
	void run();
};

#endif

