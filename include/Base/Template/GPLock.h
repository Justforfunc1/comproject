/**********************************************************
 * \file GPLock.h
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#ifndef _GPLock_H
#define _GPLock_H

#include "Base/Utils/GPException.h"

namespace GPBase {


/**********************************************************

	GPLockT				锁

	类名		：		GPLockT

	作者		：		Allen.L

	创建时间	：

	类描述		：		锁模板类

**********************************************************/

template<typename T>
class GPLockT {
public:

	GPLockT(const T& mutex) :
			_mutex(mutex) {
		_mutex.lock();
		_acquired = true;
	}

	~GPLockT() {
		if (_acquired) {
			_mutex.unlock();
		}
	}

	GPVOID release() const {
		if (_acquired) {
			_mutex.unlock();
		}
		_acquired = false;
	}

	GPBOOL acquired() const {
		return _acquired;
	}

protected:

	GPLockT(const T& mutex, GPBOOL) :
			_mutex(mutex) {
		_acquired = _mutex.tryLock();
	}

private:

	const T& _mutex;
	mutable bool _acquired;

};


/**********************************************************

	GPTryLockT			获取锁

	类名		：		GPLockT

	作者		：		Allen.L

	创建时间	：

	类描述		：		获取锁模板类

**********************************************************/

template<typename T>
class GPTryLockT: public GPLockT<T> {
public:

	GPTryLockT(const T& mutex) :
			GPLockT<T>(mutex, true) {
	}
};

}

#endif

