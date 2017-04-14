/**********************************************************
 *  \file MainRun.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author Allen.L
**********************************************************/
#ifndef _MainRun_H
#define _MainRun_H





class MainRun
{

private:

	//服务器id
	string m_server_id;

	//信号bind
	static void userSignals(int signo);
	//信号处理
	static void dealSignals(int signo);
	//信号监听
	void setSignals();

public:
	bool m_running;		//服务器状态
	bool m_isRun;		//某服务运行
	bool m_isExit;		//某服务退出

	int master;

	MainRun();

	//配置文件读取
	void config();

	//服务入口
	int run();
	//服务退出
	void destory();
	//服务停止
	void stopRun();

};


#endif

