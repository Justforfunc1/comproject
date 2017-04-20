/**********************************************************
 * \file GPLogger.h
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#ifndef _GPLogger_H
#define _GPLogger_H

#include <stdarg.h>
#include <linux/unistd.h>
#include <sys/syscall.h>

#include "Base/Common/GPSingleton.h"
#include "Base/Common/GPDefine.h"
#include "Base/Template/GPMutex.h"

#define MAX_FRAGMENT 102400
#define FRAGMENT_SIZE 2048
#define QUEUE_MAXLEN 102401
#define gettid() syscall(__NR_gettid)

namespace GPBase {

enum {
	LOG_LEVEL_ERROR = 0, LOG_LEVEL_WARNING = 1, LOG_LEVEL_INFO = 2, LOG_LEVEL_DEBUG = 3
};

enum {
	EMPTY = 0, WRITING = 1, FILLED = 2, OUTPUT = 3
};

static const GPChar LOG_LEVEL_TAG[][10] = { "error", "warning", "info", "debug" };

typedef struct {
	long req_ltime;
	GPInt32 cache_no;
	GPInt32 status;
} CacheFragment_t;

#define GPBASE_GPLOGGER_MANAGER GPBase::GPLoggerManager::getInstance()
#define GPBASE_GPLOGGER_INIT(fname, level, rotate) GPBASE_GPLOGGER_MANAGER->init(fname, level, rotate)
#define GPBASE_GPLOGGER_DESTORY() GPBase::GPLoggerManager::release()



/**********************************************************

	GPLoggerManager		日志操作管理类

	类名		：	GPLoggerManager

	作者		：	Allen.L

	创建时间	：

	类描述		：	日志管理

**********************************************************/

class GPLoggerManager: public GPSingleton<GPLoggerManager> {
private:

	static GPChar _cache[MAX_FRAGMENT][FRAGMENT_SIZE];
	static CacheFragment_t _queue[QUEUE_MAXLEN];
	static GPInt32 _empty_head, _empty_tail, _filled_tail;

	static GPChar _logName[128];
	static GPChar _date[10];
	static FILE* _logfile;
	static GPBOOL _isRun;
	static GPBOOL _autoRotate;

	GPInt32 m_level;

	GPVOID pprint(GPString op);

	GPMutex m_mutex;
	pthread_t m_logWriter;

public:

	GPLoggerManager();
	~GPLoggerManager();

	inline GPInt32 getLogLevel() {
		return m_level;
	}


	/**********************************************************

		函数名		：	requestFragment

		功能		：	请求碎片

		参数		：	filled 已用碎片。首次请求，缺省NULL

		返回值		：	可用碎片

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPInt32 requestFragment(GPInt32 filled_no = -1);


	/**********************************************************

		函数名		：	releaseFragment

		功能		：	释放碎片，单线程调用

		参数		：

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPInt32 releaseFragment(GPInt32 empty_no = -1);


	/**********************************************************

		函数名		：	overFragment

		功能		：	交还碎片

		参数		：

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPVOID overFragment(GPInt32 filled_no = -1);


	/**********************************************************

		函数名		：	locateFragment

		功能		：	返回碎片位置

		参数		：

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPChar* locateFragment(GPInt32 frag_no);


	/**********************************************************

		函数名		：	init

		功能		：	初始化日志管理

		参数		：

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPVOID init(const GPString fname, const GPString level, GPBOOL rotate = true);


	/**********************************************************

		函数名		：	createLog

		功能		：	启动日志输出线程

		参数		：

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPVOID createLog(GPUInt32 msecond = 0);GPBOOL createWriter();


	/**********************************************************

		函数名		：	stopWriter

		功能		：	等待日志输出线程退出

		参数		：

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPVOID stopWriter();


	/**********************************************************

		函数名		：	writingLog

		功能		：	日志输出

		参数		：

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPVOID *writingLog(GPVOID *arg);
};

#define GPBASE_GPLOGGER GPBase::GPLogger::getInstance()
#define GPLOG_ERROR(_fmt_, args...) GPBASE_GPLOGGER->error(_fmt_, ##args)
#define GPLOG_DEBUG(_fmt_, args...) GPBASE_GPLOGGER->debug(_fmt_, ##args)
#define GPLOG_INFO(_fmt_, args...) GPBASE_GPLOGGER->info(_fmt_, ##args)
#define GPLOG_WARNING(_fmt_, args...) GPBASE_GPLOGGER->warning(_fmt_, ##args)



/**********************************************************

	GPLogger			日志类

	类名			：	GPLogger

	作者			：	Allen.L

	创建时间		：

	类描述			：	日志操作

**********************************************************/

class GPLogger: public GPSingleton<GPLogger> {
private:
	GPInt32 m_frag_no;
	GPInt32 m_level;
	GPChar* pbuf;
	GPChar* sbuf;GPBOOL m_delay;
	pid_t m_pid;

	GPMutex m_mutex;
public:
	GPLogger(GPBOOL delay = false);
	~GPLogger();

	inline GPVOID setLogLevel(GPInt32 lv) {
		m_level = lv;
	}


	/**********************************************************

		函数名		：	debug	info	warning		error

		功能		：	日志级别输出

		参数		：

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPVOID debug(const GPChar *format, ...);
	GPVOID info(const GPChar *format, ...);
	GPVOID warning(const GPChar *format, ...);
	GPVOID error(const GPChar *format, ...);


	/**********************************************************

		函数名		：	logPrint

		功能		：	日志输出函数

		参数		：

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPVOID logPrint(GPInt32 level, const GPChar *format, va_list args);
	GPInt32 my_vsnprintf(GPChar *str, size_t size, const GPChar *format, va_list ap);
};



/**********************************************************

	GPLogging			日志基类

	类名		：		GPLogging

	作者		：		Allen.L

	创建时间	：

	类描述		：

**********************************************************/

//日志基类
class GPLogging {
protected:
	GPLogger *m_logger;
public:
	GPLogging() {
		m_logger = new GPLogger(false);
	}

	~GPLogging() {
		if (m_logger)
			delete m_logger;
	}
};

}

#endif

