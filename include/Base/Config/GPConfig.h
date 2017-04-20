/**********************************************************
 * \file GPConfig.h
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#ifndef _GPConfig_H
#define _GPConfig_H

#include "Base/Common/GPDefine.h"
#include <ext/hash_map>

#define GPBASE_CONFIG GPBase::GPConfig::getConfig()
#define GPCONFIG_LOAD(filename) GPBASE_CONFIG.load(filename)
#define GPCONFIG_GETSTR(section, key, d) GPBASE_CONFIG.getString(section, key, d)
#define GPCONFIG_GETINT(section, key, d) GPBASE_CONFIG.getInt(section, key, d)

namespace GPBase {

struct str_hash {
	size_t operator()(const GPString& str) const {
		return HASH_NAME::__stl_hash_string(str.c_str());
	}
};


//字符串比较
struct char_equal {
	bool operator()(const GPChar* s1, const GPChar* s2) const {
		return strcmp(s1, s2) == 0;
	}
};


typedef HASH_NAME ::hash_map<GPString, GPString, str_hash> STR_STR_MAP;
typedef STR_STR_MAP::iterator STR_STR_MAP_ITER;
typedef HASH_NAME ::hash_map<GPString, STR_STR_MAP*, str_hash> STR_MAP;
typedef STR_MAP::iterator STR_MAP_ITER;



/**********************************************************

	GPConfig		读取配置文件类

	类名		：	GPConfig

	作者		：	Allen.L

	创建时间	：

	类描述		：	解析配置文件,并将配置项以key-value的形式存储到内存中

**********************************************************/

class GPConfig {
public:
	GPConfig();
	~GPConfig();

	/**********************************************************

		函数名		：	load

		功能		：	加载文件

		参数		：	char*	filename 文件名

		返回值		：	GPInt32	 0:表示成功    -1：表示失败

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPInt32 load(const GPChar *filename);


	/**********************************************************

		函数名		：	getString

		功能		：	取一个字符串

		参数		：	char*	section 	标识
		参数		：	string	key 		键
		参数		：	char*	d 		初始值

		返回值		：	GPChar *

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	const GPChar *getString(const GPChar *section, const GPString& key, const GPChar *d = NULL);


	/**********************************************************

		函数名		：	getStringList

		功能		：	取一string列表

		参数		：	char*	section 	标识
		参数		：	string	key 	键

		返回值		：std::vector<const GPChar*>

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	std::vector<const GPChar*> getStringList(const GPChar *section, const GPString& key);


	/**********************************************************

		函数名		：	getInt

		功能		：	取一个整型

		参数		：	char*	section 标识
		参数		：	string	key 键
		参数		：	int		d 初始值

		返回值		：	GPInt32

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPInt32 getInt(GPChar const *section, const GPString& key, GPInt32 d = 0);


	/**********************************************************

		函数名		：	getIntList

		功能		：	取一整型列表

		参数		：	char*	section 标识
		参数		：	string	key 键

		返回值		：GPInt32Vector

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPInt32Vector getIntList(const GPChar *section, const GPString& key);


	/**********************************************************

		函数名		：	getSectionKey

		功能		：	取一section下所有的key

		参数		：	section 标识
		参数		：	key 键

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPInt32 getSectionKey(const GPChar *section, GPStringVector &keys);


	/**********************************************************

		函数名		：	getSectionName
		函数名		：	toString

		功能		：	得到所有section的名字
		功能		：	完整配置文件转string

		参数		：	sections 标识

		返回值		：GPInt32

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPInt32 getSectionName(GPStringVector &sections);
	GPString toString();


	static GPConfig& getConfig();

private:
	// 两层map
	STR_MAP m_configMap;

private:


	/**********************************************************

		函数名		：	parseValue

		功能		：	解析字符串

		参数		：	char *str 源数据
		参数		：	char *key	键
		参数		：	char *val	值

		返回值		：GPInt32	0：成功		other：失败

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPInt32 parseValue(GPChar *str, GPChar *key, GPChar *val);


	/**********************************************************

		函数名		：	isSectionName

		功能		：	段名判断

		参数		：	char *str 源数据

		返回值		：	GPChar *

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPChar *isSectionName(GPChar *str);
};

}

#endif

