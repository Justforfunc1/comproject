/**********************************************************
 * \file main_run.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include "MainRun.h"


constexpr MainRun m;

MainRun::MainRun() :
	running_(false), isRun_(false), isExit_(false), master_(0)	{

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
	isRun_ = ture;

}

void MainRun::userSignals() {
	boost::thread thr(boost::bind(MainRun::dealSignals, signo)));
	thr.join();
}

void MainRun::dealSignals(int signo) {
	switch (signo) {
		case 35:  {
			break;
		}
		case 36:  {
			break;
		}
		case 40:  {
			break;
		}
	}
}

void MainRun::setSignals() {
	if (signal(SIGRTMIN + 1, MainRun::userSignals) == SIG_ERR) {
		printf("------>signal set error[signalNo=%d]!", SIGRTMIN + 1);
	}
	if (signal(SIGRTMIN + 2, MainRun::userSignals) == SIG_ERR) {
		printf("------>signal set error[signalNo=%d]!", SIGRTMIN + 2);
	}
	if (signal(SIGRTMIN + 3, MainRun::userSignals) == SIG_ERR) {
		printf("------>signal set error[signalNo=%d]!", SIGRTMIN + 3);
	}
	if (signal(SIGRTMIN + 4, MainRun::userSignals) == SIG_ERR) {
		printf("------>signal set error[signalNo=%d]!", SIGRTMIN + 4);
	}
	if (signal(SIGRTMIN + 5, MainRun::userSignals) == SIG_ERR) {
		printf("------>signal set error[signalNo=%d]!", SIGRTMIN + 5);
	}
	if (signal(SIGRTMIN + 6, MainRun::userSignals) == SIG_ERR) {
		printf("------>signal set error[signalNo=%d]!", SIGRTMIN + 6);
	}
	if (signal(SIGRTMIN + 7, MainRun::userSignals) == SIG_ERR) {
		printf("------>signal set error[signalNo=%d]!", SIGRTMIN + 7);
	}
}

void MainRun::run() {
	isRun_ = ture;
	while (isRun_) {
		/*
		 *xxxxxx
		 */
		sleep(10);
	}

	//*****
	isRun_ = false;
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
	isRun_ = false;
}

void help() {
	std::cout << "XXX Server usage:" << std::endl;
	std::cout << "\t -h  help." << std::endl;
	std::cout << "\t -->signal: " << std::endl;
	std::cout << "\t\t 35  pause supply." << std::endl;
	std::cout << "\t\t 36  restart supply." << std::endl;
	std::cout << "\t\t 40  stop program." << std::endl;
}

int main(int args, char **argv)	{
	int pid = (int) getpid();
	if (args > 1) {
		if (strcmp(argv[1], "-h") == 0) {
			help();
			return 0;
		}
		else if (strcmp(argv[1], "") == 0) {
			//*****
		}
	} else {
			cout << "xxx Server[" << pid << "] running...." << endl;
	}

	m.config();

	while (!m.m_isExit) {
		if (m.m_isRun)
			m.run();
		else
			sleep(1);
	}

	m.destory();
	return 0;
}


