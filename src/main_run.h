/**********************************************************
 * \file main_run.h
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/
#ifndef _MAIN_RUN_H
#define _MAIN_RUN_H

#include <stdio.h>
#include <signal.h>

class MainRun{

 public:
	bool running_;
	bool isRun_;
	bool isExit_;

	int master_;

	MainRun();

	void config();

	int run();
	void destory();
	void stopRun();


 private:
	string server_id_;
	ServerState *state_;

	static void userSignals(int signo);
	static void dealSignals(int signo);
	void setSignals();

};

#endif

