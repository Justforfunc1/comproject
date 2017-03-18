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

	//! \brief	读取文件
	GPString readFile(const GPString &path);
	//! \brief	写入文件（覆盖）
	GPBOOL writeFile(const GPString &path, const GPString &content);
	//! \brief	写入文件（追加）
	GPBOOL addContentToFile(const GPString &path, const GPString &content);
	//! \brief	创建目录。
	//! \return	如果不存在的话会创建，存在或创建失败都会返回false.
	GPBOOL createDir(GPString filePath);
	//! \brief	删除目录下的文件和子目录.
	GPBOOL clearDir(GPString filePath);
	//! \brief	删除文件
	GPBOOL deleteFile(const GPString &filePath);
	//! \brief	查找目录下的所有文件名。
	//! \param[in] filePattern 过滤条件。是正则表达式方式。默认为空，表示不过滤。
	GPBOOL findFileFromDir(GPString filePath, std::vector<GPString> &fileNames, GPString filePattern = "");
	//! \brief	获取绝对路径。
	GPString getAbsPath(GPString filepath = ".");
	//! \brief	获取文件名称（带后缀）
	GPString getFileNameByPath(const GPString &filepath);
	//! \brief	补全路径：如果路径最后一个不是/ 则增加/
	GPString completePath(const GPString &path);

}//namespace GPUniversal

#endif

