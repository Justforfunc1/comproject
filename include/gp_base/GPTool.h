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

		函数名		：	formatTimeStr

		功能		：	格式化日期

		参数		：	str

		返回值		：	0：成功		other：失败

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPVOID formatTimeStr(GPChar *str);

};

}// namespace GPUniversal

#endif

