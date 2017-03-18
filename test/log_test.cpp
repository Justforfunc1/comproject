/**********************************************************
 * \file log_test.cpp
 * \brief
 * \note	注意事项： 
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include "gp_base/GPBase.h"

#define INIT_FILE "../conf/log_test.ini"

using namespace gpbase;
using namespace std;

int main(int args, char* argv[])
{
	GPCONFIG_LOAD(INIT_FILE);
	string log_file = string(GPCONFIG_GETSTR("logging", "logger", "log_YYYYMMDD.log"));
	cout << "log_path:" << log_file.c_str() << endl;
	string log_level = string(GPCONFIG_GETSTR("logging", "level", "debug"));
	cout << "log_level:" << log_level.c_str() << endl;
	GPBASE_GPLOGGER_INIT(log_file, log_level, true);

	int i=10;

	do
	{
		cout << i << endl;
		GPLOG_INFO("------>TEST startup.....");
		GPLOG_ERROR("------>TEST errno.....");
		GPLOG_DEBUG("------>TEST debug.....");
		GPLOG_WARNING("------>TEST warning.....");
		sleep(1);

	}while(i--);

	return 0;
}


