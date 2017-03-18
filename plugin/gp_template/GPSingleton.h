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
#include "boost/shared_ptr.hpp"


namespace GPBase {


/**********************************************************

	GPSingleton		单例类

	类名			：	GPSingleton

	作者			：	Allen.L

	创建时间		：

	类描述			：	1	被单例的对象，需要有公有的无参数的构造器。
						2	使用该类时可以保证单例。但如果外界有其他创建则无法知晓和保证单例。
	note	建议的通用命名：使用typedef来设置别名，别名在其他类的基础上增加S / Single。
	note	支持有参数的构造器：使用getInstance传入指针。但被单例的类依然必须保证拥有一个无参数的构造器（哪怕此构造器完全无用）
	todo	解决被单例的类必须拥有一个无参数构造器的问题。

**********************************************************/

template < typename T >
	class GPSingleton {

	public:


		/**********************************************************

			函数名		：	getInstance

			功能		：

			参数		：

			返回值		：

			创建作者	：	Allen.L

			修改记录	：

		**********************************************************/
		static boost::shared_ptr<T> getInstance();


		/**********************************************************

			函数名		：	getInstance

			功能		：	允许外界传入一个指针初始化：为保证意外的delete，故使用智能指针。

			参数		：	boost::shared_ptr<T> ptrT

			返回值		：

			创建作者	：	Allen.L

			修改记录	：

		**********************************************************/
		static boost::shared_ptr<T> getInstance( boost::shared_ptr<T> ptrT );


		/**********************************************************

			函数名		：	release

			功能		：	在希望删除单例时调用。但需要注意，如果删除后再调用getInstance依然会重新创建单例.

			参数		：

			返回值		：

			创建作者	：	Allen.L

			修改记录	：

		**********************************************************/
		static void release();


	private:
		GPSingleton(){
						m_p_Object = boost::shared_ptr<T>(new T());
					}
		GPSingleton( boost::shared_ptr<T> _ptrT )
						:m_p_Object( _ptrT )
					{ ; }
		virtual ~GPSingleton(){ ; }


	private:
		class Deleter
		{
			public:
				~Deleter()
				{
					if( GPSingleton::m_s_p_Instance != NULL )
						delete GPSingleton::m_s_p_Instance;
				}
		};

		static Deleter							m_s_Deleter;
	private:
		static GPSingleton<T>*					m_s_p_Instance;
		static GPBase::GPMutex					m_s_Mutex;

		boost::shared_ptr<T>					m_p_Object;

	protected:
		static T* _instance;


};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template < typename T > GPBase::GPMutex		GPSingleton<T>::m_s_Mutex = GPBase::GPMutex();
template < typename T > GPSingleton<T>*		GPSingleton<T>::m_s_p_Instance = NULL;

template < typename T >
boost::shared_ptr<T> GPSingleton<T>::getInstance(){
	if( m_s_p_Instance == NULL ){
		m_s_Mutex.lock();
		if( m_s_p_Instance == NULL ){
			m_s_p_Instance = new GPSingleton();
			return m_s_p_Instance->m_p_Object;
		}
		m_s_Mutex.unlock();
	}
	return m_s_p_Instance->m_p_Object;
}

template < typename T >
boost::shared_ptr<T> GPSingleton<T>::getInstance( boost::shared_ptr<T> ptrT ){
	if( m_s_p_Instance == NULL ){
		m_s_Mutex.lock();
		if( m_s_p_Instance == NULL ){
			m_s_p_Instance = new GPSingleton(ptrT);
			return m_s_p_Instance->m_p_Object;
		}
		m_s_Mutex.unlock();
	}
	return m_s_p_Instance->m_p_Object;
}

template < typename T >
static void GPSingleton<T>::release(){
	if( GPSingleton::m_s_p_Instance != NULL ){
		delete GPSingleton::m_s_p_Instance;
		GPSingleton::m_s_p_Instance = NULL;
	}
}

}//namespace GPBase

#endif

