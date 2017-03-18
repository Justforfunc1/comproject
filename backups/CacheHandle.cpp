 /**********************************************************
 *  \file CacheHandle.cpp
 *  \brief
 *  \note	×¢ÒâÊÂÏî£º 
 * 
 * \version 
 * * \author Allen.L
 *********************************************************/


#include "CacheHandle.h"

Connection::Connection(const std::string& host, const unsigned int port, unsigned int timeout) {
	struct timeval tv;
	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	context = redisConnectWithTimeout(host.c_str(), port, tv);
	if (context->err != REDIS_OK) {
		m_logger->warning("cachedb connection fail![host=%s, port=%d, error=%s]", host.c_str(), (int) port, context->errstr);
		redisFree(context);
		context = NULL;
	}
}

Connection::~Connection() {
	if (context) {
		redisFree(context);
		context = NULL;
	}
}

bool Connection::Append(const std::vector<std::string>& command) {
	if (!Isvalid())
		return false;

	std::vector<const char*> argv;
	argv.reserve(command.size());
	std::vector<size_t> argvlen;
	argvlen.reserve(command.size());

	for (std::vector<std::string>::const_iterator it = command.begin(); it != command.end(); it++) {
		argv.push_back(it->c_str());
		argvlen.push_back(it->size());
	}

	int ret = redisAppendCommandArgv(context, static_cast<int>(command.size()), argv.data(), argvlen.data());

	return ret == REDIS_OK;
}

Reply Connection::GetReply() {
	redisReply* r = NULL;
	int ret = redisGetReply(context, reinterpret_cast<void**>(&r));
	if (ret != REDIS_OK)
		m_logger->warning("get reply for cachedb fail![error=%s, ret=%d]", context->errstr, ret);

	Reply reply(r);
	freeReplyObject(r);
	r = NULL;

	return reply;
}

Connection::ptr_t Connection::Create(const std::string& host, const unsigned int port, unsigned int timeout) {
	return new Connection(host, port);
}

Reply Connection::Run(const std::vector<std::string>& command) {
	return Append(command) ? GetReply() : Reply(NULL);
}

bool Connection::Isvalid() {
	return context && context->err == REDIS_OK;
}

CacheHandle::CacheHandle(const std::string& host, unsigned int port, unsigned int timeout) {
	_host = host;
	_port = port;
	_timeout = timeout;
	_db = 0;
}

CacheHandle::~CacheHandle() {
	ClearConnection();
}

CacheHandle::ptr_t CacheHandle::Create(const std::string& host, unsigned int port, unsigned int timeout) {
	return CacheHandle::ptr_t(new CacheHandle(host, port, timeout));
}

Connection::ptr_t CacheHandle::Get() {
	Connection::ptr_t conn = NULL;
	{
		boost::unique_lock<boost::shared_mutex> lock(pool_mutext);
		if (!connPool.empty()) {
			conn = connPool.front();
			connPool.pop_front();
		}
	}

	if (!conn) {
		conn = Connection::Create(_host, _port);
		if (conn->Isvalid() && _db > 0)
			conn->Run(Command("SELECT")(_db));
	}

	return conn;
}

void CacheHandle::Put(Connection::ptr_t conn) {
	if (conn->Isvalid()) {
		boost::unique_lock<boost::shared_mutex> lock(pool_mutext);
		connPool.push_back(conn);
	} else {
		delete conn;
		conn = NULL;
	}
}

void CacheHandle::SetDB(const unsigned int value) {
	_db = value;
}

void CacheHandle::ClearConnection() {
	boost::unique_lock<boost::shared_mutex> lock(pool_mutext);
	for (std::deque<Connection::ptr_t>::iterator it = connPool.begin(); it != connPool.end(); it++) {
		delete (*it);
	}
	connPool.clear();
}

bool CacheHandle::IsConnection() {
	bool ret = false;
	Connection::ptr_t conn = Get();
	if (conn->Isvalid()) {
		Reply r = conn->Run(Command("PING"));
		if (r.type == REDIS_REPLY_STATUS && r.str == "PONG")
			ret = true;
	}
	Put(conn);

	return ret;
}

int CacheHandle::Flush() {
	bool ret = -1;
	Connection::ptr_t conn = Get();
	if (conn->Isvalid()) {
		Reply r = conn->Run(Command("FLUSHDB"));
		if (r.type == REDIS_REPLY_STATUS && r.str == "OK") {
			ret = 0;
		}
	}
	Put(conn);

	return ret;
}
