/**********************************************************
 * \file server_state.h
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/
#ifndef _SERVER_STATE_H
#define _SERVER_STATE_H

#include <ctime>

class ServerState {
 public:
  int state_;
  char charge_data_[20];
  long last_start_time_;
  long last_end_time_;

  ServerState();
  void Config(const char* start_time, const char* end_time);
  void Run();
};

#endif

