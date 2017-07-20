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
#include "base/base.h"

#define INIT_FILE "../conf/log_test.ini"

using namespace base;
using namespace std;

int main(int args, char* argv[])
{
	CONFIG_LOAD(INIT_FILE);
	string log_file = string(CONFIG_GETSTR("logging", "logger", "log_YYYYMMDD.log"));
	cout << "log_path:" << log_file.c_str() << endl;
	string log_level = string(CONFIG_GETSTR("logging", "level", "debug"));
	cout << "log_level:" << log_level.c_str() << endl;
	BASE_LOGGER_INIT(log_file, log_level, true);

	int i=10;

	do
	{
		cout << i << endl;
		LOG_INFO("------>TEST startup.....");
		LOG_ERROR("------>TEST errno.....");
		LOG_DEBUG("------>TEST debug.....");
		LOG_WARNING("------>TEST warning.....");
		sleep(1);

	}while(i--);

	return 0;
}


