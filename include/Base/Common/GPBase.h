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

#include "Base/Common/GPDefine.h"
#include "Base/Common/GPSingleton.h"
#include "Base/Utils/GPStringUtil.h"
#include "Base/Utils/GPEncrypt.h"
#include "Base/Utils/GPTool.h"
#include "Base/Template/GPMutex.h"
#include "Base/Log/GPLogger.h"
#include "Base/Config/GPConfig.h"

#endif

