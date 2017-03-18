/**********************************************************
 * \file GPJosn.h
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/
#ifndef _GPJosn_H
#define _GPJosn_H

#include <algorithm>
#include "json/json.h"

using namespace std;
using namespace Json;

namespace lpsys {




/**********************************************************

	JsonTool			JSON的处理类。基于JSONCPP库。

	类名		：	JsonTool

	作者		：	Allen.L

	创建时间	：	fork zheng39562

	类描述		：	此类目的：封装JSONCPP。增加复杂的操作功能 以及 增加前置判断来减少JSONCPP的崩溃行为。
					Get开头的函数为值传递，Find或其他开头的为引用/指针传递。
					解决解析问题：111{sdsa}ssds解析结果为true，jsonvalue中保存111。
					解决错误索引和未知JSON访问导致异常的问题。（阅读源码，查找解决思路）。

**********************************************************/

class JsonTool
{
	public:
		JsonTool( );
		~JsonTool( );



		/**********************************************************

			函数名		：	parseJs

			功能		：	JSON字符串解析函数。讲JSON字符串解析为Json::Value的类对象。
							解析本质由三方库jsoncpp提供解析。存在部分瑕疵。
							已知问题：111{sdsa}ssds解析结果为true，jsonvalue中保存111。

			参数		：	jsStr 需要被解析的JSON字符串。
							jsValue 解析返回的对象。

			返回值		：	bool     true:表示解析成功    false：表示解析失败

			创建作者	：	Allen.L

			修改记录	：

		**********************************************************/
		static bool parseJs( const string &jsStr, Json::Value &jsValue );


		/**********************************************************

			函数名		：	getJsValue

			功能		：	通过路径获取对应的JSON对象。

			参数		：	path  JSON的索引路径。默认由 '.' 分隔。暂未提供更改的接口。
							jsValue 解析返回的对象。

			返回值		：

			创建作者	：	Allen.L

			修改记录	：

		**********************************************************/
		static Json::Value getJsValue( const Json::Value &jsValue, const string &path );


		/**********************************************************

			函数名		：	getJsValue

			功能		：	通过key-value来索引JSON对象。此函数可在数组中查找所需要的JSON对象并返回。

			参数		：	jsArray JSON对象。此功能通常用来索引数据。
							key JSON对象中包含的KEY值。
							value JSON对象中包含的Value值。

			返回值		：	通常返回索引对象。查询不到是则为空对象

			创建作者	：	Allen.L

			修改记录	：

		**********************************************************/
		static Json::Value getJsValue( const Json::Value &jsArray, const string &key, const string &value );


		/**********************************************************

			函数名		：	findJsValue

			功能		：	与GetJsValue功能基本一致。但Find返回的是当前对象的指针。
							操作指针会影响对应的对象。如果仅需要值，建议使用Get函数

			参数		：	path  JSON的索引路径。默认由 '.' 分隔。暂未提供更改的接口。
							jsValue 解析返回的对象。

			返回值		：

			创建作者	：	Allen.L

			修改记录	：

		**********************************************************/
		static Json::Value* findJsValue( Json::Value &jsValue, const string &path );
		static Json::Value* findJsValue( Json::Value &jsArray, const string &key, const string &value );


		/**********************************************************

			函数名		：	getJsString

			功能		：	获取JS的value值。
							检测并不完善。使用此函数尽量确认其为正确的字符类型。

			参数		：

			返回值		：

			创建作者	：	Allen.L

			修改记录	：

		**********************************************************/
		static string getJsString( const Json::Value &jsValue, string path = "" );


		/**********************************************************

			函数名		：	getJsStrList

			功能		：	JSON对象中获取对应Key值的所有value值。
							该函数目标为JSON数组对象。但其可以对一般JSON对象使用。

			参数		：	strList 返回的value值数组

			返回值		：

			创建作者	：	Allen.L

			修改记录	：

		**********************************************************/
		static bool getJsStrList( const Json::Value &jsArray, const string &key, vector<string> &strList );


		/**********************************************************

			函数名		：	convertJsToMap

			功能		：	转换JSON对象到map对象。

			参数		：	mapResult 保存JSON数据的map对象
							keyList 根据key的列表来转换对应的key。默认为空时，表示所有的key都将被转换。

			返回值		：

			创建作者	：	Allen.L

			修改记录	：

		**********************************************************/
		static bool convertJsToMap( const Json::Value &jsValue, map<string, string> &mapResult, string keyList = "" );


		/**********************************************************

			函数名		：	convertJsToMapList

			功能		：	转换JSON对象到map对象数组。

			参数		：	mapResult 保存JSON数据的map对象
							keyList 根据key的列表来转换对应的key。默认为空时，表示所有的key都将被转换。

			返回值		：	mapListResult 保存JSON数据的map对象数组。

			创建作者	：	Allen.L

			修改记录	：

		**********************************************************/
		static bool convertJsToMapList( const Json::Value &jsValue, vector<map<string, string> > &mapListResult, string keyList = "" );
		static bool convertJsToMapList( const Json::Value &jsValue, list<map<string, string> > &mapListResult, string keyList = "" );


		/**********************************************************

			函数名		：	convertMapToJsStr

			功能		：	map 转换为 JSON对象。

			参数		：	mapData 将被转换的map对象

			返回值		：

			创建作者	：	Allen.L

			修改记录	：

		**********************************************************/
		static string convertMapToJsStr( const map<string, string> mapData );


		/**********************************************************

			函数名		：	convertMapListToJsStr

			功能		：	map数组 转换为 JSON对象

			参数		：	mapList 将被转换的map对象数组。
							arrayName 可以对此数组进行额外命名。默认为空表示不进行命名

			返回值		：

			创建作者	：	Allen.L

			修改记录	：

		**********************************************************/
		static string convertMapListToJsStr( const vector<map<string, string> > &mapList, string arrayName = "" );
		static string convertMapListToJsStr( const list<map<string, string> > &mapList, string arrayName = "" );



	private:

		static string indexOfSplittingPath;			//! 索引路径的默认分隔符
		static string indexOfSplittingKey;			//! 关键字列表的默认分隔符


		/**********************************************************

			函数名		：	parseKeyPath

			功能		：	解析key的索引路径

			参数		：	keys 保存对象的key值

			返回值		：

			创建作者	：	Allen.L

			修改记录	：

		**********************************************************/
		static bool parseKeyPath( const string &path, vector<string> &keys );


		/**********************************************************

			函数名		：	parseKeyList

			功能		：	解析key值的列表。并保存到key的数组中。

			参数		：

			返回值		：

			创建作者	：	Allen.L

			修改记录	：

		**********************************************************/
		static bool parseKeyList( const string &keyList, vector<string> &keys );


		/**********************************************************

			函数名		：	findJsValueByKey

			功能		：	通过key找到对象的JSON对象。

			参数		：

			返回值		：

			创建作者	：	Allen.L

			修改记录	：

		**********************************************************/
		static Json::Value* findJsValueByKey( Json::Value &jsValue, string &key );

	};

}

#endif

