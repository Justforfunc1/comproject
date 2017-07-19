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

static long getSeconds() {
	time_t rawtime;
	struct tm* curtime;
	time(&rawtime);
	curtime = localtime(&rawtime);
	return (curtime->tm_hour * 3600 + curtime->tm_min * 60 + curtime->tm_sec);
}


void ServerState::run() {
}


void ServerState::config(const char* start_time, const char* end_time) {
}

