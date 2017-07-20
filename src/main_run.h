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
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <thread>

#include "server_state.h"

class MainRun {
 public:
  bool running_;
  bool is_run_;
  bool is_exit_;
  int master_;

  MainRun();
  void Config();
  void Run();
  void Destory();
  void StopRun();

 private:
  std::string server_id_;
  ServerState *state_;

  void SetSignals();
  static void UserSignals(int signo);
  static void DealSignals(int signo);

};

#endif

