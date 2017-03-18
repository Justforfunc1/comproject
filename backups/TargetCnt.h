/**********************************************************
 *  \file TargetCnt.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author Allen.L
 **********************************************************/
#ifndef _TARGET_CNT_H
#define _TARGET_CNT_H

#include "common.h"
#include "share_map.h"
#include <boost/thread/shared_mutex.hpp>

class TargetCnt: public lpsys::Singleton<TargetCnt> {
protected:
	boost::shared_mutex rwmutex;
	share_map<uint32, uint64> m_cnt;
public:
	TargetCnt() :
			m_cnt("ShareMem", 1024 * 1024 * 64, "TargetCnt") {
	}

	uint64 getCnt(uint32 k) {
		boost::shared_lock<boost::shared_mutex> rdlock(rwmutex);
		return m_cnt[k];
	}

	void setCnt(uint32 k, uint64 v) {
		boost::unique_lock<boost::shared_mutex> wtlock(rwmutex);
		share_map<uint32, uint64>::iterator iter = m_cnt.find(k);
		if (iter == m_cnt.end()) {
			m_cnt.insert(std::pair<const uint32, uint64>(k, v));
		} else {
			iter->second = v;
		}
	}

	uint64 plus(uint32 k) {
		boost::unique_lock<boost::shared_mutex> wtlock(rwmutex);
		return m_cnt[k]++;
	}

	void destory() {
		m_cnt.destory("ShareMem", "TargetCnt");
		m_cnt.release("ShareMem");
	}

	static void release() {
		share_map<uint32, uint64>::release("ShareMem");
	}

	void clear() {
		boost::unique_lock<boost::shared_mutex> wtlock(rwmutex);
		m_cnt.clear();
	}
};
#endif
