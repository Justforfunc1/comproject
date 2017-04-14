/**********************************************************
 * \file GPMutex.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include "gp_base/GPMutex.h"
#include "gp_base/GPException.h"

namespace GPBase {

GPMutex::GPMutex() {
	const GPInt32 rt = pthread_mutex_init(&_mutex, NULL);
	if (rt != 0) {
		throw GPBase::GPException(__FILE__, __LINE__);
	}
}

GPMutex::~GPMutex() {
	pthread_mutex_destroy(&_mutex);
}

GPVOID GPMutex::lock() const {
	const GPInt32 rt = pthread_mutex_lock(&_mutex);
	if (rt != 0) {
		throw GPBase::GPException(__FILE__, __LINE__);
	}
}

GPBOOL GPMutex::tryLock() const {
	const GPInt32 rt = pthread_mutex_trylock(&_mutex);
	return (rt == 0);
}

GPVOID GPMutex::unlock() const {
	const GPInt32 rt = pthread_mutex_unlock(&_mutex);
	if (rt != 0) {
		throw GPBase::GPException(__FILE__, __LINE__);
	}
}

}

