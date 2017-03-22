/**********************************************************
 * \file thread_test.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include <iostream>
#include <thread>

void hello()
{
  std::cout << "Hello Concurrent World\n";
}
int main(int argc, char** argv)
{
  std::thread t(hello);
  t.join();

 // return 0;
}
