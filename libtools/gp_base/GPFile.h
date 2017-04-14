/**********************************************************
 * \file GPFile.h
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/
#ifndef _GPFile_H
#define _GPFile_H

#include "gp_base/GPDefine.h"

namespace GPUniversal {


/**********************************************************

	GPFile			文件处理类

	类名		：	GPFile

	作者		：	Allen.L

	创建时间	：

	类描述		：

**********************************************************/

class GPFile {

public:


	/**********************************************************

		函数名		：	readFile

		功能		：	读取文件

		参数		：	string	path	文件路径

		返回值		：	0：成功		other：失败

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPString readFile(const GPString &path);


	/**********************************************************

		函数名		：	writeFile
		函数名		：	addContentToFile

		功能		：	写入文件（覆盖）
		功能		：	写入文件（追加）

		参数		：	string	path		文件路径
		参数		：	string	content		文件内容

		返回值		：	GPBOOL

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPBOOL writeFile(const GPString &path, const GPString &content);
	GPBOOL addContentToFile(const GPString &path, const GPString &content);


	/**********************************************************

		函数名		：	createDir

		功能		：	创建目录	如果不存在的话会创建，存在或创建失败都会返回false.

		参数		：	string	filePath	文件路径

		返回值		：	GPBOOL

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPBOOL createDir(GPString filePath);


	/**********************************************************

		函数名		：	clearDir
		函数名		：	deleteFile

		功能		：	删除目录下的文件和子目录
		功能		：	删除当前文件

		参数		：	string	filePath	文件路径
		参数		：	string	filePath	文件路径

		返回值		：	GPBOOL

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPBOOL clearDir(GPString filePath);
	GPBOOL deleteFile(const GPString &filePath);


	/**********************************************************

		函数名		：	findFileFromDir

		功能		：	查找目录下的所有文件名	如果不存在的话会创建，存在或创建失败都会返回false.

		参数		：	string	filePath	文件路径
		参数		：	string	fileNames	文件名
		参数		：	string	filePattern	过滤条件。是正则表达式方式。默认为空，表示不过滤。

		返回值		：	GPBOOL

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPBOOL findFileFromDir(GPString filePath, std::vector<GPString> &fileNames, GPString filePattern = "");


	/**********************************************************

		函数名		：	getAbsPath
		函数名		：	getFileNameByPath
		函数名		：	completePath

		功能		：	获取绝对路径件和子目录
		功能		：	获取文件名称（带后缀）
		功能		：	补全路径：如果路径最后一个不是/ 则增加/

		参数		：	string	filePath	文件路径
		参数		：	string	filePath	文件路径
		参数		：	string	path	文件路径

		返回值		：	GPString

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	GPString getAbsPath(GPString filepath = ".");
	GPString getFileNameByPath(const GPString &filepath);
	GPString completePath(const GPString &path);

};

}//namespace GPUniversal

#endif

