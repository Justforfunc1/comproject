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
#include "base/base.h"
using namespace std;

static void* hello1(void *)
{
	std::cout << "Hello1 Concurrent World\n";
}

static void* hello2(void *)
{
	std::cout << "Hello2 Concurrent World\n";
}

static void* hello3(void *)
{
	std::cout << "Hello3 Concurrent World\n";
}

static void* hello4(void *)
{
	std::cout << "Hello4 Concurrent World\n";
}

static void* hello5(void *)
{
	std::cout << "Hello5 Concurrent World\n";
}



int main(int argc, char** argv)
{
//	void (*p)() = nullptr;
//	p = &hello1;
//	thread thread1(p);
//	thread1.join();

	vector<int> dsp_vo;
	for (int i = 1; i<4; i++) {
		dsp_vo.push_back(i);
	}

	map<int,void* (*)(void*)> dsp_work;
	dsp_work.insert(map<int,void* (*)(void*)>::value_type(1, &hello1));
	dsp_work.insert(map<int,void* (*)(void*)>::value_type(2, &hello2));
	dsp_work.insert(map<int,void* (*)(void*)>::value_type(3, &hello3));
	dsp_work.insert(map<int,void* (*)(void*)>::value_type(4, &hello4));
	dsp_work.insert(map<int,void* (*)(void*)>::value_type(5, &hello5));

	pthread_t t[10];
	//thread t[5];

	void *p1 = nullptr;
	int num = 0;
	for (auto it = dsp_vo.cbegin(); it !=dsp_vo.cend(); ++it ) {
		int dsp_id = *it;
		auto a_it = dsp_work.find(dsp_id);
		if (a_it != dsp_work.cend()) {
			pthread_create(&(t[num]),NULL,(*a_it).second, NULL);
			num++;
//			void (*p)() = (*a_it).second;
//			t[(*a_it).first](*p);
		}
	}

	for (int j = 0; j < num; j++) {
		pthread_join(t[j],NULL);
	}


	return 0;
}
