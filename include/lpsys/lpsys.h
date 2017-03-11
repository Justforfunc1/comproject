/**********************************************************
 *  \file lpsys.h
 *  \brief	
 *  \note	×¢ÒâÊÂÏî£º 
 * 
 * \version 
 * * \author Allen.L
 ***********************************************************/

#ifndef LPSYS_H_
#define LPSYS_H_

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <queue>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>

#ifndef UNUSED
#define UNUSED(v) ((void)(v))
#endif

typedef unsigned char Byte;
typedef short Short;
typedef int Int;
typedef long long Long;
typedef float Float;
typedef double Double;

typedef ::std::vector<bool> BoolSeq;
typedef ::std::vector<Byte> ByteSeq;
typedef ::std::vector<Short> ShortSeq;
typedef ::std::vector<Int> IntSeq;
typedef ::std::vector<Long> LongSeq;
typedef ::std::vector<Float> FloatSeq;
typedef ::std::vector<Double> DoubleSeq;
typedef ::std::vector< ::std::string> StringSeq;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef u_int8_t uint8;
typedef u_int16_t uint16;
typedef u_int32_t uint32;
typedef u_int64_t uint64;

typedef struct timeval time_cnt;

namespace lpsys {

class StringUtil;
class Mutex;
class LoggerManager;
class Logger;
class Logging;

class noncopyable {
protected:

	noncopyable() {
	}
	~noncopyable() {
	}
private:

	noncopyable(const noncopyable&);
	const noncopyable& operator=(const noncopyable&);
};

inline int getSystemErrno() {
	return errno;
}

}

#include "Singleton.h"
#include "StringUtil.h"
#include "Mutex.h"
#include "Logger.h"
#include "Config.h"

#endif /* LPSYS_H_ */
