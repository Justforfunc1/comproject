
#ifndef _MQLOGGER_H_
#define _MQLOGGER_H_

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <errno.h>
#include <stdbool.h>
#include <stdarg.h>
#include <boost/thread/mutex.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

using namespace std;
using namespace boost::property_tree;

enum {LOG_LEVEL_ERROR=0, LOG_LEVEL_WARNING=1, LOG_LEVEL_INFO=2, LOG_LEVEL_DEBUG=3};
static const char LOG_LEVEL_TAG[][10]={ "error", "warning", "info" , "debug" };

class MQLogger{
private:
	static MQLogger* _instance;
    char m_logName[100];
    FILE* m_logfile;
    int m_level;
	
    boost::mutex m_mutex;
	
public:
	static MQLogger* getInstance() {
		if ( !_instance )
			_instance = new MQLogger();
		return _instance;
	}
	
	static void release() {
		if ( _instance ) {
			delete _instance;
			_instance = NULL;
		}
	}

	MQLogger():m_logfile(stdout),m_level(LOG_LEVEL_INFO){};
	~MQLogger(){ if(m_logfile!=stdout) fclose(m_logfile); };

	inline int getLogLevel() { return m_level; }
	inline void setLogLevel(int lv) { m_level = lv; }
	
	void createLog();
	
    bool init(const char* fname, const char* group_name);
	void formatTimeStr(char *buf);

    void debug(const char *format, ...);	
    void info(const char *format, ...);
    void warning(const char *format, ...);
    void error(const char *format, ...);
	
    void logPrint(int level,const char *format, va_list args);
};

//日志基类
class MQLogging{
protected:
	MQLogger *m_logger;
public:
	MQLogging() { m_logger = MQLogger::getInstance(); };
};
#endif
