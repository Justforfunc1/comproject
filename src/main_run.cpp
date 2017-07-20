/**********************************************************
 * \file main_run.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include "main_run.h"


MainRun m;

MainRun::MainRun() :
	running_(false), is_run_(false), is_exit_(false), master_(0) {

	/*初始化日志时间、等级，生成当日日志文件
	*生成项目所需的对象
	*
	*/
}

void MainRun::Config() {

	/*
	 *初识化项目所需模块相关配置
	 *
	 *
	  */
	SetSignals();
	is_run_ = true;

}

void MainRun::UserSignals(int signo) {
	std::thread thr(std::bind(MainRun::DealSignals, signo));
	thr.join();
}

void MainRun::DealSignals(int signo) {
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

void MainRun::SetSignals() {
	if (signal(SIGRTMIN + 1, MainRun::UserSignals) == SIG_ERR) {
		printf("------>signal set error[signalNo=%d]!", SIGRTMIN + 1);
	}
	if (signal(SIGRTMIN + 2, MainRun::UserSignals) == SIG_ERR) {
		printf("------>signal set error[signalNo=%d]!", SIGRTMIN + 2);
	}
	if (signal(SIGRTMIN + 3, MainRun::UserSignals) == SIG_ERR) {
		printf("------>signal set error[signalNo=%d]!", SIGRTMIN + 3);
	}
	if (signal(SIGRTMIN + 4, MainRun::UserSignals) == SIG_ERR) {
		printf("------>signal set error[signalNo=%d]!", SIGRTMIN + 4);
	}
	if (signal(SIGRTMIN + 5, MainRun::UserSignals) == SIG_ERR) {
		printf("------>signal set error[signalNo=%d]!", SIGRTMIN + 5);
	}
	if (signal(SIGRTMIN + 6, MainRun::UserSignals) == SIG_ERR) {
		printf("------>signal set error[signalNo=%d]!", SIGRTMIN + 6);
	}
	if (signal(SIGRTMIN + 7, MainRun::UserSignals) == SIG_ERR) {
		printf("------>signal set error[signalNo=%d]!", SIGRTMIN + 7);
	}
}

void MainRun::Run() {
	is_run_ = true;
	while (is_run_) {
		/*
		 *xxxxxx
		 */
		sleep(10);
	}

	//*****
	is_run_ = false;
}


void MainRun::Destory() {

	/*
	 *
	 * 退出处理
	 * 记录日志
	 *
	 *
	 * */

}

void MainRun::StopRun() {
	is_run_ = false;
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
		std::cout << "xxx Server[" << pid << "] running...." << std::endl;
	}

	m.Config();

	while (!m.is_exit_) {
		if (m.is_run_)
			m.Run();
		else
			sleep(1);
	}

	m.Destory();
	return 0;
}


