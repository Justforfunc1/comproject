/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file exception_util.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-07-06
 ** \last modified 2017-07-19 16:23:23
**********************************************************/

#include "exception_util.h"

namespace base {

Exception::Exception()
	: file_(0), line_(0), desc_(NULL) {
}

Exception::Exception(const char *file, int32 line)
	: file_(file), line_(line), desc_(NULL) {
}

Exception::Exception(const char *file, int32 line, const char *desc)
	: file_(file), line_(line), desc_(desc) {
}

Exception::~Exception() throw() {
}

const char *Exception::name_ = "base::Exception";

std::string Exception::name() const {
	return name_;
}

void Exception::print(std::ostream &out) const {
	if (file_ && line_ > 0) {
		out << file_ << ':' << line_ << ": ";
		if (desc_) {
			out << desc_ << ": ";
		}
	}
	out << name();
}

const char *Exception::what() const throw() {
	try {
		if (str_.empty()) {
			std::stringstream s;
			print(s);
			str_ = s.str();
		}
		return str_.c_str();
	} catch (...) {
	}
	return "";
}

Exception *Exception::clone() const {
	return new Exception(*this);
}

void Exception::_throw() const {
	throw *this;
}

const char *Exception::file() const {
	return file_;
}

int32 Exception::line() const {
	return line_;
}

std::ostream& operator <<(std::ostream &out, const Exception &ex) {
	ex.print(out);
	return out;
}

NullHandleException::NullHandleException(const char * file, int32 line)
	: Exception(file, line) {
	abort();
}

NullHandleException::~NullHandleException() throw () {
}

const char *NullHandleException::name_ = "NullHandleException";

std::string NullHandleException::name() const {
	return name_;
}

Exception* NullHandleException::clone() const {
	return new NullHandleException(*this);
}

void NullHandleException::_throw() const {
	throw *this;
}

IllegalArgumentException::IllegalArgumentException(const char *file, int32 line)
	: Exception(file, line) {
}

IllegalArgumentException::IllegalArgumentException(const char *file, int32 line, const std::string &r)
	: Exception(file, line), reason_(r) {
}

IllegalArgumentException::~IllegalArgumentException() throw() {
}

const char *IllegalArgumentException::name_ = "IllegalArgumentException";

std::string IllegalArgumentException::name() const {
	return name_;
}

void IllegalArgumentException::print(std::ostream &out) const {
	Exception::print(out);
	out << ": " << reason_;
}

Exception *IllegalArgumentException::clone() const {
	return new IllegalArgumentException(*this);
}

void IllegalArgumentException::_throw() const {
	throw *this;
}

std::string IllegalArgumentException::reason() const {
	return reason_;
}

const char *SyscallException::name_ = "SyscallException";

std::string SyscallException::name() const {
    return name_;
}

void SyscallException::print(std::ostream &os) const {
    Exception::print(os);
    if(error_ != 0) { ; }
}

Exception *SyscallException::clone() const {
    return new SyscallException(*this);
}

void SyscallException::_throw() const {
    throw *this;
}

int32 SyscallException::error() {
    return error_;
}

} //base
