/**********************************************************
 * \file GPMemoryObject.h
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#ifndef _GPMemoryObject_H
#define _GPMemoryObject_H

#include "Base/Common/GPDefine.h"
#include <boost/foreach.hpp>


/**********************************************************

	DoubleObject			双对象内存

	类名		：	DoubleObject

	作者		：	Allen.L

	创建时间	：

	类描述		：

**********************************************************/

namespace GPBase {

template<class T>
class DoubleObject {
public:
	bool m_switch;
	T m_items[2];

	DoubleObject() :
			m_switch(false) {
	}

	~DoubleObject() {
		clear(3);
	}

	//当前
	T& getItemsA() {
		return m_items[m_switch];
	}

	//备用
	T& getItemsB() {
		return m_items[!m_switch];
	}

	void clone() {
		//备用清空
		m_items[!m_switch].clear();
		//当前复制
		m_items[!m_switch] = m_items[m_switch];
	}

	//flag: 1 当前 2 备用 3全部
	void clear(int flag) {
		if ((flag & 0x01) == 0x01) {
			m_items[m_switch].clear();
		}
		if ((flag & 0x02) == 0x02) {
			m_items[!m_switch].clear();
		}
	}

	void swap() {
		m_switch = !m_switch; //切换区
	}
};

//
template<typename K, typename V>
class DoubleMap {
public:
	bool m_switch;
	map<K, V> m_items[2];

	DoubleMap() :
			m_switch(false) {
	}

	~DoubleMap() {
		clear(3);
	}

	//当前
	map<K, V>& getItemsA() {
		return m_items[m_switch];
	}

	//备用
	map<K, V>& getItemsB() {
		return m_items[!m_switch];
	}

	void clone() {
		m_items[!m_switch].clear();

		for (typename map<K, V>::iterator iter = m_items[m_switch].begin(); iter != m_items[m_switch].end(); iter++) { //当前复制
			m_items[!m_switch][(*iter).first] = (*iter).second;
		}
	}
	//flag: 1 当前 2 备用 3全部
	void clear(int flag) {
		if ((flag & 0x01) == 0x01) {
			m_items[m_switch].clear();
		}
		if ((flag & 0x02) == 0x02) {
			m_items[!m_switch].clear();
		}
	}

	void swap() {
		m_switch = !m_switch; //切换区
	}
};


/**********************************************************

	BaseMO			批数据内存对象

	类名		：	BaseMO

	作者		：	Allen.L

	创建时间	：

	类描述		：

**********************************************************/

template<typename K, typename V>
class BaseMO: public DoubleMap<K, V> {
public:
	void loadBatch(map<K, V>& list) {
		map<K, V>& items = DoubleMap<K, V>::getItemsB();
		items = list;
	}

	void load(K &key, V &vo) {
		DoubleMap<K, V>::getItemsB()[key] = vo;
	}
};

}

#endif

