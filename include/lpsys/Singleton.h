/**********************************************************
 *  \file Singleton.h
 *  \brief	单例类
 *  \note	注意事项：
 * 
 * \version 
 * * \author Allen.L
**********************************************************/
#ifndef _Singleton_H
#define _Singleton_H
#include <stddef.h>

namespace lpsys {

template<class T>
	class Singleton {
	protected:
		static T* _instance;

	public:
		static T* getInstance() {
			if (!_instance) {
				_instance = new T();
			}
				return _instance;
		}

		static void release() {
			if (_instance) {
				delete _instance;
				_instance = NULL;
		}
	}
};

template<class T> T* Singleton<T>::_instance = NULL;

}

#endif 

