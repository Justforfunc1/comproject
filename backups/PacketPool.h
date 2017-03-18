/**********************************************************
 *  \file PacketPool.h
 *  \brief	
 *  \note	注意事项： 
 * 
 * \version 
 * * \author Allen.L
 **********************************************************/
#ifndef _PacketPool_H
#define _PacketPool_H
#include "common.h"
#include <boost/thread/mutex.hpp>

#define BUFFER_SIZE 1024

class PacketPool;

class FieldPointer: public list<pair<char*, char*> > {
public:
	int length;
};

class PacketBuffer {
	friend class PacketPool;
public:
	pair<uint32, char*> end_pos; //有效字符的后一个位置
	deque<char *> buflist; //基指针
	PacketPool* m_pool;
	int total_len;

	PacketBuffer();
	~PacketBuffer();

	void req(char* &buf, int &size, int& len);
	bool reqNew(char* &buf, int &size);
	void accept(char* buf, int len);

	//包分析部分
	int m_rlen;
	int m_state;
	char rlen_buf[7];
	pair<uint32, char*> http_body; //第一个有效字符位置
	pair<uint32, char*> analysis_pos; //有效字符的后一个位置
	enum {
		H_START, H_PACKET_LEN, H_PACKET_BODY_START, H_PACKET_BODY, H_END
	};
	int anlaysisPacket();

	void getBodySection(FieldPointer& section);
	void pprint();
};

class PacketPool {
	int m_max_len;
	char* m_buffer;
	queue<int> m_qfree;
	boost::mutex mtx_qfree;
public:
	PacketPool(int maxlen);
	~PacketPool();

	PacketBuffer* reqBuffer();
	void handBackBuffer(PacketBuffer* buf);
	char* applyFree();

	void pprint();
};

class RequestItem {
public:
	int fd;
	void (*fun_handler)(RequestItem* request);

	RequestItem() :
			fd(0), fun_handler(NULL) {
	}
};

#endif
