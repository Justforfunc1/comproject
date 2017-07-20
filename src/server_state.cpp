/**********************************************************
 * \file server_state.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include "server_state.h"

ServerState::ServerState() {
}

static long GetSeconds() {
	time_t rawtime;
	struct tm* curtime;
	time(&rawtime);
	curtime = localtime(&rawtime);
	return (curtime->tm_hour * 3600 + curtime->tm_min * 60 + curtime->tm_sec);
}


void ServerState::Run() {
}


void ServerState::Config(const char* start_time, const char* end_time) {
}

