/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file logger.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-07-06
 ** \last modified 2017-08-07 16:58:28
**********************************************************/

#include "base/logger.h"
#include "base/string_util.h"
#include "base/exception_util.h"

namespace base {

CacheFragment LoggerManager::queue_[] = {};
char LoggerManager::cache_[][FRAGMENT_SIZE] = {};
char LoggerManager::log_name_[] = {};
char LoggerManager::date_[] = {};
FILE *LoggerManager::log_file_ = NULL;
bool LoggerManager::is_run_ = false;
bool LoggerManager::auto_rotate_ = true;
int32 LoggerManager::empty_head_ = 0;
int32 LoggerManager::empty_tail_ = 0;
int32 LoggerManager::filled_tail_ = 0;

LoggerManager::LoggerManager() {
	level_ = LOG_LEVEL_DEBUG;
	log_writer_ = 0;

	//初始：
	//	空队列接满队列
	//	空队列满，满队列空
	//	空队列尾指针为空，满尾为空尾
	for (int32 i = 0; i <= MAX_FRAGMENT; i++) {
		queue_[i].req_ltime = 0;
		queue_[i].cache_no = i;
		queue_[i].status = EMPTY;
	}

	empty_head_ = 0;
	empty_tail_ = MAX_FRAGMENT;
	filled_tail_ = empty_tail_;
}

LoggerManager::~LoggerManager() {
	StopWriter();
}

void LoggerManager::Init(const std::string file_name, const std::string level, bool rotate) {
	if (file_name == "") {
		log_name_[0] = '\0';
	} else {
		strncpy(log_name_, file_name.c_str(), 127);
	}
	if (level == "debug") {
		level_ = LOG_LEVEL_DEBUG;
	} else if (level == "info") {
		level_ = LOG_LEVEL_INFO;
	} else if (level == "warning") {
		level_ = LOG_LEVEL_WARNING;
	} else if (level == "error") {
		level_ = LOG_LEVEL_ERROR;
	} else {
		level_ = LOG_LEVEL_INFO;
	}

	auto_rotate_ = rotate;
	CreateWriter();
}

void LoggerManager::CreateLog(uint32 msecond) {
	if (msecond > 0) {
		usleep(msecond * 1000);
	}
	if (log_name_[0] == '\0') {
		log_file_ = stdout;
		return;
	}
	if (log_file_ != NULL && log_file_ != stdout) {
		fclose(log_file_);
	}
	char  buf[100];
	strcpy(buf, log_name_);
	base::utils::StringUtil::FormatTimeStr(buf);
	log_file_ = fopen(buf, "a+");
	if (log_file_ == NULL) {
		printf("create log[%s] error![errno=%d]", buf, errno);
		throw base::Exception(__FILE__, __LINE__, buf);
	}
	strcpy(date_, "YYYYMMDD");
	base::utils::StringUtil::FormatTimeStr(date_);
}

bool LoggerManager::CreateWriter() {
	CreateLog();
	is_run_ = true;

	try {
		int32 ret = pthread_create(&log_writer_, NULL, WritingLog, NULL);
		return ret == 0;
	} catch (...) {
		return false;
	}
}

void LoggerManager::StopWriter() {
	if (is_run_) {
		is_run_ = false;
		pthread_join(log_writer_, NULL);
	}
}

void *LoggerManager::WritingLog(void* arg) {
	int32 delay_times = -1;
	char * p;
	int32 frag_no = -1;
	while (1) {
		frag_no = LoggerManager::Instance().ReleaseFragment(frag_no);
		if (frag_no >= 0) { //输出
			p = LoggerManager::Instance().LocateFragment(frag_no);
			fputs(p, log_file_);
			fflush(log_file_);
		}
		if (!is_run_) { //进入停止
			if (delay_times > 0) { //延时退出
				delay_times--;
			} else if (delay_times == 0) {
				break;
			} else
				delay_times = 5;
		}
		if (frag_no < 0 && delay_times < 0) {
			if (log_file_ != stdout && auto_rotate_) {
				char  tmp[20];
				strcpy(tmp, "YYYYMMDD");
				base::utils::StringUtil::FormatTimeStr(tmp);
				if (strcmp(date_, tmp) != 0) { //切换新日志文件
					fprintf(log_file_, "<---- switch new Log! \n");
					LoggerManager::Instance().CreateLog();
					fprintf(log_file_, "----- start new Log! -----> \n");
				}
			}
			usleep(200000);
			fflush(stdout);
		}
	}
	fclose(log_file_);
	log_file_ = NULL;

	return (void *) 0;
}

int32 LoggerManager::RequestFragment(int32 filled_no) {
	long ltime = time(NULL);

	std::lock_guard<std::mutex> lck (mutex_);
	//允许忙，而请求失败
	if (filled_no >= 0) { //满队列不应为满，可加入
		CacheFragment& f_item = queue_[filled_tail_]; //加入
		f_item.req_ltime = ltime;
		f_item.cache_no = filled_no;
		f_item.status = FILLED;

		filled_tail_ = (filled_tail_ + 1) % QUEUE_MAXLEN; //移1

		//定位当前片
		int32 p = (filled_tail_ + 1) % QUEUE_MAXLEN;
		while (p != empty_head_) {
			if (queue_[p].cache_no == filled_no) {
				queue_[p].req_ltime = queue_[filled_tail_].req_ltime; //迁移原位使用碎片，保持尾指针为空节点
				queue_[p].cache_no = queue_[filled_tail_].cache_no;
				queue_[p].status = queue_[filled_tail_].status;
				break;
			}
			p = (p + 1) % QUEUE_MAXLEN;
		}

		queue_[filled_tail_].req_ltime = 0; //满队列尾节点初始化，暂不用
		queue_[filled_tail_].cache_no = MAX_FRAGMENT;
		queue_[filled_tail_].status = EMPTY;
	}
	if (empty_head_ != empty_tail_) { //空队列非空
		CacheFragment& item = queue_[empty_head_];
		empty_head_ = (empty_head_ + 1) % QUEUE_MAXLEN;
		item.req_ltime = ltime;
		item.status = WRITING;
		cache_[item.cache_no][0] = '\0';

		return item.cache_no;
	} else {
		return -1;
	}
}

int32 LoggerManager::ReleaseFragment(int32 empty_no) {
	long ltime = time(NULL);

	std::lock_guard<std::mutex> lck (mutex_);
	if (empty_no >= 0) { //入空队列,序号无需变化
		queue_[empty_tail_].req_ltime = 0;
		queue_[empty_tail_].status = EMPTY;
		empty_tail_ = (empty_tail_ + 1) % QUEUE_MAXLEN;
	}
	if (filled_tail_ != empty_tail_) { //满队列非空，返回待写节点
		queue_[empty_tail_].req_ltime = ltime;
		queue_[empty_tail_].status = OUTPUT;

		return queue_[empty_tail_].cache_no;
	} else {
		return -1;
	}
}

void LoggerManager::OverFragment(int32 filled_no) {
	long ltime = time(NULL);
	if (filled_no >= 0) { //满队列不应为满，可加入
		std::lock_guard<std::mutex> lck (mutex_);

		CacheFragment& f_item = queue_[filled_tail_]; //加入
		f_item.req_ltime = ltime;
		f_item.cache_no = filled_no;
		f_item.status = FILLED;
		filled_tail_ = (filled_tail_ + 1) % QUEUE_MAXLEN; //移1

		//定位当前片
		int32 p = (filled_tail_ + 1) % QUEUE_MAXLEN;
		while (p != empty_head_) {
			if (queue_[p].cache_no == filled_no) {
				queue_[p].req_ltime = queue_[filled_tail_].req_ltime; //迁移原位使用碎片，保持尾指针为空节点
				queue_[p].cache_no = queue_[filled_tail_].cache_no;
				queue_[p].status = queue_[filled_tail_].status;
				break;
			}
			p = (p + 1) % QUEUE_MAXLEN;
		}
		queue_[filled_tail_].req_ltime = 0; //满队列尾节点初始化
		queue_[filled_tail_].cache_no = MAX_FRAGMENT;
		queue_[filled_tail_].status = EMPTY;
	}
}

char * LoggerManager::LocateFragment(int32 frag_no) {
	return LoggerManager::cache_[frag_no];
}

Logger::Logger(bool delay)
	: frag_no_(-1) {
	LoggerManager* manager = &(LoggerManager::Instance());
	pid_ = gettid();
	level_ = manager->GetLogLevel();
	delay_ = delay;
	frag_no_ = manager->RequestFragment();
	if (frag_no_ >= 0) {
		pbuf_ = manager->LocateFragment(frag_no_);
		sbuf_ = pbuf_ + FRAGMENT_SIZE - 1;
	} else {
		pbuf_ = NULL;
		sbuf_ = NULL;
	}
}

Logger::~Logger() {
	if (frag_no_ >= 0) {
		LoggerManager* manager = &(LoggerManager::Instance());
		manager->OverFragment(frag_no_);
		frag_no_ = -1;
	}
}

void Logger::Error(const char  *format, ...) {
	if (level_ >= LOG_LEVEL_ERROR) {
		va_list args;
		va_start(args, format);
		LogPrint(LOG_LEVEL_ERROR, format, args);
		va_end(args);
	}
}

void Logger::Warning(const char  *format, ...) {
	if (level_ >= LOG_LEVEL_WARNING) {
		va_list args;
		va_start(args, format);
		LogPrint(LOG_LEVEL_WARNING, format, args);
		va_end(args);
	}
}

void Logger::Info(const char  *format, ...) {
	if (level_ >= LOG_LEVEL_INFO) {
		va_list args;
		va_start(args, format);
		LogPrint(LOG_LEVEL_INFO, format, args);
		va_end(args);
	}
}

void Logger::Debug(const char  *format, ...) {
	if (level_ >= LOG_LEVEL_DEBUG) {
		va_list args;
		va_start(args, format);
		LogPrint(LOG_LEVEL_DEBUG, format, args);
		va_end(args);
	}
}

void Logger::LogPrint(int32 level, const char  *format, va_list args) {
	if (frag_no_ < 0) { //太忙，不打印
		std::cout << "busy: " << frag_no_ << " " << level << std::endl;
		return;
	}

	std::lock_guard<std::mutex> lck (mutex_);

	if ((sbuf_ - pbuf_) < 512) { //空间不够，重新分配
		LoggerManager* manager = &(LoggerManager::Instance());
		frag_no_ = manager->RequestFragment(frag_no_);
		if (frag_no_ >= 0) {
			pbuf_ = manager->LocateFragment(frag_no_);
			sbuf_ = pbuf_ + FRAGMENT_SIZE - 1;
		} else
			return;
	}

	time_t rawtime;
	struct tm* curtime;
	time(&rawtime);
	curtime = localtime(&rawtime);

	int32 u_len = sprintf(pbuf_, "%4d-%02d-%02d %02d:%02d:%02d [%s] {%05d} ", curtime->tm_year + 1900, curtime->tm_mon + 1, curtime->tm_mday,
			curtime->tm_hour, curtime->tm_min, curtime->tm_sec, LOG_LEVEL_TAG[level], pid_);
	pbuf_ += (u_len > 0) ? u_len : 0;

	u_len = my_vsnprintf(pbuf_, sbuf_ - pbuf_ - 2, format, args);

	pbuf_ += u_len;
	*pbuf_ = '\n';
	pbuf_++;
	*pbuf_ = '\0';

	if (!delay_) {
		LoggerManager* manager = &(LoggerManager::Instance());
		frag_no_ = manager->RequestFragment(frag_no_);
		if (frag_no_ >= 0) {
			pbuf_ = manager->LocateFragment(frag_no_);
			sbuf_ = pbuf_ + FRAGMENT_SIZE - 1;
		}
	}
}

//支持变量类型 %% %c %s %d %ld %u %lu %f %lf
int32 Logger::my_vsnprintf(char  *str, size_t size, const char  *format, va_list ap) {
	enum {
		P_STR, P_VAR, P_TYPE_EXT, P_TYPE
	};
	int32 state = P_STR;
	size_t out_cnt = 0;
	int32 var_cnt = 0;
	char  type = '0';
	const char  *ch = format;
	char  *p = str;
	bool output;
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
			int32 out_len = size - 1 - out_cnt;
			char  tmp_buf[50];
			char  *tmp_s = tmp_buf;
			if (type == '0') { //
				*p = *ch;
				p++;
				out_cnt++;
				out_len = 0;
			} else if (type == 'c') { //单字符
				*p = va_arg(ap, int32);
				p++;
				out_cnt++;
				var_cnt++;
				out_len = 0;
			} else if (type == 's') { //字符串
				tmp_s = va_arg(ap, char  *);
				var_cnt++;
			} else if (type == '1') { //不支持变量
				tmp_buf[0] = '%';
				tmp_buf[1] = *ch;
				tmp_buf[2] = '\0';
				var_cnt++;
			} else if (type == 'd') { //有符号十进制数
				snprintf(tmp_buf, 50, "%d", va_arg(ap, int32));
				var_cnt++;
			} else if (type == 'D') { //
				snprintf(tmp_buf, 50, "%ld", va_arg(ap, long));
				var_cnt++;
			} else if (type == 'u') { //无符号十进制数
				snprintf(tmp_buf, 50, "%u", va_arg(ap, uint32));
				var_cnt++;
			} else if (type == 'U') { //
				snprintf(tmp_buf, 50, "%lu", va_arg(ap, unsigned long));
				var_cnt++;
			} else if (type == 'f' || type == 'F') { //浮点/双精
				snprintf(tmp_buf, 50, "%lf", va_arg(ap, double));
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

} //base
