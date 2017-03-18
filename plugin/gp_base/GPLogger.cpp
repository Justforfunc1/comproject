/**********************************************************
 * \file GPLogger.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include <unistd.h>
#include "gp_base/GPDefine.h"
#include "gp_base/GPLogger.h"
#include "gp_base/GPException.h"
#include "gp_base/GPStringUtil.h"
#include "gp_base/GPTool.h"

namespace GPBase {

GPChar GPLoggerManager::_cache[][FRAGMENT_SIZE] = { };
CacheFragment_t GPLoggerManager::_queue[] = { };

GPChar GPLoggerManager::_logName[] = { };
GPChar GPLoggerManager::_date[] = { };
FILE* GPLoggerManager::_logfile = NULL;

GPBOOL GPLoggerManager::_isRun = false;
GPBOOL GPLoggerManager::_autoRotate = true;
GPInt32 GPLoggerManager::_empty_head = 0;
GPInt32 GPLoggerManager::_empty_tail = 0;
GPInt32 GPLoggerManager::_filled_tail = 0;

GPLoggerManager::GPLoggerManager() {
	m_level = LOG_LEVEL_DEBUG;
	m_logWriter = 0;

	//初始：
	//	空队列接满队列
	//	空队列满，满队列空
	//	空队列尾指针为空，满尾为空尾
	for (GPInt32 i = 0; i <= MAX_FRAGMENT; i++) {
		_queue[i].req_ltime = 0;
		_queue[i].cache_no = i;
		_queue[i].status = EMPTY;
	}

	_empty_head = 0;
	_empty_tail = MAX_FRAGMENT;
	_filled_tail = _empty_tail;
}

GPLoggerManager::~GPLoggerManager() {
	stopWriter();
}

GPVOID GPLoggerManager::init(const GPString fname, const GPString level, GPBOOL rotate) {
	if (fname == "") {
		_logName[0] = '\0';
	} else {
		strncpy(_logName, fname.c_str(), 127);
	}

	if (level == "debug") {
		m_level = LOG_LEVEL_DEBUG;
	} else if (level == "info") {
		m_level = LOG_LEVEL_INFO;
	} else if (level == "warning") {
		m_level = LOG_LEVEL_WARNING;
	} else if (level == "error") {
		m_level = LOG_LEVEL_ERROR;
	} else
		m_level = LOG_LEVEL_INFO;

	_autoRotate = rotate;

	_instance->createWriter();
}

GPVOID GPLoggerManager::createLog(GPUInt32 msecond) {
	if (msecond > 0) {
		usleep(msecond * 1000);
	}

	if (_logName[0] == '\0') {
		_logfile = stdout;
		return;
	}
	if (_logfile != NULL && _logfile != stdout) {
		fclose(_logfile);
	}

	GPChar buf[100];
	strcpy(buf, _logName);
	GPUniversal::GPTool::formatTimeStr(buf);
	_logfile = fopen(buf, "a+");
	if (_logfile == NULL) {
		printf("create log[%s] error![errno=%d]", buf, errno);
		throw GPBase::GPException(__FILE__, __LINE__, buf);
	}

	strcpy(_date, "YYYYMMDD");
	GPUniversal::GPTool::formatTimeStr(_date);
}

//启动日志输出线程
GPBOOL GPLoggerManager::createWriter() {
	_instance->createLog();

	_isRun = true;

	try {
		GPInt32 ret = pthread_create(&m_logWriter, NULL, writingLog, NULL);
		return ret == 0;
	} catch (...) {
		return false;
	}
}

//等待日志输出线程退出
GPVOID GPLoggerManager::stopWriter() {
	if (_isRun) {
		_isRun = false;
		pthread_join(m_logWriter, NULL);
	}
}

//日志输出
GPVOID *GPLoggerManager::writingLog(GPVOID* arg) {
	GPInt32 delay_times = -1;
	GPChar* p;
	GPInt32 frag_no = -1;
	while (1) {
		frag_no = _instance->releaseFragment(frag_no);
		if (frag_no >= 0) { //输出
			p = _instance->locateFragment(frag_no);
			fputs(p, _logfile);
			fflush(_logfile);
		}
		if (!_isRun) { //进入停止
			if (delay_times > 0) { //延时退出
				delay_times--;
			} else if (delay_times == 0) {
				break;
			} else
				delay_times = 5;
		}

		if (frag_no < 0 && delay_times < 0) {
			if (_logfile != stdout && _autoRotate) {
				GPChar tmp[20];
				strcpy(tmp, "YYYYMMDD");
				GPUniversal::GPTool::formatTimeStr(tmp);
				if (strcmp(_date, tmp) != 0) { //切换新日志文件
					fprintf(_logfile, "<---- switch new Log! \n");
					_instance->createLog();
					fprintf(_logfile, "----- start new Log! -----> \n");
				}
			}
			usleep(200000);
			fflush(stdout);
		}
	}
	fclose(_logfile);
	_logfile = NULL;

	return (GPVOID *) 0;
}

//请求碎片
GPInt32 GPLoggerManager::requestFragment(GPInt32 filled_no) {
	long ltime = time(NULL);

	GPMutex::GPLock lock(m_mutex);

	//允许忙，而请求失败
	if (filled_no >= 0) { //满队列不应为满，可加入
		CacheFragment_t& f_item = _queue[_filled_tail]; //加入
		f_item.req_ltime = ltime;
		f_item.cache_no = filled_no;
		f_item.status = FILLED;

		_filled_tail = (_filled_tail + 1) % QUEUE_MAXLEN; //移1

		//定位当前片
		GPInt32 p = (_filled_tail + 1) % QUEUE_MAXLEN;
		while (p != _empty_head) {
			if (_queue[p].cache_no == filled_no) {
				_queue[p].req_ltime = _queue[_filled_tail].req_ltime; //迁移原位使用碎片，保持尾指针为空节点
				_queue[p].cache_no = _queue[_filled_tail].cache_no;
				_queue[p].status = _queue[_filled_tail].status;
				break;
			}
			p = (p + 1) % QUEUE_MAXLEN;
		}

		_queue[_filled_tail].req_ltime = 0; //满队列尾节点初始化，暂不用
		_queue[_filled_tail].cache_no = MAX_FRAGMENT;
		_queue[_filled_tail].status = EMPTY;
	}

	if (_empty_head != _empty_tail) { //空队列非空
		CacheFragment_t& item = _queue[_empty_head];
		_empty_head = (_empty_head + 1) % QUEUE_MAXLEN;
		item.req_ltime = ltime;
		item.status = WRITING;
		_cache[item.cache_no][0] = '\0';

		return item.cache_no;
	} else {
		return -1;
	}
}

//释放碎片，单线程调用
GPInt32 GPLoggerManager::releaseFragment(GPInt32 empty_no) {
	long ltime = time(NULL);

	GPMutex::GPLock lock(m_mutex);
	if (empty_no >= 0) { //入空队列,序号无需变化
		_queue[_empty_tail].req_ltime = 0;
		_queue[_empty_tail].status = EMPTY;
		_empty_tail = (_empty_tail + 1) % QUEUE_MAXLEN;
	}

	if (_filled_tail != _empty_tail) { //满队列非空，返回待写节点
		_queue[_empty_tail].req_ltime = ltime;
		_queue[_empty_tail].status = OUTPUT;

		return _queue[_empty_tail].cache_no;
	} else {
		return -1;
	}
}

//交还碎片
GPVOID GPLoggerManager::overFragment(GPInt32 filled_no) {
	long ltime = time(NULL);
	if (filled_no >= 0) { //满队列不应为满，可加入
		GPMutex::GPLock lock(m_mutex);

		CacheFragment_t& f_item = _queue[_filled_tail]; //加入
		f_item.req_ltime = ltime;
		f_item.cache_no = filled_no;
		f_item.status = FILLED;

		_filled_tail = (_filled_tail + 1) % QUEUE_MAXLEN; //移1

		//定位当前片
		GPInt32 p = (_filled_tail + 1) % QUEUE_MAXLEN;
		while (p != _empty_head) {
			if (_queue[p].cache_no == filled_no) {
				_queue[p].req_ltime = _queue[_filled_tail].req_ltime; //迁移原位使用碎片，保持尾指针为空节点
				_queue[p].cache_no = _queue[_filled_tail].cache_no;
				_queue[p].status = _queue[_filled_tail].status;
				break;
			}
			p = (p + 1) % QUEUE_MAXLEN;
		}

		_queue[_filled_tail].req_ltime = 0; //满队列尾节点初始化
		_queue[_filled_tail].cache_no = MAX_FRAGMENT;
		_queue[_filled_tail].status = EMPTY;
	}
}

GPChar* GPLoggerManager::locateFragment(GPInt32 frag_no) {
	return GPLoggerManager::_cache[frag_no];
}

GPLogger::GPLogger(GPBOOL delay) :
		m_frag_no(-1) {
	GPLoggerManager* manager = GPLoggerManager::getInstance();
	m_pid = gettid();
	m_level = manager->getLogLevel();
	m_delay = delay;
	m_frag_no = manager->requestFragment();
	if (m_frag_no >= 0) {
		pbuf = manager->locateFragment(m_frag_no);
		sbuf = pbuf + FRAGMENT_SIZE - 1;
	} else {
		pbuf = NULL;
		sbuf = NULL;
	}
}

GPLogger::~GPLogger() {
	if (m_frag_no >= 0) {
		GPLoggerManager* manager = GPLoggerManager::getInstance();
		manager->overFragment(m_frag_no);
		m_frag_no = -1;
	}
}

GPVOID GPLogger::error(const GPChar *format, ...) {
	if (m_level >= LOG_LEVEL_ERROR) {
		va_list args;
		va_start(args, format);
		logPrint(LOG_LEVEL_ERROR, format, args);
		va_end(args);
	}
}

GPVOID GPLogger::warning(const GPChar *format, ...) {
	if (m_level >= LOG_LEVEL_WARNING) {
		va_list args;
		va_start(args, format);
		logPrint(LOG_LEVEL_WARNING, format, args);
		va_end(args);
	}
}

GPVOID GPLogger::info(const GPChar *format, ...) {
	if (m_level >= LOG_LEVEL_INFO) {
		va_list args;
		va_start(args, format);
		logPrint(LOG_LEVEL_INFO, format, args);
		va_end(args);
	}
}

GPVOID GPLogger::debug(const GPChar *format, ...) {
	if (m_level >= LOG_LEVEL_DEBUG) {
		va_list args;
		va_start(args, format);
		logPrint(LOG_LEVEL_DEBUG, format, args);
		va_end(args);
	}
}

GPVOID GPLogger::logPrint(GPInt32 level, const GPChar *format, va_list args) {
	if (m_frag_no < 0) { //太忙，不打印
		std::cout << "busy: " << m_frag_no << " " << level << std::endl;
		return;
	}

	GPMutex::GPLock lock(m_mutex);

	if ((sbuf - pbuf) < 512) { //空间不够，重新分配
		GPLoggerManager* manager = GPLoggerManager::getInstance();
		m_frag_no = manager->requestFragment(m_frag_no);
		if (m_frag_no >= 0) {
			pbuf = manager->locateFragment(m_frag_no);
			sbuf = pbuf + FRAGMENT_SIZE - 1;
		} else
			return;
	}

	time_t rawtime;
	struct tm* curtime;
	time(&rawtime);
	curtime = localtime(&rawtime);

	GPInt32 u_len = sprintf(pbuf, "%4d-%02d-%02d %02d:%02d:%02d [%s] {%05d} ", curtime->tm_year + 1900, curtime->tm_mon + 1, curtime->tm_mday,
			curtime->tm_hour, curtime->tm_min, curtime->tm_sec, LOG_LEVEL_TAG[level], m_pid);
	pbuf += (u_len > 0) ? u_len : 0;

	u_len = my_vsnprintf(pbuf, sbuf - pbuf - 2, format, args);

	pbuf += u_len;
	*pbuf = '\n';
	pbuf++;
	*pbuf = '\0';

	if (!m_delay) {
		GPLoggerManager* manager = GPLoggerManager::getInstance();
		m_frag_no = manager->requestFragment(m_frag_no);
		if (m_frag_no >= 0) {
			pbuf = manager->locateFragment(m_frag_no);
			sbuf = pbuf + FRAGMENT_SIZE - 1;
		}
	}
}

//支持变量类型 %% %c %s %d %ld %u %lu %f %lf
GPInt32 GPLogger::my_vsnprintf(GPChar *str, size_t size, const GPChar *format, va_list ap) {
	enum {
		P_STR, P_VAR, P_TYPE_EXT, P_TYPE
	};
	GPInt32 state = P_STR;
	size_t out_cnt = 0;
	GPInt32 var_cnt = 0;
	GPChar type = '0';
	const GPChar *ch = format;
	GPChar *p = str;
	GPBOOL output;
	while (*ch != '\0') {
		output = false;
		if (*ch == '%') { //变量
			if (state == P_VAR) { //%%
				state = P_STR;
				type = '0';
				output = true;
			} else if (state == P_STR) { //_%
				state = P_VAR;
				type = '\0';
			}
		} else if (state == P_VAR) { //
			if (*ch == 'l') { //%l_
				type = 'l';
			} else if (*ch == 'c' || *ch == 's' || *ch == 'd' || *ch == 'u' || *ch == 'f') { //%_
				if (type == 'l') {
					type = *ch - 'a' + 'A';
				} else {
					type = *ch;
				}
				state = P_STR;
				output = true;
			} else {
				state = P_STR;
				type = '1';
				output = true;
			}
		} else {
			state = P_STR;
			type = '0';
			output = true;
		}

		if (output) {  //输出
			GPInt32 out_len = size - 1 - out_cnt;
			GPChar tmp_buf[50];
			GPChar *tmp_s = tmp_buf;
			if (type == '0') { //
				*p = *ch;
				p++;
				out_cnt++;
				out_len = 0;
			} else if (type == 'c') { //单字符
				*p = va_arg(ap, GPInt32);
				p++;
				out_cnt++;
				var_cnt++;
				out_len = 0;
			} else if (type == 's') { //字符串
				tmp_s = va_arg(ap, GPChar *);
				var_cnt++;
			} else if (type == '1') { //不支持变量
				tmp_buf[0] = '%';
				tmp_buf[1] = *ch;
				tmp_buf[2] = '\0';
				var_cnt++;
			} else if (type == 'd') { //有符号十进制数
				snprintf(tmp_buf, 50, "%d", va_arg(ap, GPInt32));
				var_cnt++;
			} else if (type == 'D') { //
				snprintf(tmp_buf, 50, "%ld", va_arg(ap, long));
				var_cnt++;
			} else if (type == 'u') { //无符号十进制数
				snprintf(tmp_buf, 50, "%u", va_arg(ap, GPUInt32));
				var_cnt++;
			} else if (type == 'U') { //
				snprintf(tmp_buf, 50, "%lu", va_arg(ap, unsigned long));
				var_cnt++;
			} else if (type == 'f' || type == 'F') { //浮点/双精
				snprintf(tmp_buf, 50, "%lf", va_arg(ap, GPDouble));
				var_cnt++;
			} else {
				type = '\0';
				out_len = 0;
			}

			while (out_len > 0 && *tmp_s != '\0') {
				*p = *tmp_s;
				p++;
				tmp_s++;
				out_cnt++;
				out_len--;
			}

			if (out_cnt + 1 >= size) {
				break;
			}
		}
		ch++;
	}
	str[out_cnt] = '\0';
	return out_cnt;
}

}

