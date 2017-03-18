/**********************************************************
 * \file GPFile.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include "gp_base/GPFile.h"
#include <fstream>
#include "boost/regex.hpp"
#include "boost/filesystem.hpp"
#include "gp_base/GPTool.h"

using namespace boost;
using namespace boost::filesystem;

namespace GPUniversal {

	GPString readFile(const GPString &path){
		std::ifstream infile;
		infile.open(path.c_str());
		if(infile){
			oGPStringstream ostr;
			ostr << infile.rdbuf();
			infile.close();
			GPString strTmp = ostr.str();

			return strTmp;
		}
		else{
			PUB_DEBUG_I("open aciton is failed.Please check arg of path:" << path << ".");
			return NULL;
		}
	}
	bool writeFile(const GPString &path, const GPString &content){
		std::ofstream outfile(path.c_str());
		if(outfile){
			outfile << content;
			outfile.close();
			return true;
		}
		else{
			PUB_DEBUG_I("open aciton is failed.Please check arg of path:" << path << ".");
		}
		return false;
	}
	bool addContentToFile(const GPString &path, const GPString &content){
		std::ofstream outfile(path.c_str(), ios::app);
		if(outfile){
			outfile << content;
			outfile.close();
			return true;
		}
		else{
			PUB_DEBUG_I("open aciton is failed.Please check arg of path:" << path << ".");
			return false;
		}
	}

	bool createDir(GPString filePath){
		return (bool)create_directory(path(filePath));
	}


	bool clearDir(GPString filePath){
		return (bool)remove_all(path(filePath));
	}


	bool deleteFile(const GPString &filePath){
		return (bool)remove(path(filePath));
	}

	bool findFileFromDir(GPString filePath, GPStringVector &fileNames, GPString filePattern){
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

	GPString getAbsPath(GPString filepath){
		return system_complete(path(filepath)).string();
	}

	GPString getFileNameByPath(const GPString &filepath){
		path filePath(filepath);
		if(is_directory(filePath)){
			return filePath.relative_path().string();
		}
		else{
			return filePath.parent_path().string();
		}
	}

	GPString completePath(const GPString &path){
		if(!path.empty() && path[path.size()-1 ] != '/'){
			return path + "/";
		}
		return path;
	}

}//namespace GPUniversal
