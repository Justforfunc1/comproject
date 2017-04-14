/**********************************************************
 * \file test.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include "gp_template/GPSingleton.h"
#include "boost/shared_ptr.hpp"
#include <iostream>

using namespace GPBase;

class GPTest
{
	public:
		GPTest();
		~GPTest();
		int testprint();

	private:
		int m_id;
};

GPTest::GPTest():m_id(2){};
GPTest::~GPTest(){};

typedef GPBase::GPSingleton<GPTest> SingleTest;

int GPTest::testprint()
{
	std::cout << this->m_id << std::endl;
	return 0;
}

int main(int argc, char** argv)
{
	auto test1 = SingleTest::getInstance();
	test1->testprint();
	return 0;
}
