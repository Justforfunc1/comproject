/**********************************************************
 *  \file MainRun.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author Allen.L
**********************************************************/

#include "MainRun.h"

MainRun m;

MainRun::MainRun() :
	m_running(false), m_isRun(false), m_isExit(false),master(0)	{

	/*初始化日志时间、等级，生成当日日志文件
	*生成项目所需的对象
	*
	*/
}

viod MainRun::config() {

	/*
	 *初识化项目所需模块相关配置
	 *
	 *
	  */
	setSignals();
	m_isRun = ture;

}

void MainRun::userSignals() {
	boost::thread thr(boost::bind(MainRun::dealSignals, signo)));
	thr.join();
}

void MainRun::dealSignals(int signo) {
	switch(signo)	{
		case:
		{
			//*****
		}
	}
}

void MainRun::setSignals() {

	if (signal(SIGRTMIN + 1, MainRun::userSignals) == SIG_ERR) {
		PSLOG_ERROR("------>signal set error[signalNo=%d]!", SIGRTMIN + 1);
	}
	/*
	 *
	 *xxxxxxx
	 *
	 * */
}

void MainRun::run()
{

	m_isRun = ture;

	while(m_isRun)
	{
		/*
		 *xxxxxx
		 */
		sleep(10);
	}

	//*****
	m_isRun = false;

	return 0;

}


void MainRun::destory() {

	/*
	 *
	 * 退出处理
	 * 记录日志
	 *
	 *
	 * */

}

void MainRun::stopRun() {

	m_isRun = false;

}

void help()
{
	cout << "\t -h  help." << endl;
	/*
	 *xxxxxxxxxx
	 * */
}

int main(int args, char **argv)	{

	int pid = (int) getpid();
	if(args > 1) {

		if(strcmp(argv[1], "-h") == 0) {
			help();
			return 0;
		}
		else if(strcmp(argv[1], "") == 0) {
			//*****
		}
	} else {
			cout << "xxx Server[" << pid << "] running...." << endl;
	}

	m.config();

	while(!m.m_isExit) {
		if(m.m_isRun)
			m.run();
		else
			sleep(1);
	}

	m.destory();

	return 0;

}

















