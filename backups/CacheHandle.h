/**********************************************************
 *  \file CacheHandle.h
 *  \brief
 *  \note	×¢ÒâÊÂÏî: 
 * 
 * \version 
 * * \author Allen.L
 **********************************************************/

#ifndef _CACHEHANDLE_H_
#define _CACHEHANDLE_H_

#include "common.h"
#include <memory>
#include "hiredis/hiredis.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

template<class T>
std::string convertToString(const T v) {
	std::stringstream ss;
	ss << v;
	return ss.str();
}

class Command {
public:
	Command(const std::string& cmd) {
		_args.push_back(cmd);
	}

	template<class Type>
	Command& operator()(const Type& cmd) {
		_args.push_back(convertToString(cmd));
		return (*this);
	}

	operator const std::vector<std::string> &() {
		return _args;
	}

private:
	std::vector<std::string> _args;
};

typedef struct Reply {
	int type;
	long long integer;
	std::string str;
	std::vector<Reply> elements;

	Reply(const redisReply* reply) :
			type(REDIS_REPLY_ERROR), integer(0), str("") {
		if (reply != NULL) {
			type = reply->type;
			switch (type) {
			case REDIS_REPLY_STRING:
			case REDIS_REPLY_STATUS:
			case REDIS_REPLY_ERROR:
				str = std::string(reply->str);
				break;
			case REDIS_REPLY_INTEGER:
				integer = reply->integer;
				break;
			case REDIS_REPLY_ARRAY:
				for (size_t i = 0; i < reply->elements; i++) {
					elements.push_back(reply->element[i]);
				}
				break;
			default:
				break;
			}
		}
	}
} Reply;

class Connection: lpsys::noncopyable, lpsys::Logging {
public:
	typedef Connection* ptr_t;

	static ptr_t Create(const std::string& host = "127.0.0.1", const unsigned int port = 6379, unsigned int timeout = 100);
	Reply Run(const std::vector<std::string>& command);
	bool Isvalid();

	~Connection();
private:
	redisContext* context;

	Connection(const std::string& host, const unsigned int port, unsigned int timeout = 100);
	bool Append(const std::vector<std::string>& command);
	Reply GetReply();

	friend class CacheHandle;
};

class CacheHandle {
public:
	typedef std::auto_ptr<CacheHandle> ptr_t;

	static ptr_t Create(const std::string& host = "127.0.0.1", unsigned int port = 6379, unsigned int timeout = 100);

	Connection::ptr_t Get();
	void Put(Connection::ptr_t conn);
	void SetDB(const unsigned int value);

	bool IsConnection();
	int Flush();

	~CacheHandle();

private:
	std::string _host;
	unsigned int _port;
	unsigned int _timeout;
	unsigned int _db;

	std::deque<Connection::ptr_t> connPool;

	boost::shared_mutex pool_mutext;

	CacheHandle(const std::string& host, unsigned int port, unsigned int timeout);
	void ClearConnection();
};

#endif
