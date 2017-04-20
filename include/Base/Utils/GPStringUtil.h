/**********************************************************
 * \file GPStringUtil.h
 * \brief	字符串的通用函数类
 * \note	注意事项：尽可能使用C++标准库的函数，但不保证windows下100%可用（未在window下测试）
 *					函数集 声明普通static 函数 生成动态库时编译截断  /usr/bin/ld: final link failed: Bad value
 *					推荐静态类的成员函数封装方式
 *\version 
 ** \author Allen.L
**********************************************************/

#ifndef _GPStringUtil_H
#define _GPStringUtil_H

#include "Base/Common/GPDefine.h"

namespace GPUniversal {

//	给str增加单引号(主要用于SQL)
inline GPString quotedStr(const GPString &str){ return "'" + str + "'"; }
inline GPString doubleQuotedStr(const GPString &str){ return "\"" + str + "\""; }


/**********************************************************

	GPStringUtil	字符操作类

	类名		：	GPStringUtil

	作者		：	Allen.L

	创建时间	：

	类描述		：	字符的相关操作

**********************************************************/

class GPStringUtil {

public:


	/**********************************************************

		函数名		：	split

		功能		：	把string以delim分隔开,放到list中

		参数		：	char*	src		源字符串
		参数		：	char*	delim	目标字符串
		参数		：	char*	list	输出list

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPVOID split(GPChar *str, const GPChar *delim, std::vector<GPChar*> &list);


	/**********************************************************

		函数名		：	strReplaceAll

		功能		：	替换字符串中对应的子字符串

		参数		：	string	content		替换的字符串
		参数		：	string	replace		被替换的子字符串
		参数		：	string*	dest		换入的子字符串

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPBOOL strReplaceAll(GPString &content, const GPString &replace, const GPString &dest);


	/**********************************************************

		函数名		：	splitString

		功能		：	通过分割符来分割字符串，并将分割好的数据存在数组中
					当输入字符串为空时，结果会返回一个空值的字符串

		参数		：	string	&strValue		替换的字符串
		参数		：	string	&splittingKey	分隔符
		参数		：	GPStringVector	&result		输出的Vector字符串
		参数		：	std::list<GPString> &result		输出的List字符串

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPBOOL splitString(const GPString &strValue, const GPString &splittingKey, GPStringVector &result);
	static GPBOOL splitString(const GPString &strValue, const GPString &splittingKey, std::list<GPString> &result);


	/**********************************************************

		函数名		：	mergeString

		功能		：	合并字符串。使用分隔符分隔。

		参数		：	GPStringVector	&strings	合并字符串Vector
		参数		：	string	&mergerKey	分隔符

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPString mergeString(const GPStringVector &strings, const GPString &mergerKey);


	/**********************************************************

		函数名		：	trim

		功能		：	删除左边(右边)的空白符，制表符，换行符
					不处理中间空格，但会删除所有的制表符和换行符

		参数		：	string	&str	输出字符串

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPString trimRight(const GPString &str);
	static GPString trimLeft(const GPString &str);
	static GPString trim(const GPString &str);


	/**********************************************************

		函数名		：	*trim

		功能		：	去前后空格

		参数		：	string	&str	输入字符串
		参数		：	char*	&what	替换为空字符
		参数		：	GPInt32 mode	模式

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPChar *trim(GPChar *str, const GPChar *what = " ", GPInt32 mode = 3);


	/**********************************************************

		函数名		：	strToXmlStr
		函数名		：	xmlStrToStr

		功能		：	字符串中XML的关键符号转换为实体
		功能		：	字符串中XML的实体转换为常规符号

		参数		：	string	&content	输入输出字符串

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPString strToXmlStr(GPString content);
	static GPString xmlStrToStr(GPString content);


	/**********************************************************

		函数名		：	strToJsonStr
		函数名		：	jsonStrToStr

		功能		：	字符串中json的关键符号转换为实体
		功能		：	字符串中json的实体转换为常规符号

		参数		：	string	&content	输入输出字符串

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPString strToJsonStr(GPString content);
	static GPString jsonStrToStr(GPString content);


	/**********************************************************

		函数名		：	isInt
		函数名		：	intToStr
		函数名		：	strToInt
		函数名		：	strToInt
		函数名		：	strToDouble
		函数名		：	doubleToStr
		函数名		：	checkNumber

		功能		：	int类型判断
		功能		：	int与string类型互转
		功能		：	long型数值转string
		功能		：	double与string类型互转
		功能		：	参数类型检测

		参数		：	byte	&	输入输出字符串

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPInt32 isInt(const GPChar *str);
	static GPString intToStr(const long &number);
	static GPInt32 strToInt(const GPChar *str, GPInt32 d);
	static long strToInt(const GPString &number);
	static GPDouble strToDouble(const GPString &number);
	static GPString doubleToStr(const GPDouble &number);
	static GPBOOL checkNumber(const GPString &number);


	/**********************************************************

		函数名		：	*strToLower strToLower
		函数名		：	*strToUpper strToUpper

		功能		：	字符串大小写互转

		参数		：	string	&str	输入输出字符串
		参数		：	char*	&str	输入输出字符串

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPChar *strToLower(GPChar *str);
	static GPString strToLower(const GPString &str);
	static GPChar *strToUpper(GPChar *str);
	static GPString strToUpper(const GPString &str);

};

}	//namespace  GPUtil

#endif

