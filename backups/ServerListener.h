/**********************************************************
 *  \file ServerListener.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author Allen.L
 **********************************************************/
#ifndef _ServerListener_H
#define _ServerListener_H
#include "common.h"
#include "PacketPool.h"
#include "FilterManager.h"

#define FD_MAX	1024

using namespace lpsys;

typedef enum {
	FDEVENT_READ, FDEVENT_WRITE, FDEVENT_DELETE
} fdenvent_type_t;

class ServerListener: public Singleton<ServerListener>, public Logging {
private:
	bool m_isRun;

	int m_port;
	int m_max_conn;

	boost::thread* m_runThread;

	static PacketPool* m_ppool;

	static FilterManager* m_filter;

	static int epoll_fd;

	static uint64 s_conn[FD_MAX];

	static int setNonblock(int fd);

	static inline void appendConnRec(int fd) {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		s_conn[fd] = tv.tv_sec + (tv.tv_usec << 32);
	}

	static inline void cleanConnRec(int fd) {
		s_conn[fd] = 0;
	}

	static inline uint64 getConnRec(int fd) {
		return s_conn[fd];
	}

	static void connAccept(RequestItem* request);
	static void dataRead(RequestItem* request);

	static int fdevent_ctl(int op, RequestItem* request, fdenvent_type_t filter);
	static int fdevent_insert(RequestItem* request, fdenvent_type_t filter);
	static int fdevent_delete(RequestItem* request);
	static void fdevent_destroy();

	static RequestItem* request_new(int fd, void (*handler)(RequestItem* request));
	static void request_destory(RequestItem* request);

	void startupServer();

public:
	ServerListener();
	~ServerListener();

	void config(const int port, const int max_conn, const int max_pool, const int packet_pool);

	static int fdevent_update(RequestItem* request, fdenvent_type_t filter);
	static void runProxy();

	void start();
	void stop();

	static void dataSend(int fd, uint32 size, const char* data);

	static void readUntilEmpty(int conn);

	static int checkConnRec(int fd, uint64 mac) {
		if (s_conn[fd] == mac) {
			s_conn[fd] = 0;
			return 1;
		} else {
			return 0;
		}
	}
};

#endif
