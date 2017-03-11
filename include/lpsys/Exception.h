/**********************************************************
 *  \file Exception.h
 *  \brief	
 *  \note	ע����� 
 * 
 * \version 
 * * \author Allen.L
 ***********************************************************/

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <exception>
#include <string>
#include <ostream>

namespace lputil {

/**
 * Exception�Ƕ�std::exception����װ
 */
class Exception: public std::exception {
public:

	Exception();
	Exception(const char*, int);
	Exception(const char*, int, const char*);
	virtual ~Exception() throw ();
	virtual std::string name() const;
	virtual void print(std::ostream&) const;
	virtual const char* what() const throw ();
	virtual Exception* clone() const;
	virtual void _throw() const;
	const char* file() const;
	int line() const;

private:

	const char* _file;
	int _line;
	const char* _desc;
	static const char* _name;
	mutable ::std::string _str;
};

std::ostream& operator <<(std::ostream& out, const Exception& ex);

/**
 * HandleΪ���쳣��
 */
class NullHandleException: public Exception {
public:

	NullHandleException(const char*, int);
	virtual ~NullHandleException() throw ();
	virtual std::string name() const;
	virtual Exception* clone() const;
	virtual void _throw() const;

private:
	static const char* _name;
};

/**
 * ���Ϸ������쳣��
 */
class IllegalArgumentException: public Exception {
public:

	IllegalArgumentException(const char*, int);
	IllegalArgumentException(const char*, int, const std::string&);
	virtual ~IllegalArgumentException() throw ();
	virtual std::string name() const;
	virtual void print(std::ostream&) const;
	virtual Exception* clone() const;
	virtual void _throw() const;

	std::string reason() const;

private:

	static const char* _name;
	std::string _reason;
};

}
#endif /* EXCEPTION_H_ */
