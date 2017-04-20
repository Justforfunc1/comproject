/**********************************************************
 * \file GPException.h
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#ifndef _GPException_H
#define _GPException_H

#include <exception>
#include <ostream>
#include "Base/Common/GPDefine.h"

namespace GPBase {


/**********************************************************

	GPException		异常处理类

	类名			：	GPException

	作者			：	Allen.L

	创建时间		：

	类描述			：	GPException是对std::exception的卦装

**********************************************************/

class GPException: public std::exception {
public:

	GPException();
	GPException(const GPChar*, GPInt32);
	GPException(const GPChar*, GPInt32, const GPChar*);
	virtual ~GPException() throw ();
	virtual GPString name() const;
	virtual GPVOID print(std::ostream&) const;
	virtual const GPChar* what() const throw ();
	virtual GPException* clone() const;
	virtual GPVOID _throw() const;
	const GPChar* file() const;
	GPInt32 line() const;

private:

	const GPChar* _file;
	GPInt32 _line;
	const GPChar* _desc;
	static const GPChar* _name;
	mutable GPString _str;
};

std::ostream& operator <<(std::ostream& out, const GPException& ex);


/**********************************************************

	NullHandleException		空异常处理类

	类名			：	NullHandleException

	作者			：	Allen.L

	创建时间		：

	类描述			：

**********************************************************/

class NullHandleException: public GPException {
public:

	NullHandleException(const GPChar*, GPInt32);
	virtual ~NullHandleException() throw ();
	virtual GPString name() const;
	virtual GPException* clone() const;
	virtual GPVOID _throw() const;

private:
	static const GPChar* _name;
};



/**********************************************************

	IllegalArgumentException		不合法参数异常类

	类名			：	IllegalArgumentException

	作者			：	Allen.L

	创建时间		：

	类描述			：

**********************************************************/

class IllegalArgumentException: public GPException {
public:

	IllegalArgumentException(const GPChar*, GPInt32);
	IllegalArgumentException(const GPChar*, GPInt32, const GPString&);
	virtual ~IllegalArgumentException() throw ();
	virtual GPString name() const;
	virtual GPVOID print(std::ostream&) const;
	virtual GPException* clone() const;
	virtual GPVOID _throw() const;

	GPString reason() const;

private:

	static const GPChar* _name;
	GPString _reason;
};



/**********************************************************

	SyscallException		系统调用异常类

	类名			：	SyscallException

	作者			：	Allen.L

	创建时间		：

	类描述			：

**********************************************************/

class SyscallException : public GPException	{
public:
    SyscallException( const GPChar* , GPInt32 );
    SyscallException(const GPChar*, GPInt32, GPInt32);
    virtual GPString name() const;
    virtual GPVOID print(std::ostream&) const;
    virtual GPException* clone() const;
    virtual GPVOID _throw() const;

    int error() ;

    int _error;
    static const char* _name;
};

}

#endif

