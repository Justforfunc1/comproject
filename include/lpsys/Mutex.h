/**********************************************************
 *  \file	Mutex.h
 *  \brief	
 *  \note	注意事项： 
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
	 * @brief lock 函数尝试获取互斥体。如果互斥体已经锁住，它就会挂起发出
	 * 调用的线程（calling thread），直到互斥体变得可用为止。一旦发出调
	 * 用的线程获得了互斥体，调用就会立即返回
	 */
	void lock() const;

	/**
	 * @brief tryLock 函数尝试获取互斥体。如果互斥体可用，互斥体就会锁
	 * 住，而调用就会返回true。如果其他线程锁住了互斥体，调用返回false
	 *
	 * @return
	 */
	bool tryLock() const;

	/**
	 * @brief unlock 函数解除互斥体的加锁
	 */
	void unlock() const;

private:

	mutable pthread_mutex_t _mutex;

};

}

#endif /* MUTEX_H_ */
