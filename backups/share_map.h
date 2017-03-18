/**********************************************************
 *  \file share_map.h
 *  \brief 共享内存类
 *  \note	注意事项： 
 * 
 * \version 
 * * \author Allen.L
 **********************************************************/
#ifndef _share_map_H
#define _share_map_H
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <functional>
#include <utility>

template<typename K, typename V>
class share_map {
public:
	typedef boost::interprocess::allocator<std::pair<const K, V>, boost::interprocess::managed_shared_memory::segment_manager> AllocatorType;
	typedef boost::container::map<K, V, std::less<K>, AllocatorType> _Map_Type;
	typedef typename _Map_Type::iterator iterator;
	typedef typename _Map_Type::size_type size_type;
	typedef std::pair<const K, V> value_type;
protected:
	_Map_Type *_map;
	boost::interprocess::managed_shared_memory* _segment;
public:
	share_map(const char* segment_name, uint32 size, const char* name) :
			_map(NULL), _segment(NULL) {
		try {
			_segment = new boost::interprocess::managed_shared_memory(boost::interprocess::create_only, segment_name, size);
			AllocatorType alloc_inst(_segment->get_segment_manager());
			_map = _segment->construct<_Map_Type>(name)(std::less<K>(), alloc_inst);
		} catch (boost::interprocess::interprocess_exception& e) {
			std::cout << "share_map already exist error[" << e.what() << "] try open it." << std::endl;
			try {
				_segment = new boost::interprocess::managed_shared_memory(boost::interprocess::open_only, segment_name);
				_map = _segment->find<_Map_Type>(name).first;
				std::cout << "share_map open succeed!" << std::endl;
			} catch (boost::interprocess::interprocess_exception& e) {
				std::cout << "share_map open error:" << e.what() << std::endl;
			}
		}
	}

	~share_map() {
		delete _segment;
	}

	static void release(const char* segment_name) {
		boost::interprocess::shared_memory_object::remove(segment_name);
	}

	void destory(const char* segment_name, const char* name) {
		try {
			_segment->destroy<_Map_Type>(name);
		} catch (boost::interprocess::interprocess_exception& e) {
			std::cout << "share_map open error:" << e.what() << std::endl;
		}
	}

	void clear() {
		(*_map).clear();
	}

	V& operator[](const K& k) {
		return (*_map)[k];
	}

	iterator find(const K& k) {
		return (*_map).find(k);
	}

	iterator begin() {
		return (*_map).begin();
	}

	iterator end() {
		return (*_map).end();
	}

	size_type size() const {
		return (*_map).size();
	}

	std::pair<iterator, bool> insert(const value_type & v) {
		return (*_map).insert(v);
	}
};

#endif
