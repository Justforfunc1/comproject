/**********************************************************
 *  \file ServerListener.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author Allen.L
 **********************************************************/
#include <sys/socket.h>
#include <linux/tcp.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <boost/threadpool.hpp>
#include <boost/bind.hpp>
#include "ServerListener.h"

#define MAX_TIMEWAIT	2
#define MAX_EVENTS		16

uint64 ServerListener::s_conn[FD_MAX] = { };

int ServerListener::epoll_fd = 0;

PacketPool* ServerListener::m_ppool = NULL;

FilterManager* ServerListener::m_filter = NULL;

ServerListener::ServerListener() {
	m_port = 8080;
	m_max_conn = 20;
	m_isRun = false;
	m_ppool = NULL;
	m_filter = NULL;
	m_runThread = NULL;
	memset(s_conn, 0, sizeof(uint64) * FD_MAX);
}

ServerListener::~ServerListener() {
	if (m_ppool != NULL) {
		delete m_ppool;
	}

	if (m_filter != NULL) {
		delete m_filter;
	}
}

int ServerListener::setNonblock(int fd) {
	int flags;
	if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
		PSLOG_ERROR("fcntl failed!(errno=%d)", errno);
		return -1;
	}

	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void ServerListener::config(const int port, const int max_conn, const int max_pool, const int pool_size) {
	m_port = port;
	m_max_conn = max_conn;

	m_ppool = new PacketPool(pool_size);

	m_filter = new FilterManager(max_pool);
	m_filter->init();
}

int ServerListener::fdevent_ctl(int op, RequestItem* request, fdenvent_type_t filter) {
	struct epoll_event event;
	memset(&event, 0, sizeof(struct epoll_event));

	if (filter == FDEVENT_READ)
		event.events = EPOLLIN | EPOLLERR | EPOLLHUP;  //EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP | EPOLLET
	else if (filter == FDEVENT_WRITE)
		event.events = EPOLLOUT | EPOLLERR | EPOLLHUP; //EPOLLOUT | EPOLLPRI | EPOLLERR | EPOLLHUP | EPOLLET
	else
		event.events = 0;

	event.data.ptr = (void *) request;

	return epoll_ctl(epoll_fd, op, request->fd, &event);
}

int ServerListener::fdevent_insert(RequestItem* request, fdenvent_type_t filter) {
	return fdevent_ctl(EPOLL_CTL_ADD, request, filter);
}

int ServerListener::fdevent_update(RequestItem* request, fdenvent_type_t filter) {
	return fdevent_ctl(EPOLL_CTL_MOD, request, filter);
}

int ServerListener::fdevent_delete(RequestItem* request) {
	return fdevent_ctl(EPOLL_CTL_DEL, request, FDEVENT_DELETE);
}

void ServerListener::fdevent_destroy() {
	close(epoll_fd);
}

RequestItem* ServerListener::request_new(int fd, void (*handler)(RequestItem* request)) {
	RequestItem* request = new RequestItem();
	request->fd = fd;
	request->fun_handler = handler;
	appendConnRec(fd);
	return request;
}

void ServerListener::request_destory(RequestItem* request) {
	if (request) {
		PSLOG_DEBUG("connection[fd=%d] closed!", request->fd);
		fdevent_delete(request);
		close(request->fd);
		cleanConnRec(request->fd);
		delete request;
		request = NULL;
	}
}

/**
 * 启动端口监听、epoll事件处理机制
 */
void ServerListener::startupServer() {
	m_isRun = true;

	struct sockaddr_in sock;
	memset(&sock, 0 ,sizeof(sock));
	sock.sin_family = AF_INET;
	sock.sin_port = htons((uint16_t) m_port);
	sock.sin_addr.s_addr = htonl(INADDR_ANY);

	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1) {
		m_logger->error("socket listen open error, %s!", strerror(errno));
		return;
	}
	setNonblock(listenfd);
	const int dummy = 1, accept_delay = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *) &dummy, sizeof(dummy));
	setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY, (char *)&dummy, sizeof(dummy));
	setsockopt(listenfd, IPPROTO_TCP, TCP_DEFER_ACCEPT, &accept_delay, sizeof(accept_delay));

	if (bind(listenfd, (struct sockaddr *) &sock, sizeof(sock)) < 0) {
		if (errno == EADDRINUSE)
			m_logger->error("bind failed! Address already in use.(errno=%d)", errno);
		else
			m_logger->error("bind failed!(errno=%d)", errno);
		return;
	}

	if (listen(listenfd, MAX_EVENTS) < 0) {
		m_logger->error("listen failed!(errno=%d)", errno);
		return;
	}

	epoll_fd = epoll_create(m_max_conn);
	if (epoll_fd <= 0) {
		close(listenfd);
		m_logger->error("epoll epoll_create failed!(errno=%d)", errno);
		return;
	}

	RequestItem* request = request_new(listenfd, ServerListener::connAccept);
	if (fdevent_insert(request, FDEVENT_READ) < 0) {
		request_destory(request);
		fdevent_destroy();
		m_logger->error("epoll epoll_ctl failed!(errno=%d)", errno);
		return;
	}

	struct epoll_event events[MAX_EVENTS];
	int ndfs, i;
	struct epoll_event *cevent;

	m_logger->info("-------> ServerListener is wait.....");

	//watch the epoll evetns
	while (m_isRun) {
		ndfs = epoll_wait(epoll_fd, events, MAX_EVENTS, MAX_TIMEWAIT);
		if (ndfs < 0) {
			m_logger->warning("epoll_wait failed!(errno=%d)", errno);
			continue;
		}

		for (i = 0, cevent = events; i < ndfs; i++, cevent++) {
			RequestItem *req = (RequestItem*) cevent->data.ptr;
			if (req == NULL)
				continue;

			if (cevent->events & (EPOLLHUP | EPOLLERR)) {
				m_logger->warning("epoll fd_conn[%d] is EPOLLHUP | EPOLLERR. It is closed!", req->fd);
				request_destory(req);
			} else {
				req->fun_handler(req);
			}
		}
	}

	request_destory(request);
	fdevent_destroy();

	m_logger->info("<------- ServerListener stop!");

	sleep(1); //保证线程正常退出
}

/**
 * 接收新的连接
 */
void ServerListener::connAccept(RequestItem* request) {
	struct sockaddr_in addr_client;
	socklen_t addr_client_len = sizeof(addr_client);
	int fd = accept(request->fd, (sockaddr *) &addr_client, &addr_client_len);
	if (fd < 0) {
		PSLOG_ERROR("accept failed[fd=%d]!(errno=%d)", errno);
		return;
	}
	setNonblock(fd);

	PSLOG_DEBUG("acception conneciton[fd=%d] success!", fd);

	RequestItem* req = request_new(fd, ServerListener::dataRead);
	if (fdevent_insert(req, FDEVENT_READ) < 0) {
		request_destory(req);
		PSLOG_ERROR("epoll_wait failed!(errno=%d)", errno);
		return;
	}
}

/**
 * 接收Client的请求数据
 */
void ServerListener::dataRead(RequestItem* request) {
	PacketBuffer* pBuf = m_ppool->reqBuffer();
	if (pBuf == NULL) {
		readUntilEmpty(request->fd);
		PSLOG_ERROR("packet buffer first alloc is filled!");
		return;
	}

	int psize = 0, total_len = 0, r_len = 0, ret = 0;
	char* pbuf;
	do {
		pBuf->req(pbuf, psize, total_len);
		if (psize <= 0) {
			if (!pBuf->reqNew(pbuf, psize)) {
				readUntilEmpty(request->fd);
				delete pBuf;
				pBuf = NULL;
				PSLOG_ERROR("packet buffer sencond alloc is filled!");
				break;
			}
		}

		r_len = read(request->fd, pbuf, psize);
		if (r_len > 0) {
			total_len += r_len;
			pbuf += r_len;
			pBuf->accept(pbuf, total_len);
		} else if (r_len < 0 && errno == EAGAIN) { //数据接收完成
			if (total_len > 0) {
				ret = pBuf->anlaysisPacket();
				if (ret == 0) {
					m_filter->run(request->fd, getConnRec(request->fd), pBuf);
				} else {
					delete pBuf;
					pBuf = NULL;
					PSLOG_ERROR("packet buffer anlaysis error!");
				}
			} else {
				delete pBuf;
				pBuf = NULL;
				request_destory(request);
				PSLOG_ERROR("packet buffer total length is zero!");
			}
			break;
		} else if (r_len == 0) { //连接断开
			delete pBuf;
			pBuf = NULL;
			request_destory(request);
			break;
		} else {
			readUntilEmpty(request->fd);
			delete pBuf;
			pBuf = NULL;
			break;
		}
	} while (1);
}

/**
 * 向Client发送数据
 */
void ServerListener::dataSend(int fd, uint32 size, const char* data) {
	if (size > 0) {
		uint32 tlen = 0, wlen = 0;
		while (tlen < size) {
			wlen = write(fd, data + tlen, size - tlen);
			if (wlen >= 0) {
				tlen += wlen;
			} else if (errno == EAGAIN) {
				continue;
			} else {
				PSLOG_ERROR("response packet write error(%d)!", errno);
				break;
			}
		};
	}
}

void ServerListener::runProxy() {
	_instance->startupServer();
}

void ServerListener::start() {
	m_runThread = new boost::thread(ServerListener::runProxy);
}

void ServerListener::stop() {
	if (m_isRun) {
		m_isRun = false;
		try {
			bool isOver = m_runThread->timed_join(boost::posix_time::microseconds(2000));
			if (!isOver)
				m_runThread->interrupt();
		} catch (exception & e) {
			printf("interrupt logWriter error![%s]", e.what());
		}

		delete m_runThread;
		m_runThread = NULL;
	}
}

void ServerListener::readUntilEmpty(int conn) {
	int r_len;
	char buf[1024];
	do {
		r_len = recv(conn, buf, 1024, 0);
	} while (r_len > 0);
}
