/**********************************************************
 * \file GPStringUtil.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include "Base/Utils/GPStringUtil.h"
using namespace std;

namespace GPUniversal {


//是整数
GPInt32 GPStringUtil::isInt(const GPChar *p) {
	if (p == NULL || (*p) == '\0')
		return 0;
	if ((*p) == '-')
		p++;
	while ((*p)) {
		if ((*p) < '0' || (*p) > '9')
			return 0;
		p++;
	}
	return 1;
}

GPBOOL GPStringUtil::checkNumber(const GPString &number){
	if( number.empty() ){	return false; }
	stringstream sin(number);
	GPDouble doubleTmp;
	GPChar charTmp;
	if( !(sin >> doubleTmp) )
		return false;
	if( sin >> charTmp )
		return false;

	return true;
}

//把int转成string
GPString GPStringUtil::intToStr(const long &number){
	stringstream strStream;
	strStream << number;
	return strStream.str();
}

//把double转成string 使用是std::stringstream形式
GPString GPStringUtil::doubleToStr(const GPDouble &number){
	stringstream strStream;
	strStream << number;
	return strStream.str();
}

//把string转成double
GPDouble GPStringUtil::strToDouble(const GPString &number){
	stringstream strStream;
	strStream << number;

	GPDouble numberTmp;
	strStream >> numberTmp;
	return numberTmp;
}

//把string转成int, d是默认值
GPInt32 GPStringUtil::strToInt(const GPChar *str, GPInt32 d) {
	if (isInt(str)) {
		return atoi(str);
	} else {
		return d;
	}
}

//把string转成int
long GPStringUtil::strToInt(const GPString &number){
	stringstream strStream;
	strStream << number;

	long numberTmp;
	strStream >> numberTmp;
	return numberTmp;
}

//转成小写
GPChar *GPStringUtil::strToLower(GPChar *pszBuf) {
	if (pszBuf == NULL)
		return pszBuf;

	GPChar *p = pszBuf;
	while (*p) {
		if ((*p) & 0x80)
			p++;
		else if ((*p) >= 'A' && (*p) <= 'Z')
			(*p) += 32;
		p++;
	}
	return pszBuf;
}

//转成小写
GPString GPStringUtil::strToLower(const GPString &str){
	GPString strTmp = str;
	transform(strTmp.begin(), strTmp.end(), strTmp.begin(), (int (*)(int))tolower);
	return strTmp;
}

//转成大写
GPChar *GPStringUtil::strToUpper(GPChar *pszBuf) {
	if (pszBuf == NULL)
		return pszBuf;

	GPChar *p = pszBuf;
	while (*p) {
		if ((*p) & 0x80)
			p++;
		else if ((*p) >= 'a' && (*p) <= 'z')
			(*p) -= 32;
		p++;
	}
	return pszBuf;
}

//转成大写
GPString GPStringUtil::strToUpper(const GPString &str){
	GPString strTmp = str;
	transform(strTmp.begin(), strTmp.end(), strTmp.begin(), (int (*)(int))toupper);
	return strTmp;
}

//str转Xml
GPString GPStringUtil::strToXmlStr(GPString content){
	// & -> &amp;
	strReplaceAll(content, "&", "&amp;");
	// < -> &lt;
	strReplaceAll(content, "<", "&lt;");
	// > -> &gt;
	strReplaceAll(content, ">", "&gt;");
	// ' -> &apos;
	strReplaceAll(content, "'", "&apos;");
	// " -> &quot;
	strReplaceAll(content, "\"", "quot;");

	return content;
}

//xml转Str
GPString GPStringUtil::xmlStrToStr(GPString content){
	// & -> &amp;
	strReplaceAll(content, "&amp;", "&");
	// < -> &lt;
	strReplaceAll(content, "&lt;", "<");
	// > -> &gt;
	strReplaceAll(content, "&gt;", ">");
	// ' -> &apos;
	strReplaceAll(content, "&apos;", "'");
	// " -> &quot;
	strReplaceAll(content, "quot;", "\"");

	return content;
}

//转JSON
GPString GPStringUtil::strToJsonStr(GPString content){
	strReplaceAll(content, "\"", "\\\"");
	return content;
}

//JSON转str
GPString GPStringUtil::jsonStrToStr(GPString content){
	strReplaceAll(content, "\\\"", "\"");
	return content;
}

//去前后空格
GPChar *GPStringUtil::trim(GPChar *str, const GPChar *what, GPInt32 mode) {
	GPChar mask[256];
	GPByte *p;
	GPByte *ret;
	memset(mask, 0, 256);
	ret = (GPByte *) str;
	p = (GPByte *) what;
	while (*p) {
		mask[*p] = '\1';
		p++;
	}
	if (mode & 1) { // 前面
		p = ret;
		while (*p) {
			if (!mask[*p]) {
				break;
			}
			ret++;
			p++;
		}
	}
	if (mode & 2) { // 后面
		p = ret + strlen((const GPChar*) ret) - 1;
		while (p >= ret) {
			if (!mask[*p]) {
				break;
			}
			p--;
		}
		p++;
		*p = '\0';
	}
	return (GPChar*) ret;
}

//替换字符串中对应的子字符串
GPBOOL GPStringUtil::strReplaceAll(GPString &content, const GPString &replace, const GPString &dest){
	GPString::size_type pos = 0;
	GPString::size_type replaceSize = replace.size();
	GPString::size_type destSize = dest.size();
	while((pos = content.find(replace, pos)) != GPString::npos){
		content.replace(pos, replaceSize, dest);
		pos+=destSize;
	}

	return true;
}

//通过分割符来分割字符串，并将分割好的数据存在数组中
GPBOOL GPStringUtil::splitString(const GPString &strValue, const GPString &splittingKey, GPStringVector &result){
	result.clear();

	if(strValue.empty()){
		result.push_back("");
		return true;
	}

	int count = 0;
	GPString::size_type curindex = 0;
	GPString::size_type preindex = 0;
	while(1){
		if(++count > strValue.size())  break;

		curindex = strValue.find_first_of(splittingKey, preindex);
		if(curindex == GPString::npos){
			if(preindex != GPString::npos)
				result.push_back(strValue.substr(preindex));
			return true;
		}
		if(preindex != curindex){
			result.push_back(strValue.substr(preindex, curindex-preindex));
		}
		else{
			result.push_back("");
		}
		preindex = curindex + splittingKey.size();
	}
	return false;
}

//通过分割符来分割字符串，并将分割好的数据存在链表中
GPBOOL GPStringUtil::splitString(const GPString &strValue, const GPString &splittingKey, list<GPString> &result){
	result.clear();

	if(strValue.empty()){
		result.push_back("");
		return true;
	}

	int count = 0;
	GPString::size_type curindex = 0;
	GPString::size_type preindex = 0;
	while(1){
		if(++count > strValue.size())  break;

		curindex =	strValue.find_first_of(splittingKey, preindex);
		if(curindex == GPString::npos){
			if(preindex != GPString::npos)
				result.push_back(strValue.substr(preindex));
			return true;
		}
		if(preindex != curindex){
			result.push_back(strValue.substr(preindex, curindex-preindex));
		}
		preindex = curindex + splittingKey.size();
	}
	return false;
}

//合并字符串，使用分隔符分隔
GPString GPStringUtil::mergeString(const GPStringVector &strings, const GPString &mergerKey){
	GPString strTmp;
	for(GPStringVector::const_iterator citer = strings.begin(); citer != strings.end();){
		strTmp += *citer;
		if(++citer != strings.end())
			 strTmp += mergerKey;
	}
	return strTmp;
}

//删除左边(右边)的空白符，制表符，换行符
GPString GPStringUtil::trimRight(const GPString &str){
	GPString trimStr = str.substr(0, str.find_last_not_of(" ")+1);
	trimStr = trimStr.substr(0, trimStr.find_last_not_of("\t")+1);
	trimStr = trimStr.substr(0, trimStr.find_last_not_of("\r\n")+1);
	trimStr = trimStr.substr(0, trimStr.find_last_not_of("\n")+1);
	return trimStr;
}

//
GPString GPStringUtil::trimLeft(const GPString &str){
	GPString trimStr = str.substr(str.find_first_not_of(" ") == GPString::npos ? str.size()-1 : str.find_first_not_of(" "));
	trimStr = trimStr.substr(trimStr.find_first_not_of("\t") == GPString::npos ? str.size()-1 : trimStr.find_first_not_of("\t"));
	trimStr = trimStr.substr(trimStr.find_first_not_of("\r\n") == GPString::npos ? str.size()-1 : trimStr.find_first_not_of("\r\n"));
	trimStr = trimStr.substr(trimStr.find_first_not_of("\n") == GPString::npos ? str.size()-1 : trimStr.find_first_not_of("\n"));
	return trimStr;
}

//
GPString GPStringUtil::trim(const GPString &str){
	return str.empty() ? str : trimRight(trimLeft(str));
}

//把string以delim分隔开,放到list中
GPVOID GPStringUtil::split(GPChar *str, const GPChar *delim, std::vector<GPChar*> &list) {
	if (str == NULL) {
		return;
	}
	if (delim == NULL) {
		list.push_back(str);
		return;
	}

	GPChar* s = str;
	const GPChar* spanp = delim;

	start: while (*s) {
		if (*s == *spanp) {
			GPChar* p = s;
			spanp++;
			s++;
			while (*spanp) {
				if (*s != *spanp) {
					spanp = delim;
					goto start;
				}
				spanp++;
				s++;
			}
			*p = '\0';
			list.push_back(str);
			str = s;
			spanp = delim;
		} else {
			s++;
		}
	}
	if (*str) {
		list.push_back(str);
	}
}

}//namespace GPBase

