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
#include <list>
#include <mutex>
#include <algorithm>

void hello()
{
	std::cout << "Hello Concurrent World\n";
}
int main(int argc, char** argv)
{
	std::thread t(hello);
	t.join();
	std::list<int> mylist;
	std::mutex mymutex;
	std::lock_guard<std::mutex> guard(mymutex);


 // return 0;
}
