/**********************************************************
 * \file GPMutex.h
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#ifndef _GPMutex_H
#define _GPMutex_H

#include "gp_base/GPDefine.h"
#include "gp_base/GPLock.h"


namespace GPBase {


/**********************************************************

	GPMutex		互斥锁

	类名			：	GPMutex

	作者			：	Allen.L

	创建时间		：

	类描述			：

**********************************************************/

class GPMutex {
public:

	typedef GPLockT<GPMutex> GPLock;
	typedef GPTryLockT<GPMutex> GPTryLock;

	GPMutex();
	~GPMutex();


	/**********************************************************

		函数名		：	lock

		功能		：	尝试获取互斥体。如果互斥体已经锁住，它就会挂起发出调用的线程（calling thread），
					直到互斥体变得可用为止。一旦发出调用的线程获得了互斥体，调用就会立即返回。

		参数		：

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPVOID lock() const;


	/**********************************************************

		函数名		：	tryLock

		功能		：	尝试获取互斥体。如果互斥体可用，互斥体就会锁住，而调用就会返回true。
					如果其他线程锁住了互斥体，调用返回false

		参数		：

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPBOOL tryLock() const;


	/**********************************************************

		函数名		：	unlock

		功能		：	解除互斥体的加锁

		参数		：

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPVOID unlock() const;

private:

	mutable pthread_mutex_t _mutex;

};

}

#endif

