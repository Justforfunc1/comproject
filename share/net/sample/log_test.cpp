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
#include "lpsys/lpsys.h"

#define INIT_FILE "test.ini"
using namespace lpsys;
using namespace std;

int main(int args, char **argv)
{
	LPCONFIG_LOAD(INIT_FILE);
	string log_file = string(LPCONFIG_GETSTR("logging", "logger", "log_YYYYMMDD.log"));
	cout << "file:" << log_file.c_str() << endl;
	string log_level = string(LPCONFIG_GETSTR("logging", "level", "debug"));
	cout << "level:" << log_level.c_str() << endl;
	LPSYS_LOGGER_INIT(log_file, log_level, true);
/*
	const char* path = LPCONFIG_GETSTR("logging","logger","log1_YYYYMMDD.log");
	cout << "path:" << path << endl;
	const char* level = LPCONFIG_GETSTR("logging","level","debug");
	cout << "level:" << level << endl;
	LPSYS_LOGGER_INIT(string(path), string(level), true);
*/

	int i=1;
	while(1)
	{
		cout << i << endl;
		LPLOG_INFO("------>TEST startup.....");
		LPLOG_ERROR("------>TEST errno.....");
		LPLOG_DEBUG("------>TEST debug.....");
		LPLOG_WARNING("------>TEST warning.....");
		sleep(10);
		i++;
	}

	return 0;
}


