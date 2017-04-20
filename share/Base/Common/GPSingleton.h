/**********************************************************
 * \file GPSingleton.h
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#ifndef _GPSingleton_H
#define _GPSingleton_H

#include <stddef.h>

namespace GPBase {


/**********************************************************

	GPSingleton		单例类

	类名			：	GPSingleton

	作者			：	Allen.L

	创建时间		：

	类描述			：


**********************************************************/

template<class T>
class GPSingleton {
protected:
	static T* _instance;

public:
	static T* getInstance() {
		if (!_instance)
			_instance = new T();
		return _instance;
	}

	static void release() {
		if (_instance) {
			delete _instance;
			_instance = NULL;
		}
	}
};

template<class T> T* GPSingleton<T>::_instance = NULL;

}//namespace GPBase

#endif

