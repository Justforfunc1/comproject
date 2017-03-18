/**********************************************************
 * \file GPBase.h
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#ifndef _GPBase_H
#define _GPBase_H


namespace GPBase {

class GPMutex;
class GPLoggerManager;
class GPLogger;
class GPLogging;
class GPTool;
class GPStringUtil;
class GPEncrypt;


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

#include "gp_base/GPDefine.h"
#include "gp_base/GPSingleton.h"
#include "gp_base/GPStringUtil.h"
#include "gp_base/GPEncrypt.h"
#include "gp_base/GPTool.h"
#include "gp_base/GPMutex.h"
#include "gp_base/GPLogger.h"
#include "gp_base/GPConfig.h"

#endif

