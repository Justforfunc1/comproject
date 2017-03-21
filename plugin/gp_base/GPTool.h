/**********************************************************
 * \file GPTool.h
 * \brief	通用工具函数类
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#ifndef _GPTool_H
#define _GPTool_H

#include "gp_base/GPDefine.h"

namespace GPUniversal{



/**********************************************************

	GPTool			通用工具类

	类名		：	GPTool

	作者		：	Allen.L

	创建时间	：

	类描述		：

**********************************************************/

class GPTool{

public:


	/**********************************************************

		函数名		：	murMurHash

		功能		：	比较好的hash算法	http://murmurhash.googlepages.com/

		参数		：	void	key
		参数		：	int		len

		返回值		：	0：成功		other：失败

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPUInt32 murMurHash(const GPVOID *key, GPInt32 len);


	/**********************************************************

		函数名		：	hashCode

		功能		：	获取str的hash值

		参数		：	char*	str

		返回值		：	0：成功		other：失败

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPInt32 hashCode(const GPChar *str);


	/**********************************************************

		函数名		：	getPrimeHash

		功能		：	 获取一个str的hash值的素数

		参数		：	str

		返回值		：	0：成功		other：失败

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPInt32 getPrimeHash(const GPChar *str);


	/**********************************************************

		函数名		：	formatByteSize

		功能		：	格式化	把bytes转成可读的, 如 10K 12M 等

		参数		：	bytes

		返回值		：	0：成功		other：失败

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPString formatByteSize(GPDouble bytes);


	/**********************************************************

				时间相关处理函数

		函数名		：	formatTimeStr
		函数名		：	parseTime
		函数名		：	parseDateTime
		函数名		：	getWeek

		功能		：	格式化日期（YYYYMMDDHHMISS）固定格式
		功能		：	获取HHMISS时间戳
		功能		：	获取当日时间戳（毫秒）
		功能		：	获取周期

		参数		：	GPChar *	str

		返回值		：	0：成功		other：失败

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPVOID formatTimeStr(GPChar *str);
	static long parseTime(const GPChar *format, const GPChar *buf);
	static long parseDateTime(const GPChar *format, GPChar* buf);
	static GPString getWeek();


	/**********************************************************

		函数名		：	execShellCmd
		函数名		：	getIp

		功能		：	执行shell脚本
		功能		：	获取本机IP： 支持IPV4 支持window and linux	（window版本需要定义宏 WIN32）

		参数		：	GPString	cmd
		参数		：	GPString	ip

		返回值		：	GPBOOL

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	#ifdef LINUX
	static GPBOOL execShellCmd( const GPString &cmd );
	#endif

	GPVOID getIp(GPString &ip);


};

}// namespace GPUniversal

#endif

