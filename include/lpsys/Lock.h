/**********************************************************
 *  \file Lock.h
 *  \brief	
 *  \note	×¢ÒâÊÂÏî£º 
 * 
 * \version 
 * * \author Allen.L
 ***********************************************************/

#ifndef LOCK_H_
#define LOCK_H_

#include "Exception.h"

namespace lputil {

template<typename T>
class LockT {
public:

	LockT(const T& mutex) :
			_mutex(mutex) {
		_mutex.lock();
		_acquired = true;
	}

	~LockT() {
		if (_acquired) {
			_mutex.unlock();
		}
	}

	void release() const {
		if (_acquired) {
			_mutex.unlock();
		}
		_acquired = false;
	}

	bool acquired() const {
		return _acquired;
	}

protected:

	LockT(const T& mutex, bool) :
			_mutex(mutex) {
		_acquired = _mutex.tryLock();
	}

private:

	const T& _mutex;
	mutable bool _acquired;

};

template<typename T>
class TryLockT: public LockT<T> {
public:

	TryLockT(const T& mutex) :
			LockT<T>(mutex, true) {
	}
};

}

#endif /* LOCK_H_ */
