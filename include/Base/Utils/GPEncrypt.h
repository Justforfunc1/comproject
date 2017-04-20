/**********************************************************
 * \file GPEncrypt.h
 * \brief	字符编码通用函数类
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#ifndef _GPEncrypt_H
#define _GPEncrypt_H

#include "Base/Common/GPDefine.h"
#include "Base/Auth/GPMd5.h"
#include "Base/Auth/GPRc4.h"

namespace GPUniversal
{

//	编码格式枚举
enum eCharacterEncoding{
	eEncoding_FirstInvalid,
	eEncoding_UTF8,
	eEncoding_Unicode,
	eEncoding_GBK,
	eEncoding_LastInvalid,
};


/**********************************************************

	GPEncrypt	字符编码处理类

	类名		：	GPEncrypt

	作者		：	Allen.L

	创建时间	：

	类描述		：	字符的编码 加/解密

**********************************************************/

class GPEncrypt {

public:


	/**********************************************************

		函数名		：	urlEncode
		函数名		：	urlDecode

		功能		：	url编码
		功能		：	url解码

		参数		：	char*	src		源字符串
		参数		：	char*	dest	目标字符串

		返回值		：	0：成功		other：失败

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPInt32 urlEncode(const GPChar* src, GPChar* dest);
	static GPInt32 urlDecode(const GPChar *src, GPChar *dest);


	/**********************************************************

		函数名		：	ConvertToUtf8	ConvertUnicodeToUtf8

		功能		：	unicode编码格式 转换为UFT8
		功能		：	unicode编码格式 转换为UFT8 。注意unicode为16位

		参数		：	string	src		源字符串
		参数		：	eCharacterEncoding	originalEncoding	编码参数
		参数		：	int	unicode		待转码code

		返回值		：	0：成功		other：失败

		创建作者	：	Allen.L

		修改记录	：	todo 未完成

	**********************************************************/
	static GPVOID ConvertToUtf8(GPString &str, const eCharacterEncoding &originalEncoding);
	static GPString ConvertUnicodeToUtf8(GPInt32 &unicode);


	/**********************************************************

		函数名		：	convertFullToHalf
		函数名		：	convertHalfToFull

		功能		：	字符 全角转半角 的函数
		功能		：	字符 半角转全角 的函数

		参数		：	string	str	输出及输出
		参数		：	eCharacterEncoding	encoding	编码参数，暂时仅支持UFT8

		返回值		：	0：成功		other：失败

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPVOID convertFullToHalf(GPString &str, eCharacterEncoding encoding = eEncoding_UTF8);
	static GPVOID convertHalfToFull(GPString &str, eCharacterEncoding encoding = eEncoding_UTF8);


	/**********************************************************

		函数名		：	getStrSize

		功能		：	得到不同编码的字符长度

		参数		：	string	str	输出及输出
		参数		：	eCharacterEncoding	encoding	编码参数，暂时仅支持UFT8

		返回值		：

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static size_t getStrSize(const GPString &str, eCharacterEncoding encoding = eEncoding_UTF8);


	/**********************************************************

		函数名		：	base64Encode	base64Decode

		功能		：	base64编码
		功能		：	base64解码

		参数		：	string	str	输出及输出
		参数		：	bool	urlencode	编码参数

		返回值		：	GPString

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPString base64Encode(const GPString& str, GPBOOL urlencode = false);
	static GPString base64Decode(const GPString& str, GPBOOL urlencode = false);


	/**********************************************************

		函数名		：	rc4String

		功能		：	RC4加密

		参数		：	string	str	输出及输出
		参数		：	string	key	加密密钥

		返回值		：	GPBOOL

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPString rc4String(const GPString& key, const GPString& str);
	static GPBOOL rc4_encrypt(GPChar *content, GPInt32 contentLength, const GPChar* sKey, GPInt32 ketLength);
	static GPBOOL rc4_encrypt(GPString &content, const GPString &sKey);
	static GPBOOL rc4_encrypt(GPByte* content, GPUInt32 size, const GPString &sKey);


	/**********************************************************

		函数名		：	myStrEncrypt
		函数名		：	myStrDecrypt
		函数名		：	convertBinaryToHexString
		函数名		：	convertHexStringToBinary

		功能		：	字符串版本的rc4(加/解密)	先用rc4转换二进制，然后将二进制转为十六进制的字符串
		功能		：	二进制与十六进制的字符串互转

		参数		：	string	content	输出内容
		参数		：	string	key	加密密钥

		返回值		：	GPBOOL

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPBOOL myStrEncrypt(GPString &content, const GPString &key);
	static GPBOOL myStrDecrypt(GPString &content, const GPString &key);
	static GPBOOL convertBinaryToHexString(GPString &content);
	static GPBOOL convertHexStringToBinary(GPString &content);


	/**********************************************************

		函数名		：	md5
		函数名		：	md5_16

		功能		：	生成MD5	默认为32位
		功能		：	生成MD5(16位)

		参数		：	char*	src		源字符串
		参数		：	char*	dest	目标字符串

		返回值		：	0：成功		other：失败

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPString md5(const GPString str);
	static GPString md5_16(const GPString str);


	/**********************************************************

		函数名		：	encryptId32
		函数名		：	decryptId32
		函数名		：	encryptId64
		函数名		：	encryptId64

		功能		：	ID加/解密（32位）
		功能		：	ID加/解密（64位）

		参数		：	GPUInt32,GPUInt64	id		待加密ID
		参数		：	GPUInt32,GPUInt64	key		加密key

		返回值		：	0：成功		other：失败

		创建作者	：	Allen.L

		修改记录	：

	**********************************************************/
	static GPUInt32 encryptId32(GPUInt32 id, GPUInt32 key);
	static GPUInt32 decryptId32(GPUInt32 id, GPUInt32 key);
	static GPUInt64 encryptId64(GPUInt64 id, GPUInt64 key);
	static GPUInt64 decryptId64(GPUInt64 id, GPUInt64 key);

};

}//namespace GPUniversal
#endif

