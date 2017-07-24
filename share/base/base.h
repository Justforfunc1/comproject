/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file base.h
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-07-18
 ** \last modified 2017-07-19 15:47:08
**********************************************************/
#ifndef _BASE_H
#define _BASE_H

namespace base {

class Logging;
class LoggerManger;
class Logger;
class Config;
class StringUtil;
class Encrypt;
class Exception;

class Noncopyable {
 protected:
  Noncopyable() {}
  ~Noncopyable() {}
 private:
  Noncopyable(const Noncopyable&);
  const Noncopyable& operator=(const Noncopyable&);
};

inline int GetSystemErrno() {
	return errno;
}

} //base

#include "base/define.h"
#include "base/config.h"
#include "base/logger.h"
#include "base/encrypt.h"
#include "base/singleton.h"
#include "base/string_util.h"
#include "base/json_util.h"

#endif

