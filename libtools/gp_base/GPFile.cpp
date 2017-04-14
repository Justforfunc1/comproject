/**********************************************************
 * \file GPFile.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include "gp_base/GPFile.h"
#include "boost/regex.hpp"
#include "boost/filesystem.hpp"
#include "gp_base/GPLogger.h"

using namespace boost;
using namespace std;
using namespace boost::filesystem;

namespace GPUniversal {


//读取文件
GPString GPFile::readFile(const GPString &path){
	std::ifstream infile;
	infile.open(path.c_str());
	if(infile){
		ostringstream ostr;
		ostr << infile.rdbuf();
		infile.close();
		GPString strTmp = ostr.str();

		return strTmp;
	}
	else{
		GPLOG_DEBUG("open aciton is failed.Please check arg of path:[%s]", path.c_str());
		return NULL;
	}
}

//写入文件（覆盖）
GPBOOL GPFile::writeFile(const GPString &path, const GPString &content){
	std::ofstream outfile(path.c_str());
	if(outfile){
		outfile << content;
		outfile.close();
		return true;
	}
	else{
		GPLOG_DEBUG("open aciton is failed.Please check arg of path:[%s]", path.c_str());
	}
	return false;
}

//写入文件（追加）
GPBOOL GPFile::addContentToFile(const GPString &path, const GPString &content){
	std::ofstream outfile(path.c_str(), ios::app);
	if(outfile){
		outfile << content;
		outfile.close();
		return true;
	}
	else{
		GPLOG_DEBUG("open aciton is failed.Please check arg of path:[%s]", path.c_str());
		return false;
	}
}

//创建目录
GPBOOL GPFile::createDir(GPString filePath){
	return (GPBOOL)create_directory(path(filePath));
}

//删除目录下的文件和子目录
GPBOOL GPFile::clearDir(GPString filePath){
	return (GPBOOL)remove_all(path(filePath));
}

//删除当前文件
GPBOOL GPFile::deleteFile(const GPString &filePath){
	return (GPBOOL)remove(path(filePath));
}

//查找目录下的所有文件名
GPBOOL GPFile::findFileFromDir(GPString filePath, GPStringVector &fileNames, GPString filePattern){
	path pPath(filePath);
	if(is_directory(pPath)){
		directory_iterator end;
		for (directory_iterator citer(pPath); citer != end; ++citer){
			if(!is_directory(*citer)){
				GPString fileName = citer->path().string();
				if(!filePattern.empty()){
					boost::regex reg(filePattern);
					if (!boost::regex_match(fileName, reg)){
						continue;
					}
				}
				fileNames.push_back(fileName);
			}
		}
	}

	return true;
}

//获取绝对路径件和子目录
GPString GPFile::getAbsPath(GPString filepath){
	return system_complete(path(filepath)).string();
}

//获取文件名称（带后缀）
GPString GPFile::getFileNameByPath(const GPString &filepath){
	path filePath(filepath);
	if(is_directory(filePath)){
		return filePath.relative_path().string();
	}
	else{
		return filePath.parent_path().string();
	}
}

//补全路径：如果路径最后一个不是/ 则增加/
GPString GPFile::completePath(const GPString &path){
	if(!path.empty() && path[path.size()-1 ] != '/'){
		return path + "/";
	}
	return path;
}


}//namespace GPUniversal
