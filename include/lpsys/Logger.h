/**********************************************************
 *  \file Logger.h
 *  \brief	
 *  \note	注意事项： 
 * 
 * \version 
 * * \author Allen.L
 ***********************************************************/

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <stdbool.h>
#include <stdarg.h>
#include <linux/unistd.h>
#include <sys/syscall.h>

#include "Singleton.h"
#include "Mutex.h"

#define MAX_FRAGMENT 102400
#define FRAGMENT_SIZE 2048
#define QUEUE_MAXLEN 102401
#define gettid() syscall(__NR_gettid)

namespace lpsys {

enum {
	LOG_LEVEL_ERROR = 0, LOG_LEVEL_WARNING = 1, LOG_LEVEL_INFO = 2, LOG_LEVEL_DEBUG = 3
};

enum {
	EMPTY = 0, WRITING = 1, FILLED = 2, OUTPUT = 3
};

static const char LOG_LEVEL_TAG[][10] = { "error", "warning", "info", "debug" };

typedef struct {
	long req_ltime;
	int cache_no;
	int status;
} CacheFragment;

#define LPSYS_LOGGER_MANAGER lpsys::LoggerManager::getInstance()
#define LPSYS_LOGGER_INIT(fname, level, rotate) LPSYS_LOGGER_MANAGER->init(fname, level, rotate)
#define LPSYS_LOGGER_DESTORY() lpsys::LoggerManager::release()

/**
 * 日志操作管理类
 */
class LoggerManager: public Singleton<LoggerManager> {
private:

	static char _cache[MAX_FRAGMENT][FRAGMENT_SIZE];
	static CacheFragment _queue[QUEUE_MAXLEN];
	static int _empty_head, _empty_tail, _filled_tail;

	static char _logName[128];
	static char _date[10];
	static FILE* _logfile;
	static bool _isRun;
	static bool _autoRotate;

	int m_level;

	void pprint(std::string op);

	Mutex m_mutex;
	pthread_t m_logWriter;

public:

	LoggerManager();
	~LoggerManager();

	inline int getLogLevel() {
		return m_level;
	}

	int requestFragment(int filled_no = -1);
	int releaseFragment(int empty_no = -1);
	void overFragment(int filled_no = -1);
	char* locateFragment(int frag_no);

	void init(const std::string fname, const std::string level, bool rotate = true);

	void createLog(unsigned int msecond = 0);bool createWriter();
	void stopWriter();
	static void *writingLog(void *arg);
};

#define LPSYS_LOGGER lpsys::Logger::getInstance()
#define LPLOG_ERROR(_fmt_, args...) LPSYS_LOGGER->error(_fmt_, ##args)
#define LPLOG_DEBUG(_fmt_, args...) LPSYS_LOGGER->debug(_fmt_, ##args)
#define LPLOG_INFO(_fmt_, args...) LPSYS_LOGGER->info(_fmt_, ##args)
#define LPLOG_WARNING(_fmt_, args...) LPSYS_LOGGER->warning(_fmt_, ##args)

class Logger: public Singleton<Logger> {
private:
	int m_frag_no;
	int m_level;
	char* pbuf;
	char* sbuf;bool m_delay;
	pid_t m_pid;

	Mutex m_mutex;
public:
	Logger(bool delay = false);
	~Logger();

	inline void setLogLevel(int lv) {
		m_level = lv;
	}

	void debug(const char *format, ...);
	void info(const char *format, ...);
	void warning(const char *format, ...);
	void error(const char *format, ...);

	void logPrint(int level, const char *format, va_list args);
	int my_vsnprintf(char *str, size_t size, const char *format, va_list ap);
};

//日志基类
class Logging {
protected:
	Logger *m_logger;
public:
	Logging() {
		m_logger = new Logger(false);
	}

	~Logging() {
		if (m_logger)
			delete m_logger;
	}
};

}

#endif /* LOGGER_H_ */
