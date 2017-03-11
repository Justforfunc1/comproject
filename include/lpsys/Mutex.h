/**********************************************************
 *  \file	Mutex.h
 *  \brief	
 *  \note	ע����� 
 * 
 * \version 
 * * \author Allen.L
 ***********************************************************/

#ifndef MUTEX_H_
#define MUTEX_H_

#include <pthread.h>
#include "Lock.h"

using namespace lputil;

namespace lpsys {

class Mutex {
public:

	typedef LockT<Mutex> Lock;
	typedef TryLockT<Mutex> TryLock;

	Mutex();
	~Mutex();

	/**
	 * @brief lock �������Ի�ȡ�����塣����������Ѿ���ס�����ͻ���𷢳�
	 * ���õ��̣߳�calling thread����ֱ���������ÿ���Ϊֹ��һ��������
	 * �õ��̻߳���˻����壬���þͻ���������
	 */
	void lock() const;

	/**
	 * @brief tryLock �������Ի�ȡ�����塣�����������ã�������ͻ���
	 * ס�������þͻ᷵��true����������߳���ס�˻����壬���÷���false
	 *
	 * @return
	 */
	bool tryLock() const;

	/**
	 * @brief unlock �������������ļ���
	 */
	void unlock() const;

private:

	mutable pthread_mutex_t _mutex;

};

}

#endif /* MUTEX_H_ */
